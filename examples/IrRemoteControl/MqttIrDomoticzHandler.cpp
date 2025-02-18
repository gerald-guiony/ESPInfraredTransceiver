//************************************************************************************************************************
// MqttIrDomoticzHandler.cpp
// Version 1.0 December, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#include <Stream.h>
#include <StreamString.h>

#include <IrReplayer.h>

#include "MqttIrDomoticzHandler.h"

#include "Settings.h"

using namespace infrax;


// Todo :
// - Ajouter les paramètres du serveur mqtt (Ip, Port..) a la page du wifimanager
// https://github.com/tzapu/WiFiManager/blob/master/examples/AutoConnectWithFSParameters/AutoConnectWithFSParameters.ino


namespace wifix {


SINGLETON_IMPL (MqttIrDomoticzPublisher)


//========================================================================================================================
//
//========================================================================================================================
size_t MqttIrDomoticzPublisher :: publishStatutMessage (const String & msg) {

	static const String msgHeader = "[" + EspBoard::getDeviceName() + "] ";
	String custom_msg = msgHeader + msg;

	return publishMessage (MQTT_DOMOTICZ_LOG_IDX, custom_msg);
}


/************************************************************************************************************************/


SINGLETON_IMPL (MqttIrDomoticzSubscriber)

//========================================================================================================================
//
//========================================================================================================================
void MqttIrDomoticzSubscriber :: setup (AsyncMqttClient * asyncMqttClient) {

	_idx = MQTT_DOMOTICZ_DAIKIN_SWITCH_IDX;
	MqttDomoticzSubscriberIdx :: setup (asyncMqttClient);
}

//========================================================================================================================
//
//========================================================================================================================
bool MqttIrDomoticzSubscriber :: onTopicIdxReceived (const JsonObject& jsonArg) {

	// domoticz/out {
		// "Battery" : 255,
		// "RSSI" : 12,
		// "description" : "",
		// "dtype" : "Light/Switch",
		// "id" : "00014051",
		// "idx" : 1,
		// "name" : "Daikin (MQTT)",
		// "nvalue" : 1,
		// "stype" : "Switch",
		// "svalue1" : "0",
		// "switchType" : "On/Off",
		// "unit" : 1
	// }


	// décode le message - decode payload message
	// Here we know that the _idx member value matches the idx of the domoticz device

	if (jsonArg [PAYLOAD_NVALUE].success()) {

		// Détermine quel code IR doit être envoyé - Determines which IR signal should be sent
		int fileId = (jsonArg [PAYLOAD_NVALUE] == 1) ? 1 : 2;

		if (I(IrReplayer).loadSignal (fileId)) {

			StreamString sstrLogOut;
			I(IrReplayer).emmitSignal (sstrLogOut);

			// Response
			I(MqttIrDomoticzPublisher).publishStatutMessage (((fileId == 1) ? "Daikin On":"Daikin Off"));

			Log (sstrLogOut);

			return true;
		}
	}

	return false;
}

}