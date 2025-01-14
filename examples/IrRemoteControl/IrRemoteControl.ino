//************************************************************************************************************************
// IrRremoteControl.ino
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include <Common.h>
#include <HttpServer.h>
#include <MqttClient.h>
#include <IrReplayer.h>

#include "Settings.h"
#include "CustomWiFiServersManager.h"


//========================================================================================================================
//
//========================================================================================================================
void setup() {

	// ------------ Global Init

	EspBoard::init ();

	// ------------- setup

	I(IrReplayer).setup (IR_SEND_PIN, IR_RECV_PIN);

	I(CustomWiFiServersManager).setWifiManagerEnabled (!EspBoard::isWakeUpFromDeepSleep());		// If WakeUpFromDeepSleep => No WifiManager & No Ap Mode
	I(CustomWiFiServersManager).setup();

	// ------------- Connect notifiers

	I(HttpServer).notifyRequestReceived	+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
#ifdef USING_MQTT
	I(MqttClient).notifyValidMessageParsed += std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
#endif

	I(ModuleSequencer).enterDeepSleepWhenWifiOff ();											// Deep sleep
	I(ModuleSequencer).setup ({ &I(CustomWiFiServersManager) });
}

//========================================================================================================================
//
//========================================================================================================================
void loop() {
	I(ModuleSequencer).loop ();
}












