//************************************************************************************************************************
// MqttIrDomoticzHandler.h
// Version 1.0 February, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>
#include <MqttDomoticzHandler.h>


//------------------------------------------------------------------------------
// Singleton
class MqttIrDomoticzPublisher : public MqttDomoticzPublisher
{
	SINGLETON_CLASS(MqttIrDomoticzPublisher)

public:
	size_t publishStatutMessage		(const String & msg);
};


//------------------------------------------------------------------------------
// Singleton
class MqttIrDomoticzSubscriber : public MqttDomoticzSubscriber
{
	SINGLETON_CLASS(MqttIrDomoticzSubscriber)

public:
	virtual bool parseJsonObj			(const JsonObject& jsonObj) override;
};










