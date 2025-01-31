//************************************************************************************************************************
// Settings.h
// Version 1.0 October, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once


//------------------------------------------------------------------------------
//

// Add MQTT subscriber & publisher
#define USING_DOMOTICZ_MQTT


//------------------------------------------------------------------------------
// Pins
//

#include <EspBoardDefs.h>

#ifdef ARDUINO_ESP8266_WIO_NODE

	// WARNING : IR_SEND_PIN and IR_RECV_PIN must be differents !!!
#	define IR_SEND_PIN			PORT1B
#	define IR_RECV_PIN			PORT0B


#elif defined(ARDUINO_ESP8266_NODEMCU_ESP12E)

	// IR
#	define IR_SEND_PIN			D1
#	define IR_RECV_PIN			D2

#elif defined(ESP32)


#else

#	define IR_SEND_PIN 			TIMER_PWM_PIN
#	define IR_RECV_PIN			2 // 2 and 3 are the digital pins usable for Interrupts for boards Uno, Nano, Mini, other 328-based.. but for ESP8266 Interrupts may be attached to any GPIO pin, except GPIO16 :)

#endif


//------------------------------------------------------------------------------
// Domoticz MQTT
//

#define MQTT_DOMOTICZ_ENDPOINT			"192.168.1.138"		// IP Broker MQTT
#define MQTT_DOMOTICZ_PORT				1883				// Port Broker MQTT

// Daikin component
#define MQTT_DOMOTICZ_DAIKIN_SWITCH_IDX 1
#define MQTT_DOMOTICZ_LOG_IDX			17