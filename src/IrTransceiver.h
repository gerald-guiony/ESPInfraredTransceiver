//************************************************************************************************************************
// IrTransceiver.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

//------------------------------------------------------------------------------
// Include the right Arduino header
//
#if defined(ARDUINO) && (ARDUINO >= 100)
#	include <Arduino.h>
#endif

#include "IrPacket.h"

#define IR_CAPTURE_DELAY_MS		5000		// you may increase this value for long ir signal


//------------------------------------------------------------------------------
class IrTransceiver
{
private:
	// WARNING : _sendPin and _recvPin must be differents !!!
	uint8_t _sendPin;
	uint8_t _recvPin;

	unsigned int _halfPeriodicTime;

private:
	static void custom_delay_usec	(unsigned long uSecs);

	void mark				(uint16_t time);
	void space				(uint16_t time);
	void enableIROut		(int khz);

public:
	IrTransceiver			(uint8_t sendPin, uint8_t recvPin);

	void sendRaw			(IRPACKET & irPacket, unsigned int hz);
	void recordRaw			(IRPACKET & irPacket, unsigned int delayMs = IR_CAPTURE_DELAY_MS);
};












