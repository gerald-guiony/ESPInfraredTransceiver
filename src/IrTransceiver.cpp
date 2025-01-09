//************************************************************************************************************************
// IrTransceiver.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include "IrTransceiver.h"


// #ifdef ESP8266
// 	unsigned int pwmRange = 1023;										// PWMRANGE = 1023 by default
// 	//unsigned int pwmRange = 255;										// Force to 255 to speed the code up
// #else
// 	unsigned int pwmRange = 255;
// #endif


//========================================================================================================================
// Constructor
//========================================================================================================================
IrTransceiver :: IrTransceiver (uint8_t sendPin, uint8_t recvPin) :
	_sendPin (sendPin), _recvPin (recvPin)
{
}

//========================================================================================================================
// Custom delay function that circumvents Arduino's delayMicroseconds limit
// (the largest value that will produce an accurate delay is 16383 !!)
//========================================================================================================================
void IrTransceiver :: custom_delay_usec (unsigned long uSecs)
{
/* 	if (uSecs > 4) {
		unsigned long start = micros();										// On 16 MHz Arduino boards (e.g. Duemilanove and Nano), this function has a resolution of 4 microseconds (i.e. the value returned is always a multiple of four)
		unsigned long endMicros = start + uSecs - 4;
		if (endMicros < start) { 											// Check if overflow
			while ( micros() > start ) {} 									// wait until overflow
		}
		while ( micros() < endMicros ) {}									// normal wait
	} */

	static unsigned long time;

	time = uSecs;
	while (time > 16383) {													// delayMicroseconds is only accurate to 16383us.
		delayMicroseconds (16383);
		time -= 16383;
	}
	delayMicroseconds (time);
}

//========================================================================================================================
// Sends an IR mark for the specified number of microseconds.
// The mark output is modulated at the PWM frequency.
//========================================================================================================================
void IrTransceiver :: mark (uint16_t time)
{
#ifdef ESP8266

// Version 1
/*	// PWMRANGE is equal to 1023 by default for ESP8266 and 255 for Arduino
	analogWrite (IR_SEND_PIN, ((pwmRange+1) >> 1));							// (PWMRANGE >> 1) PWMRANGE divise par 2 => 50% de PWMRANGE, Half periodic time
	//delayMicroseconds(time);												// the largest value that will produce an accurate delay is 16383 !!
	custom_delay_usec(time);
*/

// Version 2
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
	unsigned long start = micros();
	unsigned long endMicros = start + time;
	while (micros() < endMicros) {
		digitalWrite (_sendPin, HIGH);
		delayMicroseconds (_halfPeriodicTime);
		digitalWrite (_sendPin, LOW);
		delayMicroseconds (_halfPeriodicTime);
	}

#else
	TIMER_ENABLE_PWM; 														// For Arduino Enable pin 3 PWM output
	//delayMicroseconds(time);												// the largest value that will produce an accurate delay is 16383 !!
	custom_delay_usec(time);
#endif
}

//========================================================================================================================
// Leave pin off for time (given in microseconds)
// Sends an IR space for the specified number of microseconds.
// A space is no output, so the PWM output is disabled.
//========================================================================================================================
void IrTransceiver :: space (uint16_t time)
{
#ifdef ESP8266
	analogWrite (_sendPin, 0);
	digitalWrite(_sendPin, LOW);
#else
	TIMER_DISABLE_PWM; 														// For Arduino Disable pin 3 PWM output
#endif
	//delayMicroseconds(time);
	custom_delay_usec(time);
}

//========================================================================================================================
// Enables IR output.  The khz value controls the modulation frequency in kilohertz.
// The IR output will be on pin 3 (OC2B).
// This routine is designed for 36-40KHz; if you use it for other values, it's up to you
// to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
// TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
// controlling the duty cycle.
// There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
// To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
// A few hours staring at the ATmega documentation and this will all make sense.
// See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.
//========================================================================================================================
void IrTransceiver :: enableIROut (int khz)
{
#ifdef ESP8266

	// Enables IR output.
	pinMode (_sendPin, OUTPUT);
	analogWrite (_sendPin, 0);
	digitalWrite (_sendPin, LOW);

	// --------------------------------- Analog output ---------------------------------------------
	// analogWrite(pin, value) enables software PWM on the given pin. PWM may be used on pins 0 to 16.
	// Call analogWrite(pin, 0) to disable PWM on the pin. value may be in range from 0 to PWMRANGE,
	// which is equal to 1023 by default. PWM range may be changed by calling analogWriteRange(new_range).
	// PWM frequency is 1kHz by default. Call analogWriteFreq(new_frequency) to change the frequency.
	//
	// You can set the PWM-frequency higher or lower if you wish with AnalogWriteFreq(), but if you set
	// it to 10KHz or more you should drop range to 8-bit values to speed the code up, ie. AnalogWriteRange(255)

	// https://github.com/StefanBruens/ESP8266_new_pwm

// Version 1
/*	analogWriteRange (pwmRange);
	analogWriteFreq (khz * 1000); 											// PWM frequency is 1kHz by default
*/

// Version 2
	// The khz value controls the modulation frequency in kilohertz.
	// T = 1/f but we need T/2 in microsecond and f is in kHz
	_halfPeriodicTime = 500 / khz;

#else
	// Disable the Timer2 Interrupt (which is used for receiving IR)
	TIMER_DISABLE_INTR; 													// Timer2 Overflow Interrupt

	pinMode(_sendPin, OUTPUT);
	digitalWrite(_sendPin, LOW);											// When not sending PWM, we want it low

	// COM2A = 00: disconnect OC2A
	// COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
	// WGM2 = 101: phase-correct PWM with OCRA as top
	// CS2  = 000: no prescaling
	// The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
	TIMER_CONFIG_KHZ(khz);
#endif
}








volatile unsigned long lastMicros		= 0;								// Volatile because changed by ISR, déclaration variable volatile => stockée en RAM et pas dans un registre
volatile unsigned long currentMicros	= 0;								// Volatile because changed by ISR

volatile uint16_t * irBufferPtr			= nullptr;							// Pointer thru irBuffer - volatile because changed by ISR
volatile uint16_t * irBufferPtrMax		= nullptr;							// Pointer to irBuffer end - volatile because changed by ISR


//========================================================================================================================
//
//========================================================================================================================
void IrTransceiver :: sendRaw (IRPACKET & irPacket, unsigned int hz)
{
	irBufferPtr		= &(irPacket.data[0]);
	irBufferPtrMax	= irBufferPtr + irPacket.length;

	enableIROut(hz);														// Set IR carrier frequency

	ESP.wdtDisable();														// Disable the software watchdog (https://techtutorialsx.com/2017/01/21/esp8266-watchdog-functions/)
	noInterrupts();															// Some functions will not work while interrupts are disabled, and incoming communication may be ignored. Interrupts can slightly disrupt the timing of code, however, and may be disabled for particularly critical sections of code.
	{
		unsigned int i = 0;

		while (irBufferPtr < irBufferPtrMax) {

			if (i & 1)	space(*irBufferPtr);
			else		mark (*irBufferPtr);

			irBufferPtr++; i++;
		}

		space(0);															// Always end with the LED off
	}
	interrupts();															// Re-enables interrupts (after they've been disabled by noInterrupts()). Interrupts allow certain important tasks to happen in the background and are enabled by default.
	ESP.wdtEnable(0);														// Re-enable the software watchdog (The ‘0’ is an arbitrary number that is required but not used)
}




//========================================================================================================================
// Interrupt Service Routines (ISR) handler has to be marked with ICACHE_RAM_ATTR
//
// By default, all functions are compiled into flash, which means that the cache may kick in for that code. However, the
// cache currently can’t be used during hardware interrupts. That means that, if you use a hardware ISR, such as
// attachInterrupt(gpio, myISR, CHANGE) for a GPIO change, the ISR must have the ICACHE_RAM_ATTR attribute declared. Not
// only that, but the entire function tree called from the ISR must also have the ICACHE_RAM_ATTR declared. Be aware that
// every function that has this attribute reduces available memory.
//
// In addition, it is not possible to execute delay() or yield() from an ISR, or do blocking operations, or operations that
// disable the interrupts.
//
// Finally, an ISR has very high restrictions on timing for the executed code, meaning that executed code should not take
// longer than a very few microseconds. It is considered best practice to set a flag within the ISR, and then from within
// the loop() check and clear that flag, and execute code
//========================================================================================================================
#ifdef ESP8266
void IRAM_ATTR _ISR_ir_recv_pin () {
#else
void _ISR_ir_recv_pin () {
#endif

	if (irBufferPtr >= irBufferPtrMax) return;								// ignore if irBuffer is already full

	// micros () : Returns the number of microseconds since the Arduino board began running the current program. This number
	// will overflow (go back to zero), after approximately 70 minutes. On 16 MHz Arduino boards (e.g. Duemilanove and Nano),
	// this function has a resolution of four microseconds (i.e. the value returned is always a multiple of four). On 8 MHz
	// Arduino boards (e.g. the LilyPad), this function has a resolution of eight microseconds.

	currentMicros = micros();												// just continually record the time-stamp of signal transitions
	if (lastMicros)															// not the first value !
	{
		*irBufferPtr = currentMicros - lastMicros;							// Elapsed time (WARNING : unsigned long caste en unsigned int !!!)
		irBufferPtr++;
	}
	lastMicros = currentMicros;

	// Uncomment to check the amount of time of this method :) => I found 8 micro secondes with div4 = false!!
	// irBuffer[irBufferLen-1] = micros() - currentMicros;
}


//========================================================================================================================
//
//========================================================================================================================
void IrTransceiver :: recordRaw (IRPACKET & irPacket, unsigned int delayMs /*= IR_DEFAULT_CAPTURE_DELAY_MS*/) {

	lastMicros = currentMicros = 0;

	irBufferPtr		= &(irPacket.data[0]);
	irBufferPtrMax	= irBufferPtr + IR_MAX_LEN;

	// ---------------------------------------------------------------------

	pinMode (_recvPin, INPUT);

	ESP.wdtDisable();														// Disable the software watchdog. Don't stop software watchdog too long (less than 6 seconds), otherwise it will trigger hardware watchdog reset. HOWEVER there’s still the hardware watchdog and you cannot disable that one!
//	noInterrupts();															// Some functions will not work while interrupts are disabled, and incoming communication may be ignored. Interrupts can slightly disrupt the timing of code, however, and may be disabled for particularly critical sections of code.
	{
		byte interruptNumber = digitalPinToInterrupt(_recvPin);				// WARNING : 2 and 3 are the digital pins usable for Interrupts for boards Uno, Nano, Mini, other 328-based.. but for ESP8266 Interrupts may be attached to any GPIO pin, except GPIO16 :)
		attachInterrupt(interruptNumber, _ISR_ir_recv_pin, CHANGE);			// set up ISR for receiving IR signal on digital pin 2
//		delay(delayMs * 1000);												// pause 5 secs to capture IR signal
		custom_delay_usec (delayMs * 1000);									// Can not use yield or delay or any function that uses them inside the callbacks of ESPAsyncWebServer..
		detachInterrupt(interruptNumber);									// stop interrupts & capture until finished here
	}
//	interrupts();															// Re-enables interrupts (after they've been disabled by noInterrupts()). Interrupts allow certain important tasks to happen in the background and are enabled by default.
	ESP.wdtEnable(0);														// Re-enable the software watchdog (The ‘0’ is an arbitrary number that is required but not used)

	// ---------------------------------------------------------------------

	irPacket.length = irBufferPtr - &(irPacket.data[0]);					// Update recorded signal length

	if (irPacket.length < IR_MIN_LEN) irPacket.length = 0;					// Check if the captured signal is light noise..

	if (irPacket.length > 1) {												// if a signal is captured
		if (!(irPacket.length & 0x1)) {										// irPacket.length doit etre impair si ce n'est pas le cas c'est que la premiere valeur correspond a du bruit "lumineux"...
			irPacket.length--;
			memmove (
				irPacket.data,
				&(irPacket.data[1]),
				irPacket.length * sizeof(uint16_t));
		}
	}
}

