//************************************************************************************************************************
// HttpIrRemoteControlRequestHandler.cpp
// Version 1.0 August, 2018
// Author Gerald Guiony
//************************************************************************************************************************

#include <Ticker.h>

#include <IrReplayer.h>
#include <HttpServer.h>

#include "HttpIrRemoteControlRequestHandler.h"

using namespace infrax;


#define CALL_IR_RECORD_DELAY 100

Ticker irRecordTimer;


// Use PROGMEM and server.send_P to send data from PROGMEM — in this case it doesn't need to be copied to RAM twice, you avoid allocating Strings and all the associated issues.
static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html><meta content="Gérald Guiony"name=Author><meta content="© Gérald Guiony - tous droits réservés"name=Copyright><meta content="text/html; charset=UTF-8"http-equiv=Content-Type><meta name=viewport id=viewport><title>IOT Remote Control</title><script>function SetViewport(e){var t=(window.screen.width<window.screen.height?window.screen.width:window.screen.height)/e;document.getElementById("viewport").setAttribute("content","initial-scale="+t+", maximum-scale="+t+", minimum-scale="+t+", user-scalable=no, width="+e)}function Substring(e,t,n){var o=e.indexOf(t),r=e.substring(o).indexOf(n)+o;return o>=0&&o<r?e.substring(o+t.length,r):""}function GetParamValueInString(e,t){return Substring(e,t+"=",";")}function PurgeCommentsInString(e){for(var t=e;;){var n=Substring(t,"\x3c!--","--\x3e");if(n.length<=0)break;t=t.replace("\x3c!--"+n+"--\x3e","")}return t}function HighlightBackground(e){document.getElementById("slideRecord").checked?document.bgColor=e?"Red":"pink":document.bgColor=e?"Lightblue":"Lightgrey"}function DisableRemoteControl(e){for(var t=document.getElementsByTagName("input"),n=0;n<t.length;n++)t[n].disabled=e;for(var o=document.getElementsByTagName("button"),n=0;n<o.length;n++)o[n].disabled=e}function RefreshRemoteControlMessage(e){document.getElementById("info_txt").innerHTML=e}function BuildQueryString(e){return e+"?nocache="+1e6*Math.random()}function SendAjaxRequest(e,t,n){var o=!1;if(!(o=new XMLHttpRequest))return alert("Your browser broke!"),!1;o.timeout=25e3,o.ontimeout=function(){n(o)},o.onload=function(){t(o)},o.open("GET",e,!0),o.send(null)}function TimeoutRemoteControlCallback(e){HighlightBackground(!1),DisableRemoteControl(!1),RefreshRemoteControlMessage("No response from the IOT remote control! Timed out..")}function RefreshRemoteControlCallback(e){if(4==e.readyState&&200==e.status&&null!=e.responseText){GetParamValueInString(e.responseText,"$UNCHECK_RECORD").length>0&&(document.getElementById("slideRecord").disabled=!1,document.getElementById("slideRecord").checked=!1);var t=GetParamValueInString(e.responseText,"$REFRESH_TIMEOUT");t.length>0?setTimeout("RefreshRemoteControl()",t):(HighlightBackground(!1),DisableRemoteControl(!1)),RefreshRemoteControlMessage(PurgeCommentsInString(e.responseText))}}function RefreshRemoteControl(){SendAjaxRequest(BuildQueryString("ajax_RefreshRemoteControl"),RefreshRemoteControlCallback,TimeoutRemoteControlCallback)}function OnRemoteControlButtonClick(e){HighlightBackground(!0),DisableRemoteControl(!0),RefreshRemoteControlMessage("Please wait..."),setTimeout(function(){var t=BuildQueryString("ajax_OnRemoteControlButtonClick");t+="&numbutton="+e,SendAjaxRequest(t+="&isRecord="+document.getElementById("slideRecord").checked,RefreshRemoteControlCallback,TimeoutRemoteControlCallback)},1e3)}function OnRemoteControlSlideClick(){HighlightBackground(!1),document.getElementById("slideRecord").checked?(DisableRemoteControl(!0),SendAjaxRequest(BuildQueryString("ajax_OnRemoteControlSlideClick"),RefreshRemoteControlCallback,TimeoutRemoteControlCallback)):RefreshRemoteControlMessage("")}function Timer(){var e=new Date;document.getElementById("monHorloge").innerHTML=e.getHours()+":"+e.getMinutes()+":"+e.getSeconds(),setTimeout("Timer()",1e3)}</script><style>body{color:#fff;font-family:Arial,Georgia,sans-serif;font-weight:400}table.remoteControl{background-color:#fff;background-image:-webkit-linear-gradient(top,#fff,#f4f1ee);background-image:linear-gradient(top,#fff,#f4f1ee);border-spacing:6px 6px;border-radius:40px;box-shadow:0 4px 6px 0 rgba(0,0,0,.3),inset 0 2px 1px 1px #fff,inset 0 -2px 1px 1px rgba(204,198,197,.5);color:#08c;margin:4px 4px 4px 4px;text-shadow:1px 1px 0 rgba(0,136,204,.15)}table{background-color:#fff;background-image:-webkit-linear-gradient(top,#fff,#f4f1ee);background-image:linear-gradient(top,#fff,#f4f1ee);border-spacing:2px 2px;border-radius:40px;box-shadow:inset 0 2px 1px 1px rgba(204,198,197,.5),inset 0 -2px 1px 1px #fff;color:#08c;margin:4px 4px 4px 4px;text-shadow:1px 1px 0 rgba(0,136,204,.15)}tr{border:0 solid #000}th{border:0 solid #000;width:100px;height:100px}textarea{background-color:#ffffe0;border:4px inset #add8e6;border-radius:40px;box-shadow:inset 0 4px 1px 1px rgba(204,198,197,.5),inset 0 -2px 1px 1px #fff;font-family:Consolas,Courier,sans-serif;font-weight:400;font-size:1.6em;overflow:auto;margin:10px 4px 10px 4px;padding:35px 15px 35px 15px;resize:none;text-align:center;vertical-align:middle;width:350px}button{background-image:-webkit-linear-gradient(top,#f4f1ee,#fff);background-image:linear-gradient(top,#f4f1ee,#fff);border:0 solid #000;border-radius:50%;box-shadow:0 4px 6px 0 rgba(0,0,0,.3),inset 0 2px 1px 1px #fff,inset 0 -2px 1px 1px rgba(204,198,197,.5);color:#08c;float:center;font-size:1.2em;height:60px;width:60px;margin:0 2px 2px 2px;position:relative;text-shadow:1px 1px 0 rgba(0,136,204,.15);-webkit-transition:all .1s linear;transition:all .1s linear}button:hover{background-image:-webkit-linear-gradient(top,#fff,#f4f1ee);background-image:linear-gradient(top,#fff,#f4f1ee);color:#0cc}button:active{background-image:-webkit-linear-gradient(top,#efedec,#f7f4f4);background-image:linear-gradient(top,#efedec,#f7f4f4);box-shadow:0 2px 3px 0 rgba(0,0,0,.4),inset 0 -2px 1px 1px rgba(204,198,197,.5)}div.checkBoxSlide{background-image:-webkit-linear-gradient(top,#f4f1ee,#fff);background-image:linear-gradient(top,#f4f1ee,#fff);border:0 solid #000;border-radius:50px;box-shadow:inset 0 2px 1px 1px rgba(204,198,197,.5),inset 0 -2px 1px 1px #fff;color:#08c;font-size:1.2em;font-weight:400;height:30px;width:110px;margin:0 2px 2px 2px;position:relative;text-shadow:1px 1px 0 rgba(0,136,204,.15)}div.checkBoxSlide:after{content:'OFF';color:$activeColor;position:absolute;top:4px;right:10px;z-index:0}div.checkBoxSlide:before{content:'ON';color:red;position:absolute;top:4px;left:10px;z-index:0}div.checkBoxSlide label{background-image:-webkit-linear-gradient(top,#f4f1ee,#fff);background-image:linear-gradient(top,#f4f1ee,#fff);border-radius:50px;box-shadow:0 4px 6px 0 rgba(0,0,0,.3),inset 0 2px 1px 1px #fff,inset 0 -2px 1px 1px rgba(204,198,197,.5);display:block;width:49px;height:22px;cursor:pointer;position:absolute;top:3px;left:3px;z-index:1;-webkit-transition:all .4s ease;transition:all .4s ease}div.checkBoxSlide input[type=checkbox]{visibility:hidden}div.checkBoxSlide input[type=checkbox]:checked+label{left:58px}p.checkBoxSlideLabel{color:#08c;font-size:1.2em;font-weight:400;margin:4px 4px 4px 4px}</style><body bgcolor=Lightgrey onload=Timer(),SetViewport(500)><center><h2>IOT Remote Control</h2><table class=remoteControl><tr><th colspan=4><textarea id=info_txt readonly rows=3></textarea><tr><th colspan=2><table><tr><th><button onclick=OnRemoteControlButtonClick(1)>ON</button><th><button onclick=OnRemoteControlButtonClick(2)>OFF</button></table><th colspan=2><table><tr><th><button onclick=OnRemoteControlButtonClick(3)>☀</button><th><button onclick=OnRemoteControlButtonClick(4)>❄</button></table><tr><th rowsspan=2><table><tr><th><button onclick=OnRemoteControlButtonClick(5)>🔺</button><tr></tr><th><button onclick=OnRemoteControlButtonClick(6)>🔻</button></table><th colspan=2 rowsspan=2><table><tr><th><button onclick=OnRemoteControlButtonClick(7)>☢</button><th><button onclick=OnRemoteControlButtonClick(8)>🔃</button><tr></tr><th><button onclick=OnRemoteControlButtonClick(9)>Mode</button><th><button onclick=OnRemoteControlButtonClick(10)>🌀</button></table><th rowsspan=2><table><tr><th><button onclick=OnRemoteControlButtonClick(11)>+</button><tr></tr><th><button onclick=OnRemoteControlButtonClick(12)>-</button></table></th><tr><th colspan=2 align=right><p class=checkBoxSlideLabel>Record IR signal<th colspan=2><div class=checkBoxSlide><input id=slideRecord name=check onclick=OnRemoteControlSlideClick() type=checkbox><label for=slideRecord></label></div></tr></table><br><br><div id=monHorloge>...</div></center>)rawliteral";


namespace wifix {


SINGLETON_IMPL (HttpIrRemoteControlRequestHandler)


//========================================================================================================================
//
//========================================================================================================================
void HttpIrRemoteControlRequestHandler :: handleRoot (AsyncWebServerRequest * request)
{
	Logln (F("handleRoot"));

	if (I(HttpServer).isCaptivePortal(request)) { // If captive portal redirect instead of displaying the page.
		return;
	}

	request->send_P(200, F("text/html"), INDEX_HTML);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrRemoteControlRequestHandler :: handleRefreshRemoteControl (AsyncWebServerRequest * request, int refreshCallbackTimeout, bool forceRefreshCallback)
{
	Logln (F("handleRefreshRemoteControl"));

	String msg = _respMemStream.readStringUntil(((String) LN)[0]);

	if ((_respMemStream.available() > 0) || forceRefreshCallback) {
		msg += F("<!-- $REFRESH_TIMEOUT=");
		msg += refreshCallbackTimeout;				// Paramètre de rappel pour la fonction javascript RefreshRemoteControl()
		msg += F("; -->");
	}

	// Indique de renvoyer systématiquement la requête au serveur et servir une éventuelle version en cache que dans le cas où le serveur le demande.
	// La requête ou les données ne devraient jamais être mises en cache.
	// Le cache doit refaire une requête dans le cas où les données sont expirées afin de les mettre à jour s'il y a lieu (il reste parfaitement possible que le serveur réponde avec les mêmes données).
	AsyncWebServerResponse *response = request->beginResponse(200, F("text/html"), msg);
	response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
	response->addHeader("Pragma", "no-cache");
	response->addHeader("Expires", "-1");
	request->send(response);

	Logln(F("[") << msg << F("]") << LN);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrRemoteControlRequestHandler :: handleOnRemoteControlButtonClick (AsyncWebServerRequest * request)
{
	Logln (F("handleOnRemoteControlButtonClick"));

	_respMemStream.flush();

	int numButton = request->arg(F("numbutton")).toInt();
	bool isRecord = (request->arg(F("isRecord")) == F("true"));

	if (isRecord) {
		_respMemStream << F("Press the button on your remote control now - once only") << LN;
		handleRefreshRemoteControl(request, IR_CAPTURE_DELAY_MS + CALL_IR_RECORD_DELAY, true);

		// Pour que le message précédent soit envoyé a la page web il faut rendre la main au systeme..
		// After 50ms recordSignal is called
		irRecordTimer.once_ms (
			50,
			[this, numButton] () {
				if (I(IrReplayer).recordSignal (_respMemStream)) {
					if (I(IrReplayer).saveSignal (numButton)) {
						_respMemStream << F("Now you can test the button you just recorded") << LN;
						_respMemStream << F("<!-- $UNCHECK_RECORD=true; -->") << LN;	// Uncheck the slide record
					}
				}
			}
		);
	}
	else {
		if (I(IrReplayer).loadSignal (numButton)) {
			I(IrReplayer).emmitSignal (_respMemStream);
		}
		else {
			_respMemStream << F("No InfraRed signal found, record one first") << LN;
		}
		handleRefreshRemoteControl(request);
	}
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrRemoteControlRequestHandler :: handleOnRemoteControlSlideClick (AsyncWebServerRequest * request)
{
	Logln (F("handleOnRemoteControlSlideClick"));

	_respMemStream.flush();

	_respMemStream << F("Position your real remote control facing the IOT RC") << LN;
//	_respMemStream << F("Distance between the two should be at most one meter apart") << LN;
	_respMemStream << F("Click on a button below you wish to select") << LN;

	handleRefreshRemoteControl(request);
}

//========================================================================================================================
//
//========================================================================================================================
void HttpIrRemoteControlRequestHandler :: setup (AsyncWebServer & asyncWebServer)
{
	asyncWebServer.on("/", 									std::bind(&HttpIrRemoteControlRequestHandler::handleRoot,						this, _1));
	asyncWebServer.on("/ajax_RefreshRemoteControl", 		std::bind(&HttpIrRemoteControlRequestHandler::handleRefreshRemoteControl,		this, _1, DEFAULT_REFRESH_TIMEOUT, false));
	asyncWebServer.on("/ajax_OnRemoteControlButtonClick",	std::bind(&HttpIrRemoteControlRequestHandler::handleOnRemoteControlButtonClick,	this, _1));
	asyncWebServer.on("/ajax_OnRemoteControlSlideClick",	std::bind(&HttpIrRemoteControlRequestHandler::handleOnRemoteControlSlideClick,	this, _1));
}

}

