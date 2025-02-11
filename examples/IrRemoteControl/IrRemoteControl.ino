//************************************************************************************************************************
// IrRremoteControl.ino
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#include <Common.h>
#include <IrReplayer.h>
#include <HttpServer.h>
#include <HttpAdminCommandRequestHandler.h>
#include <MqttDomoticzClient.h>
#include <WiFiConnectionManager.h>

#include "Settings.h"
#include "HttpIrCommandRequestHandler.h"
#include "HttpIrRemoteControlRequestHandler.h"
#include "MqttIrDomoticzHandler.h"

using namespace corex;
using namespace wifix;
using namespace infrax;

//========================================================================================================================
//
//========================================================================================================================
void setup() {

	// ------------ Global Init

	EspBoard::init ();

	// ------------- Connect signals

	I(HttpServer).notifyRequestReceived	+= std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));
	I(MqttDomoticzClient).notifyValidTopicReceived += std::bind (&ModuleSequencer::requestWakeUp, &I(ModuleSequencer));

	// ------------- Module sequencer

	I(ModuleSequencer).enterDeepSleepWhenWifiOff ();

	// ------------- Setup

	I(IrReplayer).setup (IR_SEND_PIN, IR_RECV_PIN);

	// If WakeUpFromDeepSleep => No WifiManager & No Ap Mode
	I(WiFiConnectionManager).setWifiManagerEnabled (!EspBoard::isWakeUpFromDeepSleep());
	I(WiFiConnectionManager).notifySetupWifiConnections += []() {

		I(HttpServer).setup ({ 	&I(HttpAdminCommandRequestHandler),
								&I(HttpIrCommandRequestHandler),
								&I(HttpIrRemoteControlRequestHandler) });

		I(MqttDomoticzClient).setup (MQTT_DOMOTICZ_ENDPOINT, MQTT_DOMOTICZ_PORT, {	&I(MqttDomoticzLogPublisher),
																					&I(MqttIrDomoticzPublisher),
																					&I(MqttIrDomoticzSubscriber) });
	};
	I(WiFiConnectionManager).setup ({	&I(HttpServer),
								 		&I(MqttDomoticzClient) });

	I(ModuleSequencer).setup (I(WiFiConnectionManager).getModules ());
}

//========================================================================================================================
//
//========================================================================================================================
void loop() {
	I(ModuleSequencer).loop ();
}