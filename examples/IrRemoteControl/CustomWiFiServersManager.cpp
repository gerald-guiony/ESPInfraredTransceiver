//************************************************************************************************************************
// CustomWiFiServersManager.cpp
// Version 1.0 August, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#include <HttpServer.h>
#include <MqttClient.h>

#include "Settings.h"

#include <HttpAdminCommandRequestHandler.h>

#include "HttpIrCommandRequestHandler.h"
#include "HttpIrRemoteControlRequestHandler.h"
#include "MqttIrDomoticzHandler.h"

#include "CustomWiFiServersManager.h"


#ifdef USING_WIFI
#	warning -- USING_WIFI defined --
#	ifdef USING_MQTT
#		warning -- USING_MQTT defined --
#	endif
#endif



SINGLETON_IMPL (CustomWiFiServersManager)


//========================================================================================================================
//
//========================================================================================================================
void CustomWiFiServersManager :: startCustomServers () {

	I(HttpServer).addRequestHandlers( { &I(HttpAdminCommandRequestHandler), &I(HttpIrCommandRequestHandler), &I(HttpIrRemoteControlRequestHandler) } );

	// Start the Web Server
	I(HttpServer).setup ();


#ifdef USING_MQTT
	if (!WiFiHelper::isAccessPointMode()) {
		// Start the MQTT Client
		I(MqttClient).addHandlers ( { &I(MqttDomoticzLogPublisher), &I(MqttIrDomoticzPublisher), &I(MqttIrDomoticzSubscriber) } );
		I(MqttClient).setup (MQTT_SERVER_IP, MQTT_SERVER_PORT);
	}
#endif
}

//========================================================================================================================
//
//========================================================================================================================
void CustomWiFiServersManager :: stopCustomServers () {

	I(HttpServer).stop();

#ifdef USING_MQTT
	if (!WiFiHelper::isAccessPointMode()) {
		I(MqttClient).stop();
	}
#endif
}

//========================================================================================================================
//
//========================================================================================================================
void CustomWiFiServersManager :: setup (bool forceAccessPoint /*= false*/) {
	WiFiServersManager::setup (forceAccessPoint);
}

//========================================================================================================================
//
//========================================================================================================================
void CustomWiFiServersManager :: loop () {

	if (WiFiHelper::isWifiAvailable()) {

	    WiFiServersManager::loop ();

		I(HttpServer).loop();

#ifdef USING_MQTT
		if (!WiFiHelper::isAccessPointMode()) {
			I(MqttClient).loop();
		}
#endif
	}
}

