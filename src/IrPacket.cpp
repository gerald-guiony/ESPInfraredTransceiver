//************************************************************************************************************************
// IrPacket.h
// Version 1.0 April, 2019
// Author Gerald Guiony
//************************************************************************************************************************

#include <Stream.h>

#include <Common.h>

#include "IrPacket.h"



//========================================================================================================================
//
//========================================================================================================================
bool IRPACKET :: _printTo (Print & p) const {

	bool isValid = (0 < length) && (length <= IR_MAX_LEN);

	if (isValid) {

		p << F(PRINT_IR_RAW_BEGIN);
		p << F(PRINT_IR_RAW_HEADER_BEGIN) << length << F(PRINT_IR_RAW_HEADER_END);
		p << data[0];

		for (int i = 1; i < length ; i++) {
			p << F(PRINT_IR_RAW_SEPARATOR);
			if (i & 0x1) p << F(PRINT_IR_RAW_MARKSPACE);
			p << data[i];
		}

		p << F(PRINT_IR_RAW_END);
	}

	return isValid;
}

//========================================================================================================================
//
//========================================================================================================================
size_t IRPACKET :: _printSize () const {

	class PrintCounter : public Print {
	public:
		size_t _counter = 0;
		virtual size_t write (uint8_t) override {
			_counter++;
		}
	};

	PrintCounter printCounter;
	if (_printTo (printCounter)) {
		return printCounter._counter;
	}
	return 0;
}

//========================================================================================================================
//
//========================================================================================================================
size_t IRPACKET :: printTo (Print & p) const {

	if (!_printTo (p)) {
		Logln(F("WARNING !!! => No signal or InfraRed buffer is too small!!"));
	}

	return _printSize ();
}

//========================================================================================================================
//
//========================================================================================================================
bool IRPACKET :: parse (Stream & stream, Print & out) {

	if (!StreamParser::checkNextStrInStream (stream, PRINT_IR_RAW_BEGIN))
	{
		out << F("Malformatted begin! ABORTED!!") << LN;
		return false;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_IR_RAW_HEADER_BEGIN))
	{
		out << F("Malformatted header begin! ABORTED!!") << LN;
		return false;
	}

	length = (uint16_t) stream.parseInt();
	if ((length < 1) || (IR_MAX_LEN < length))
	{
		out << F("Buffer length error! ABORTED!!") << LN;
		goto ERROR;
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_IR_RAW_HEADER_END))
	{
		out << F("Malformatted header end! ABORTED!!") << LN;
		goto ERROR;
	}

	data[0] = (uint16_t) stream.parseInt();

	for (int i = 1; i < length ; i++) {

		if (!StreamParser::checkNextStrInStream (stream, PRINT_IR_RAW_SEPARATOR))
		{
			out << F("Malformatted separator! ABORTED!!") << LN;
			goto ERROR;
		}

		if (i & 0x1)
		{
			if (!StreamParser::checkNextStrInStream (stream, PRINT_IR_RAW_MARKSPACE))
			{
				out << F("Malformatted marks spaces! ABORTED!!") << LN;
				goto ERROR;
			}
		}

		data[i] = (uint16_t) stream.parseInt();
	}

	if (!StreamParser::checkNextStrInStream (stream, PRINT_IR_RAW_END))
	{
		out << F("Malformatted end! ABORTED!!") << LN;
		return false;
	}

	return true;

ERROR:
	length = 0;
	return false;
}