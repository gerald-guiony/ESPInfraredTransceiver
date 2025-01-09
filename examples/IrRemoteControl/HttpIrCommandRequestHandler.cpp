//************************************************************************************************************************
// HttpIrCommandRequestHandler.cpp
// Version 1.0 August, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#include <Stream.h>
#include <StreamString.h>
#include <ArduinoJson.h>

#include <IrReplayer.h>

#include <HttpServer.h>

#include "MqttIrDomoticzHandler.h"
#include "HttpIrCommandRequestHandler.h"


// Important things to remember
// ----------------------------
// * This is fully asynchronous server and as such does not run on the loop thread.
// * You can not use yield or delay or any function that uses them inside the callbacks
// * The server is smart enough to know when to close the connection and free resources
// * You can not send more than one response to a single request



// HTTP Request Methods
// https://www.w3schools.com/tags/ref_httpmethods.asp



// Use PROGMEM and server.send_P to send data from PROGMEM — in this case it doesn't need to be copied to RAM twice, you avoid allocating Strings and all the associated issues.
static const char PROGMEM PRINT_HELP[] = R"rawliteral(
===========================================================================================================
 *** AVAILABLE IR JSON COMMANDS ON THIS DEVICE ***
===========================================================================================================

-----------------------------------------------------------------------------------------------------------
 Json command format :            /ir/$cmd?json={"$name":"$value", ..}
-----------------------------------------------------------------------------------------------------------
 InfraRed signal format :         [Raw (len) mark -space mark -space ..]
-----------------------------------------------------------------------------------------------------------

 Supported commands list ($cmd?json={"$name":"$value", ..}) :

 help ............................ Print this help
 record .......................... Record in memory the InfraRed signal emitted by your remote control
 emmit : {"id": $1} .............. Emmits the IR signal stored in the id file or the one currently in memory (if no id argument)
 print ........................... Print the InfraRed signal currently in memory in the response body (POST)
 parse [Ir signal in POST] ....... Parse IR signal in the request body (POST: No restriction on data length)
 store : {"id": $1} .............. Store the IR signal currently in memory in a file with an Id between 1 to 255
 load : {"id": $1} ............... Load in memory the stored IR signal from the corresponding id file
 delete : {"id": $1} ............. Delete the corresponding id file (containing a stored IR signal)
 printids ........................ List of all id files containing stored InfraRed signals

===========================================================================================================
)rawliteral";



//inline Print & operator <<(Print & printer, JsonObject & jsonObj) { jsonObj.printTo(printer); return printer; }


SINGLETON_IMPL (HttpIrCommandRequestHandler)


//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handleHelp (AsyncWebServerRequest * request)
{
	Logln (F("=> help"));
	request->send_P(200, F("text/plain"), PRINT_HELP);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handleRecord (AsyncWebServerRequest * request)
{
	Logln (F("=> record ir signal"));

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));

	// Memory pool for JSON object tree.
	// Inside the brackets, 200 is the size of the pool in bytes.
	// Don't forget to change this value to match your JSON document.
	// Use arduinojson.org/assistant to compute the capacity.
	//StaticJsonBuffer<200> jsonBuffer;

	// StaticJsonBuffer allocates memory on the stack, it can be
	// replaced by DynamicJsonBuffer which allocates in the heap.
	DynamicJsonBuffer jsonBuffer;

	// Create the root of the object tree.
	// It's a reference to the JsonObject, the actual bytes are inside the
	// JsonBuffer with all the other nodes of the object tree.
	// Memory is freed when jsonBuffer goes out of scope.
	JsonObject& jsonRsp = jsonBuffer.createObject();

	StreamString sstr;
	jsonRsp["command"] = "record";
	jsonRsp["status"] = I(IrReplayer).recordSignal (sstr);
	jsonRsp["message"] = sstr.c_str();

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handleEmmit (AsyncWebServerRequest * request)
{
	DynamicJsonBuffer jsonBuffer;

	Logln (F("=> emmit ir signal"));

	// If json arguments are present
	if (request->hasArg("json")) {

		JsonObject& jsonArg = jsonBuffer.parse(request->arg("json"));

		// Test if parsing succeeds.
		if (!jsonArg.success()) {
			request->send(400, F("text/plain"), F("400: Invalid json argument"));
			return;
		}

		if (!jsonArg ["id"].success()) {
			request->send(400, F("text/plain"), F("400: Invalid json field"));
			return;
		}

		int fileId = jsonArg ["id"];
		if ((fileId <= 0) || (255 < fileId)) {
			request->send(400, F("text/plain"), F("400: Invalid id argument"));
			return;
		}

		// deserialize ir signal in memory
		if (I(IrReplayer).loadSignal (fileId)) {
			I(MqttIrDomoticzPublisher).publishStatutMessage (((fileId == 1) ? "Daikin On":"Daikin Off"));
		}
	}

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	JsonObject& jsonRsp = jsonBuffer.createObject();

	StreamString sstr;
	jsonRsp["command"] = "emmit";
	jsonRsp["status"] = I(IrReplayer).emmitSignal (sstr);
	jsonRsp["message"] = sstr.c_str();

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handlePrint (AsyncWebServerRequest * request)
{
	Logln (F("=> print ir signal"));
/*
	MemStream memStream;
	memStream << IrSignal;
	AsyncWebServerResponse * response = request->beginResponse (memStream, F("text/plain"), memStream.available ());
	request->send (response);
*/

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("text/plain"));
	*response << I(IrReplayer).currentSignal();
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handleParseBody (AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
{
	Logln (F("=> parse ir signal"));

	// A revoir dans le cas de plusieurs connections en même temps..
	static MemStream * memStream = NULL;

	if (index == 0) {																		// Body start
		memStream = new MemStream();
	}

	if (memStream != NULL) {

		memStream->write (data, len);

		if (index + len == total) {															// Body end

			// This way of sending Json is great for when the result is below 4KB
			AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
			DynamicJsonBuffer jsonBuffer;
			JsonObject& jsonRsp = jsonBuffer.createObject();

			StreamString sstr;
			jsonRsp["command"] = "parse";
			jsonRsp["status"] = I(IrReplayer).currentSignal().parse (*memStream, sstr);
			jsonRsp["message"] = sstr.c_str();

			jsonRsp.prettyPrintTo(*response);
			request->send(response);

			delete memStream;
			memStream = NULL;
		}
	}
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handleStore (AsyncWebServerRequest * request)
{
	Logln (F("=> store ir signal"));

	if (!request->hasArg("json")) {
		request->send(400, F("text/plain"), F("400: json argument not found"));				// The request is invalid, so send HTTP status 400
		return;
	}

	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonArg = jsonBuffer.parse(request->arg("json"));

	// Test if parsing succeeds.
	if (!jsonArg.success()) {
		request->send(400, F("text/plain"), F("400: Invalid json argument"));
		return;
	}

	if (!jsonArg ["id"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	int fileId = jsonArg ["id"];
	if ((fileId <= 0) || (255 < fileId)) {
		request->send(400, F("text/plain"), F("400: Invalid id argument"));
		return;
	}

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject & jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "store";
	jsonRsp["status"] = I(IrReplayer).saveSignal (fileId);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handleLoad (AsyncWebServerRequest * request)
{
	Logln (F("=> load ir signal"));

	if (!request->hasArg("json")) {
		request->send(400, F("text/plain"), F("400: json argument not found"));				// The request is invalid, so send HTTP status 400
		return;
	}

	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonArg = jsonBuffer.parse(request->arg("json"));

	// Test if parsing succeeds.
	if (!jsonArg.success()) {
		request->send(400, F("text/plain"), F("400: Invalid json argument"));
		return;
	}

	if (!jsonArg ["id"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	int fileId = jsonArg ["id"];
	if ((fileId <= 0) || (255 < fileId)) {
		request->send(400, F("text/plain"), F("400: Invalid id argument"));
		return;
	}

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "load";
	// deserialize ir signal
	jsonRsp["status"] = I(IrReplayer).loadSignal (fileId);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handleDelete (AsyncWebServerRequest * request)
{
	Logln (F("=> delete ir signal"));

	if (!request->hasArg("json")) {
		request->send(400, F("text/plain"), F("400: json argument not found"));				// The request is invalid, so send HTTP status 400
		return;
	}

	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonArg = jsonBuffer.parse(request->arg("json"));

	// Test if parsing succeeds.
	if (!jsonArg.success()) {
		request->send(400, F("text/plain"), F("400: Invalid json argument"));
		return;
	}

	if (!jsonArg ["id"].success()) {
		request->send(400, F("text/plain"), F("400: Invalid json field"));
		return;
	}

	int fileId = jsonArg ["id"];
	if ((fileId <= 0) || (255 < fileId)) {
		request->send(400, F("text/plain"), F("400: Invalid id argument"));
		return;
	}

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream * response = request->beginResponseStream(F("application/json"));
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "delete";
	jsonRsp["status"] = I(IrPacketStorage).remove (fileId);

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: handlePrintIdList (AsyncWebServerRequest * request)
{
	Logln (F("=> printids"));

	// This way of sending Json is great for when the result is below 4KB
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject& jsonRsp = jsonBuffer.createObject();

	jsonRsp["command"] = "printids";
	jsonRsp["id list"] = I(IrPacketStorage).getList();

	jsonRsp.prettyPrintTo(*response);
	request->send(response);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrCommandRequestHandler :: setup (AsyncWebServer & asyncWebServer)
{
	asyncWebServer.on("/ir/help",		std::bind(&HttpIrCommandRequestHandler::handleHelp,			this, _1));
	asyncWebServer.on("/ir/record",		std::bind(&HttpIrCommandRequestHandler::handleRecord,		this, _1));
	asyncWebServer.on("/ir/emmit",		std::bind(&HttpIrCommandRequestHandler::handleEmmit,		this, _1));
	asyncWebServer.on("/ir/print",		std::bind(&HttpIrCommandRequestHandler::handlePrint,		this, _1));
	asyncWebServer.on("/ir/parse",		HTTP_ANY, [](AsyncWebServerRequest *request) {/* nothing and dont remove it */}, NULL, std::bind(&HttpIrCommandRequestHandler::handleParseBody, this, _1, _2, _3, _4, _5));
	asyncWebServer.on("/ir/store",		std::bind(&HttpIrCommandRequestHandler::handleStore,		this, _1));
	asyncWebServer.on("/ir/load",		std::bind(&HttpIrCommandRequestHandler::handleLoad,			this, _1));
	asyncWebServer.on("/ir/delete",		std::bind(&HttpIrCommandRequestHandler::handleDelete,		this, _1));
	asyncWebServer.on("/ir/printids",	std::bind(&HttpIrCommandRequestHandler::handlePrintIdList,	this, _1));
}












