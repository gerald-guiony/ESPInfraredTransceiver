//************************************************************************************************************************
// IrReplayer.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>

#include "IrPacket.h"
#include "IrPacketStorage.h"
#include "IrTransceiver.h"


//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class IrReplayer
{
	SINGLETON_CLASS(IrReplayer)

private:
	IrTransceiver * _irTransceiver		= nullptr;
	IRPACKET _currentIrSignal;	// Current IR Signal

private:
	bool recordSignal			(Print & out, IRPACKET & irPacket);
	bool emmitSignal			(Print & out, IRPACKET & irPacket);

public:
	IRPACKET & currentSignal	() 				{ return _currentIrSignal;									}

	bool recordSignal			(Print & out) 	{ return recordSignal	(out, _currentIrSignal);			}
	bool emmitSignal			(Print & out) 	{ return emmitSignal	(out, _currentIrSignal);			}

	bool saveSignal 			(uint8_t id)	{ return I(IrPacketStorage).write (id, _currentIrSignal);	}
	bool loadSignal 			(uint8_t id)	{ return I(IrPacketStorage).read  (id, _currentIrSignal);	}

	void setup					(uint8_t sendPin, uint8_t recvPin);
};


