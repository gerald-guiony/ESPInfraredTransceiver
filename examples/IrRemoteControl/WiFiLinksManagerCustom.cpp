//************************************************************************************************************************
// WiFiLinksManagerCustom.cpp
// Version 1.0 August, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#include <HttpServer.h>
#include <MqttDomoticzClient.h>

#include "Settings.h"

#include <HttpAdminCommandRequestHandler.h>

#include "HttpIrCommandRequestHandler.h"
#include "HttpIrRemoteControlRequestHandler.h"
#include "MqttIrDomoticzHandler.h"

#include "WiFiLinksManagerCustom.h"


#ifdef USING_DOMOTICZ_MQTT
#	warning -- USING_DOMOTICZ_MQTT defined --
#endif


namespace wifix {

SINGLETON_IMPL (WiFiLinksManagerCustom)

//========================================================================================================================
//
//========================================================================================================================
void WiFiLinksManagerCustom :: setupCustomLinks ()
{
	// Setup the Web Servers
	I(HttpServer).setup ({ 	&I(HttpAdminCommandRequestHandler),
							&I(HttpIrCommandRequestHandler),
							&I(HttpIrRemoteControlRequestHandler) });
	addCustomLinks ({&I(HttpServer)});

#ifdef USING_DOMOTICZ_MQTT
	// Setup the MQTT Clients
	I(MqttDomoticzClient).setup (MQTT_DOMOTICZ_ENDPOINT, MQTT_DOMOTICZ_PORT, {	&I(MqttDomoticzLogPublisher),
																	  			&I(MqttIrDomoticzPublisher),
															 					&I(MqttIrDomoticzSubscriber) });
	addCustomLinks ({&I(MqttDomoticzClient)});
#endif
}

}

