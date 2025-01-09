//************************************************************************************************************************
// HttpIrRemoteControlRequestHandler.h
// Version 1.0 August, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <ESPAsyncWebServer.h>

#include <Common.h>
#include <HttpRequestHandler.h>

#define DEFAULT_REFRESH_TIMEOUT				3000

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class HttpIrRemoteControlRequestHandler : public HttpRequestHandler
{
	SINGLETON_CLASS(HttpIrRemoteControlRequestHandler)

private:
	MemStream								_respMemStream;

private:
	void handleRoot							(AsyncWebServerRequest * request);
	void handleRefreshRemoteControl			(AsyncWebServerRequest * request, int refreshCallbackTimeout = DEFAULT_REFRESH_TIMEOUT, bool forceRefreshCallback = false);
	void handleOnRemoteControlButtonClick	(AsyncWebServerRequest * request);
	void handleOnRemoteControlSlideClick	(AsyncWebServerRequest * request);

public:
	virtual void setup 						(AsyncWebServer & asyncWebServer) override;
};
