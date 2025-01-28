//************************************************************************************************************************
// WiFiServersManagerCustom.h
// Version 1.0 June, 2017
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <Common.h>
#include <WiFiServersManager.h>


namespace wifix {

//------------------------------------------------------------------------------
// Singleton
class WiFiServersManagerCustom : public WiFiServersManager
{
	SINGLETON_CLASS(WiFiServersManagerCustom)

protected:

	virtual void setupCustomServers			() override;
	virtual void startCustomServers			() override;
	virtual void stopCustomServers			() override;
};

}
