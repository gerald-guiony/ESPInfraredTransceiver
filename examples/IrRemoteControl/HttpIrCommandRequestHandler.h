//************************************************************************************************************************
// HttpIrCommandRequestHandler.h
// Version 1.0 August, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#pragma once

#include <ESPAsyncWebServer.h>

#include <Common.h>
#include <HttpRequestHandler.h>

//------------------------------------------------------------------------------
// WARNING : SINGLETON !!!!
class HttpIrCommandRequestHandler : public HttpRequestHandler
{
	SINGLETON_CLASS(HttpIrCommandRequestHandler)

private:

	void handleHelp									(AsyncWebServerRequest * request);
	void handleRecord								(AsyncWebServerRequest * request);
	void handleEmmit								(AsyncWebServerRequest * request);
	void handlePrint								(AsyncWebServerRequest * request);
	void handleParseBody							(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total);
	void handleStore								(AsyncWebServerRequest * request);
	void handleLoad									(AsyncWebServerRequest * request);
	void handleDelete								(AsyncWebServerRequest * request);
	void handlePrintIdList							(AsyncWebServerRequest * request);

public:

	virtual void setup 								(AsyncWebServer & asyncWebServer) override;
};












