//************************************************************************************************************************
// CustomWiFiServersManager.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>
#include <WiFiServersManager.h>


//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class CustomWiFiServersManager : public WiFiServersManager
{
	SINGLETON_CLASS(CustomWiFiServersManager)

protected:

	virtual void startCustomServers			() override;
	virtual void stopCustomServers			() override;

public:

	virtual void setup						(bool forceAccessPoint = false) override;
	virtual void loop						() override;
};
