//************************************************************************************************************************
// IrPacketStorage.cpp
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include "IrPacketStorage.h"



SINGLETON_IMPL (IrPacketStorage)


//========================================================================================================================
//
//========================================================================================================================
bool IrPacketStorage :: read (uint8_t fileId, IRPACKET & irPacket)
{
//	spiffsInfos ();

	String filename = F(IR_HEADER_NAMEFILE);
	filename += fileId;
	filename += F(IR_EXT_NAMEFILE);

	File f = SPIFFS.open(filename, "r");
	if (!f) {
		Logln(F("Warning : Can't open the file : ") << filename);
		return false;
	}

	Logln(F("Open ") << filename);

	// returns the number of characters placed in the buffer (0 means no valid data found)
	// size_t readBytes( uint8_t *buffer, size_t length)
	if (f.readBytes ((char *) &irPacket.length, 2) == 2) {
		if (f.readBytes ((char *) &(irPacket.data[0]), irPacket.length * 2) == irPacket.length * 2) {
			f.close ();
			Logln(irPacket.length << F(" bytes Read"));
			return true;
		}
	}

	Logln(F("ERROR : file ") << filename << F(" is invalid"));

	irPacket.length = 0;
	f.close ();
	return false;
}

//========================================================================================================================
//
//========================================================================================================================
bool IrPacketStorage :: write (uint8_t fileId, const IRPACKET & irPacket)
{
	if (!FileStorage::spiffsCheckRemainingBytes ()) return false;
	if (irPacket.length < 1) {
		Logln(F("No InfraRed Signal!"));
		return false;
	}

	String filename = F(IR_HEADER_NAMEFILE);
	filename += fileId;
	filename += F(IR_EXT_NAMEFILE);

	File f = SPIFFS.open(filename, "w");
	if (!f) {
		Logln(F("ERROR : Can't open the file : ") << filename);
		return false;
	}

	// size_t write(const uint8_t *buffer, size_t size);
	if (f.write ((const uint8_t *) &irPacket.length, 2) == 2) {
		if (f.write ((const uint8_t *) &(irPacket.data[0]), irPacket.length * 2) == irPacket.length * 2) {
			f.close ();
			FileStorage::spiffsInfos ();
			return true;
		}
	}

	Logln(F("ERROR : Can't write in the file : ") << filename);
	FileStorage::spiffsInfos ();

	f.close ();
	return false;
}

//========================================================================================================================
//
//========================================================================================================================
bool IrPacketStorage :: remove (uint8_t fileId)
{
	String filename = F(IR_HEADER_NAMEFILE);
	filename += fileId;
	filename += F(IR_EXT_NAMEFILE);

	return SPIFFS.remove (filename);
}

//========================================================================================================================
//
//========================================================================================================================
String IrPacketStorage :: getList ()
{
	FileStorage::spiffsListFiles ();

	String result = "";
	Dir dir = SPIFFS.openDir("/");
	while (dir.next()) {
		String filename = dir.fileName();
		if (filename.indexOf (F(IR_HEADER_NAMEFILE)) == 0) {
			int lasti = filename.indexOf (F(IR_EXT_NAMEFILE));
			if (lasti > 0) {
				if (result.length()>0) {
					result += MSG_SEPARATOR_PARAM + filename.substring (strlen(IR_HEADER_NAMEFILE), lasti);
				}
				else {
					result = filename.substring (strlen(IR_HEADER_NAMEFILE), lasti);
				}
			}
		}
	}
	return result;
}

