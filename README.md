# esp32-load-tester
Weight/Force measurement using a load cell and a ESP32/ESP8266-microcontroller, with a web-interface.

## Example Device
![Photo of an example device built with esp32-load-tester](doc/img/overview.jpg)

## System Overview
![System overview diagram](doc/system_overview.drawio.svg)

## Construction
### Wiring
![Wiring diagram for ESP32](doc/img/esp32_wiring.png)


### Software
#### Prerequisites
TODO

#### Build Instructions

* Repo is set up for VSCODE IDE, with PlatformIO extension
* Custom PlatformIO build targets were added, run them in this order
  1. Generate Secrects (generates WIFI password)
  1. Generate Wifi QR
  1. Build web-if
* Then run standard targets
  1. Build Filesystem Image
  1. Uplodat Filesystem Image
  1. Build
  1. Upload and Monitor


