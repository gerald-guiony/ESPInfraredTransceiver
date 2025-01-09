# ESPInfraredTransceiver

[![ESP8266](https://img.shields.io/badge/ESP-8266-000000.svg?longCache=true&style=flat&colorA=CC101F)](https://www.espressif.com/en/products/socs/esp8266)

ESPInfraredTransceiver is a C++ library for the Arduino Core for ESP8266 board enabling the sending & receiving of any InfraRed signals.

This library is compatible with any infrared signal because it simply records and replays infrared signals, 
there is no protocol analysis like with [this other library](https://github.com/Arduino-IRremote/Arduino-IRremote/tree/master)

#### If you find this program useful, please give it a star.

## Getting started

To use ESPCoreExtension, you need:

* An ESP8266
* The Arduino IDE version 2.3.2 and more
* [Arduino core for the ESP8266](https://github.com/esp8266/Arduino) version 3.1.2
* Basic knowledge of the Arduino environment (upload a sketch, import libraries, ...)

## Installing ESPInfraredTransceiver

1. Download the latest master source code [.zip](https://github.com/gerald-guiony/ESPInfraredTransceiver/archive/master.zip)
2. In the Arduino IDE, load the `.zip` with **Sketch → Include Library → Add .ZIP Library**

## Dependencies

To compile this library you must have the latest version of [ESPCoreExtension library](https://github.com/gerald-guiony/ESPCoreExtension)

## Example

Universal IR remote control : [IrRemoteControl](https://github.com/gerald-guiony/ESPInfraredTransceiver/blob/master/examples/IrRemoteControl)
