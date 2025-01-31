//************************************************************************************************************************
// IrRremoteControl.ino
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include <Common.h>
#include <HttpServer.h>
#include <MqttDomoticzClient.h>
#include <IrReplayer.h>

#include "Settings.h"
#include "WiFiLinksManagerCustom.h"

using namespace corex;
using namespace wifix;
using namespace infrax;

//========================================================================================================================
//
//========================================================================================================================
void setup() {

	// ------------ Global Init

	EspBoard::init ();

	// ------------- setup

	I(IrReplayer).setup (IR_SEND_PIN, IR_RECV_PIN);

	// If WakeUpFromDeepSleep => No WifiManager & No Ap Mode
	I(WiFiLinksManagerCustom).setWifiManagerEnabled (!EspBoard::isWakeUpFromDeepSleep());
	I(WiFiLinksManagerCustom).setup();

	// ------------- Connect signals

	I(HttpServer).notifyRequestReceived	+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
#ifdef USING_DOMOTICZ_MQTT
	I(MqttDomoticzClient).notifyValidMessageReceived += std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
#endif

	I(ModuleSequencer).enterDeepSleepWhenWifiOff ();
	I(ModuleSequencer).setup (I(WiFiLinksManagerCustom).getModules ());
}

//========================================================================================================================
//
//========================================================================================================================
void loop() {
	I(ModuleSequencer).loop ();
}