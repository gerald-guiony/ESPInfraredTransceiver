//************************************************************************************************************************
// IrPacketStorage.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>

#include "IrPacket.h"

#define IR_HEADER_NAMEFILE					"Ir"
#define IR_EXT_NAMEFILE						".dat"


namespace infrax {

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class IrPacketStorage
{
	SINGLETON_CLASS (IrPacketStorage)

public:

	bool 	read								(uint8_t fileId, IRPACKET & irPacket);
	bool	write								(uint8_t fileId, const IRPACKET & irPacket);
	bool	remove								(uint8_t fileId);
	String	printIdList							();
};

}











