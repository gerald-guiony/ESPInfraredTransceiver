//************************************************************************************************************************
// IrPacket.h
// Version 1.0 April, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

/**
 * Buffer and data lengths
 */
#define IR_MIN_LEN			20

#ifdef ESP8266
#	define IR_MAX_LEN		1000
#else
#	define IR_MAX_LEN		585			// you may increase this value on Arduinos with greater than 2k SRAM
#endif

/**
 * Packet printable
 */
#define PRINT_IR_RAW_BEGIN					"["
#define PRINT_IR_RAW_HEADER_BEGIN			"Raw ("
#define PRINT_IR_RAW_HEADER_END				") "
#define PRINT_IR_RAW_MARKSPACE				"-"
#define PRINT_IR_RAW_SEPARATOR				", "
#define PRINT_IR_RAW_END					"]"


/**
 * Class: IRPACKET
 *
 * Description:
 * Infrared data packet class
 */
class IRPACKET : public Printable
{
 public:
	uint16_t length				= 0;
	uint16_t data [IR_MAX_LEN]	= {0};

private:
	bool _printTo 				(Print & p) const;
	size_t _printSize			() const;

 public:
    virtual size_t printTo		(Print & p) const override;
    bool parse 					(Stream & stream, Print & out);
};