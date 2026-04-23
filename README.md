# esp32-load-tester
Weight/Force measurement using a load cell and a ESP32/ESP8266-microcontroller, with a web-interface.

## Example Device
<img alt="Photo of an example device built with esp32-load-tester" src="doc/img/overview.jpg" width="300" />

## System Overview
<img alt="System overview diagram" src="doc/system_overview.drawio.svg" width=300 />

## Construction
### Wiring
<img alt="Wiring diagram for ESP32" src="doc/img/esp32_wiring.png" width=300 />


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


