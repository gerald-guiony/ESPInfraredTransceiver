# IrRemoteControl

IrRemoteControl is an application that uses an ESP8266 Wi-Fi chip to record and replay any InfraRed signals using JSON or MQTT commands.

In this example, I used it to remotely control my Daikin air conditioner using Domoticz or the embedded web server in the ESP :yum:

![Web remote control](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/Iot_remote.JPG)

![InfraRed sniffer](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/ir-sniffer-1.png)

I used a [Wio Node](https://wiki.seeedstudio.com/Wio_Node/) with the [Grove InfraRed Emitter](https://wiki.seeedstudio.com/Grove-Infrared_Emitter/) 
and the [Grove InfraRed Receiver](https://wiki.seeedstudio.com/Grove-Infrared_Receiver/)

![Board](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/IR-board.jpg)
![Daikin](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/Daikin.jpg)

## Web server

In addition to the JSON and MQTT commands directly accessible, you can also load a webpage from the embedded ESP web server using its IP address.

This HTML page extensively uses AJAX for enhanced user interactions; see the source code page [here](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/html/WebServer.html)

![Send IR signal](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/IR-send.png)
![Record IR signal](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/img/IR-record.png) 

## Dependencies

To upload this application, you need to have the latest versions of the following:
* [ESPCoreExtension library](https://github.com/gerald-guiony/ESPCoreExtension)
* [ESPWiFiServersExtension library](https://github.com/gerald-guiony/ESPWiFiServersExtension)
* [ESPInfraredTransceiver library](https://github.com/gerald-guiony/ESPInfraredTransceiver)

## Board settings

Here are the board settings in the Arduino IDE that I use to upload the sketch to my Wio Node via OTA (Over-The-Air): [Wio Node board settings](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl/doc/WioNodeBoardSettings.png)
