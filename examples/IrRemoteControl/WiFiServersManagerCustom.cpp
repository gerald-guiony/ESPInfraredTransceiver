//************************************************************************************************************************
// WiFiServersManagerCustom.cpp
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

#include "WiFiServersManagerCustom.h"


#ifdef USING_MQTT
#	warning -- USING_MQTT defined --
#endif


namespace wifix {

SINGLETON_IMPL (WiFiServersManagerCustom)

//========================================================================================================================
//
//========================================================================================================================
void WiFiServersManagerCustom :: setupCustomServers ()
{
	// Setup the Web Servers
	I(HttpServer).setup ({ 	&I(HttpAdminCommandRequestHandler),
							&I(HttpIrCommandRequestHandler),
							&I(HttpIrRemoteControlRequestHandler) });
#ifdef USING_MQTT
	// Setup the MQTT Clients
	I(MqttClient).setup (MQTT_SERVER_IP, MQTT_SERVER_PORT, { &I(MqttDomoticzLogPublisher),
															 &I(MqttIrDomoticzPublisher),
															 &I(MqttIrDomoticzSubscriber) });
#endif
}

//========================================================================================================================
//
//========================================================================================================================
void WiFiServersManagerCustom :: startCustomServers ()
{
	// Start the Web Servers
	I(HttpServer).start ();
#ifdef USING_MQTT
	// Start the MQTT Clients
	I(MqttClient).start ();
#endif
}

//========================================================================================================================
//
//========================================================================================================================
void WiFiServersManagerCustom :: stopCustomServers ()
{
	I(HttpServer).stop();
#ifdef USING_MQTT
	I(MqttClient).stop();
#endif
}

}

