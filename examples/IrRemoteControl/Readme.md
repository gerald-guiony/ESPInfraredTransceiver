# IrRemoteControl

IrRemoteControl is an application which uses an ESP8266 WiFi chip to record and replay any InfraRed signals with JSON or MQTT commands.

Here I used it to remotely control my Daikin air conditioning with Domoticz or thanks to the embedded web server in the ESP.

![Web remote control](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/Iot_remote.JPG)

![InfraRed sniffer](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/ir-sniffer-1.png)

I used a [Wio node](https://wiki.seeedstudio.com/Wio_Node/) with this [Grove InfraRed Emitter](https://wiki.seeedstudio.com/Grove-Infrared_Emitter/) 
and this [Grove InfraRed Receiver](https://wiki.seeedstudio.com/Grove-Infrared_Receiver/)

![Board](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/IR-board.jpg)
![Daikin](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/Daikin.jpg)

## Web server

In addition to the directly accessible JSON and MQTT commands, there is also the possibility of loading a web page from the web server embedded in the ESP with its IP address.

In this HTML page, AJAX is used extensively for more user interactions, see the source code page [here](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/html/WebServer.html)

![Send IR signal](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/IR-send.png)
![Record IR signal](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/IR-record.png) 

## Dependencies

To upload this application you must have the latest version of :
* [ESPCoreExtension library](https://github.com/gerald-guiony/ESPCoreExtension) 
* [ESPWiFiServersExtension library](https://github.com/gerald-guiony/ESPWiFiServersExtension)
* [ESPInfraredTransceiver library](https://github.com/gerald-guiony/ESPInfraredTransceiver)

## Board settings

Arduino IDE board settings for my Wio node [here](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/WioNodeBoardSettings.png)
