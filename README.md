# esp32-load-tester
A simple and low-cost weight/force measurement device based on an ESP32/ESP8266 module and a [load cell](https://en.wikipedia.org/wiki/Load_cell). It features a web interface that allows controlling and reading the measurement. Additionally, a small built-in OLED display shows the current measurement and the device status.

It uses cheap components, only rudimentary calibration, and a simple design, resulting in low accuracy (approximately ±5%) and a low readout frequency (< 5 Hz).

Since any [load cell](https://en.wikipedia.org/wiki/Load_cell) can be used, the measurement range can be selected over a wide span, from a few grams to multiple tons.

<img alt="Photo of an example device built with esp32-load-tester" src="doc/img/overview.jpg" width="250" />

## Applications
The esp-load-tester is suited for _low-accuracy_ measurement of _slowly changing_ forces or weights. A few examples:

- Simple tensile strength testing
- Hanging scale
- General Newton meter

#### Example: Tensile Strength Tester

<img alt="Photo of a tensile strength tester built with the esp32-load-tester" src="doc/img/rope_strength_tester.jpg" width="600" />
<img alt="Photo of a tensile strength tester built with the esp32-load-tester" src="doc/img/screenshot_tensile_strength_test.jpg" width="200" />

## System Overview

The force is measured using a [load cell](https://en.wikipedia.org/wiki/Load_cell). A low-cost HX711 analog-to-digital converter (ADC) is used to measure the voltage across the Wheatstone bridge of the load cell. Its digital interface is read by a low-cost [ESP32](https://en.wikipedia.org/wiki/ESP32) or even cheaper [ESP8266](https://en.wikipedia.org/wiki/ESP8266) microcontroller module. The ESP additionally controls a small OLED display (optional, but recommended) and a digital temperature sensor (optional).

The ESP starts a Wi-Fi access point, and up to four _clients_ can connect. These clients can be mobile phones, tablets, or any other device with Wi-Fi and a browser. In the browser, the user opens `http://192.168.4.1/` to access the web interface, which allows controlling and monitoring the measurement.

The Wi-Fi uses WPA2-PSK security with a password configured at build time. For ease of use, a QR code containing the Wi-Fi SSID and password is generated. This can be printed and attached to the device. Users can then simply scan the QR code to connect. A second QR code linking to `http://192.168.4.1/` is also generated.

The web interface is served by a small web server running on the ESP. The server side is kept simple and consists of a REST API, a Server-Sent Events (SSE) service, and a static file server to load a [React.js](https://en.wikipedia.org/wiki/React_(software)) application on the client. The main part of the interface runs in the client’s browser, rendering a page with [Material Design](https://en.wikipedia.org/wiki/Material_Design), subscribing to SSE for measurement data, and plotting the data in a graph. A watchdog checks whether SSE events arrive periodically and otherwise shows an _offline_ state.

<img alt="System overview diagram" src="doc/system_overview.drawio.svg" width=700 />

## Construction

### Parts
Care has been taken to use only inexpensive, widely available parts. For obvious reasons, no specific suppliers are listed here, but all parts can easily be found via web search.

At the time of writing (2026), the total cost is approximately €60 from low-cost suppliers.

|Count|Item|Example|Cost|
|-:|-|-|-:|
|1|Load cell|500 kg load cell|~ 30 €|
|2|Eye bolts|M12 eye bolts|~ 6 €|
|1|ADC|HX711|~ 1 €|
|1|ESP32 development board|ESP32 Dev Kit C, CP2102|~ 7 €|
|1|OLED display (optional)|SH1106 1.3", 128×64 pixels|~ 3 €|
|1|Temperature sensor (optional)|DS18B20|~ 1 €|
|1|Battery holder|4×AA/R6 battery holder|~ 1 €|
|1|Power switch|Mini toggle switch (on/off)|~ 1 €|
|1|D-Sub plug|Female 9-pin, with case|~ 1 €|
|1|D-Sub plug|Male 9-pin|~ 1 €|
|1|3D-printed case (optional)|PLA print|~ 7 €|
|||||
|||**Total (with/without optional parts)**|~ 50–60 €|

### Wiring
The following table shows the wiring for a common 38-pin ESP32 module:

|Category|Component|Pin|<->|Component|Pin|
|-|-|-|-|-|-|
|**Signals**|ESP32|GPIO 21|<->|OLED|SDA|
||ESP32|GPIO 22|<->|OLED|SCL|
||ESP32|GPIO 23|<->|HX711|SCK|
||ESP32|GPIO 19|<->|HX711|DT|
||ESP32|GPIO 18|<->|DS18B20|2 (DQ)|
||
|**Power**|ESP32|Vin / 5V|<->|On/off switch|(COM)|
||On/off switch|(A)|<->|Battery|+|
||ESP32|GND|<->|Battery|-|
||ESP32|GND|<->|OLED|GND|
||ESP32|GND|<->|HX711|GND|
||ESP32|GND|<->|DS18B20|1 (GND)|
||ESP32|3V3|<->|OLED|VCC|
||ESP32|3V3|<->|HX711|VCC|
||ESP32|3V3|<->|DS18B20|3 (VDD)|

The following diagram shows the wiring for a 38-pin ESP32 development board:
<img alt="Wiring diagram for ESP32" src="doc/img/esp32_wiring.png" width=500 />

### Software
This project is set up for development with [PlatformIO](https://platformio.org/) using [Visual Studio Code](https://code.visualstudio.com) as the IDE. The web interface is built using the [Vite](https://vite.dev/) build tool, configured for a [React.js](https://en.wikipedia.org/wiki/React_(software)) application. This setup was tested on Ubuntu Linux.

TODO: Add instructions for other Linux distributions, Windows, macOS, etc.

#### Prerequisites
- [Visual Studio Code](https://code.visualstudio.com)
- [PlatformIO for VS Code](https://platformio.org/platformio-ide)
- Python 3.12+ (installed by default on Linux; for Windows, see [install Python](https://www.python.org/downloads/))

#### Build Instructions

- The repository is set up for VS Code with the PlatformIO extension  
  - Open the directory in VS Code; PlatformIO should detect `platformio.ini` automatically
- Custom PlatformIO build targets (run in order):
  1. Generate secrets (creates Wi-Fi password)
  2. Generate Wi-Fi QR code
  3. Build web interface
- Then run standard targets:
  1. Build filesystem image
  2. Upload filesystem image
  3. Build
  4. Upload and monitor

### Case

A 3D design of the case is included as a [FreeCAD](https://www.freecad.org/) model in `mech/case.FCStd`, with an exported STL in `mech/export/case.stl`.

<img alt="Screenshot of 3D case design in FreeCAD" src="doc/img/case_v1.png" />

Warning: This design has not yet been printed or verified.

TODO: Print and verify case.

#### Resources used for 3D design
- Drawings and 3D model of [Amphenol D-Sub DE09P064TXLF](https://www.amphenol-cs.com/product/de09p064txlf.html)
- [GOOBAY 10013 subminiature toggle switch datasheet](https://cdn-reichelt.de/documents/datenblatt/C200/GOOBAY_10013DB_DE.pdf)
- [Spax 3×12 screw](https://www.spax.com/de-de/p/universalschraube-vollgewinde-senkkopf-kreuzschlitz-z-4cut-yellox.html?variant=1081020300123)
- [1.3" Arduino OLED 128×64 display](https://www.makershop.de/display/oled/oled-1-3-zoll-weiss/), mechanical drawing contained in [OLED 1.3 Datasheet / Datenblatt](https://www.makershop.de/download/datasheet-oled-13.pdf)
- 3D model [ESP32 38 Pines ESP WROOM 32](https://grabcad.com/library/esp32-38-pines-esp-wroom-32-1) 
  WARNING: Dimensions are incorrect; scaling is required (XScale=0.976923, ZScale=1.016000)

### Build the Tensile Strength Tester

A simple tensile strength tester can be built using the esp-load-tester. An example build is shown below:

<img alt="Photo of a tensile strength tester built with the esp32-load-tester" src="doc/img/rope_strength_tester.jpg" width="600" />

#### Shopping list

|Count|Item|Example|Cost|
|-:|-|-|-:|
|1|_esp-load-tester_|_(see above)_|59 €|
|2|Side bars|Wood bar (8 x 6)cm, length 130 cm, softwood sufficient|20 €|
|2|End bars|Wood bar (8 x 6)cm, length 46 cm, softwood sufficient|7 €|
|1|Hand winch, steel cable|E.g. Einhell TC-WI 500|31 €|
|1|Screw for load cell|M12 x 100mm, [carriage bolt](https://en.wikipedia.org/wiki/Carriage_bolt)|1 €|
|2|Screws for winch|Wood screw ⌀ 6 x 80mm|1 €|
|2|Washers for winch|Washer ⌀ 6.4mm x ⌀ 18mm|1 €|
|8|Screws for bars|Wood screw ⌀ 5 x 100mm|1 €|
|||||
|||**Total**|121 €|
