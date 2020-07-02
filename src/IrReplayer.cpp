//************************************************************************************************************************
// IrReplayer.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include "IrReplayer.h"



SINGLETON_IMPL (IrReplayer)


//========================================================================================================================
//
//========================================================================================================================
bool IrReplayer :: recordSignal (Print & out, IRPACKET & irPacket) {

	setModulesPower(true);
	{
		_irTransceiver->recordRaw (irPacket);
	}
	setModulesPower(false);

	if (irPacket.length < 1) {
		out << F("No captured InfraRed signal, please try again") << LN;
		return false;
	}

	blinkStatus (irPacket.length / 50);
	
	out << F("SUCCESS! InfraRed signal captured (") << irPacket.length << " words)" << LN;
	return true;
}

//========================================================================================================================
//
//========================================================================================================================
bool IrReplayer :: emmitSignal (Print & out, IRPACKET & irPacket) {

	if (irPacket.length > 0) {

		setModulesPower(true);
		{
			// Assume 38 KHz
			_irTransceiver->sendRaw (irPacket, 38);
		}
		setModulesPower(false);
		
		// Visual indicator that signal sent
		blinkStatus (irPacket.length / 50);

		out << F("InfraRed signal sent (") << irPacket.length << " words)" << LN;
	}
	else
	{
		out << F("No InfraRed signal found, record one first") << LN;
		return false;
	}
	return true;
}

//========================================================================================================================
//
//========================================================================================================================
void IrReplayer :: setup (uint8_t sendPin, uint8_t recvPin) {

	_irTransceiver = new IrTransceiver (sendPin, recvPin);
}










