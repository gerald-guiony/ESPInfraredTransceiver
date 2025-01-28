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
#include "WiFiServersManagerCustom.h"

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
	I(WiFiServersManagerCustom).setWifiManagerEnabled (!EspBoard::isWakeUpFromDeepSleep());	
	I(WiFiServersManagerCustom).setup();

	// ------------- Connect signals

	I(HttpServer).notifyRequestReceived	+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
#ifdef USING_MQTT
	I(MqttClient).notifyValidMessageParsed += std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
#endif

	I(ModuleSequencer).enterDeepSleepWhenWifiOff ();
	I(ModuleSequencer).setup ({ &I(WiFiServersManagerCustom) });
}

//========================================================================================================================
//
//========================================================================================================================
void loop() {
	I(ModuleSequencer).loop ();
}












