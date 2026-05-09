# esp32-load-tester

![PlatformIO](https://img.shields.io/badge/PlatformIO-ready-orange)
![ESP32](https://img.shields.io/badge/ESP32-supported-blue)
![ESP8266](https://img.shields.io/badge/ESP8266-supported-blue)
![License: GPL-3.0 + CERN-OHL-S](https://img.shields.io/badge/license-GPLv3%20%2B%20CERN--OHL--S-blue)


A simple, low-cost weight/force measurement device based on an ESP32/ESP8266 and a [load cell](https://en.wikipedia.org/wiki/Load_cell).

It provides:
- A web interface for control and live measurement
- Optional OLED display for local readout
- Simple hardware using inexpensive, widely available components

> ⚠️ Designed for **low-cost, low-accuracy applications** (~±5%, <5 Hz sampling rate)

---

## 📸 Overview

<img alt="Device overview" src="doc/img/overview.jpg" width="250" />

---

## ✨ Features

- Works with a wide range of load cells (grams → tons)
- Wi-Fi access point (no external network required)
- Web-based UI (phone, tablet, desktop)
- Live graph visualization
- QR code for easy connection
- Optional:
  - OLED display
  - Temperature sensor

---

## 🧰 Applications

Suitable for **slow-changing, low-accuracy measurements**, such as:

- Simple tensile strength testing
- Hanging scales
- Simple force measurement

### Example: Tensile Strength Tester

<img src="doc/img/rope_strength_tester.jpg" height="370" />
<img src="doc/img/screenshot_tensile_strength_test.jpg" height="370" />

---

## 🧠 System Overview

- Measurement via load cell (Wheatstone bridge) + HX711 ADC 
- Controlled by ESP32 / ESP8266
- Embedded web server provides:
  - REST API
  - Server-Sent Events (SSE)
  - Static file server (compiled React application)
- Client side:
  - React application
  - Rendering with Material Design

**Connectivity:**
- Device creates its own Wi-Fi network
- Wi-Fi protected by WPA2-PSK, password set at build-time
- Wi-Fi QR code is generated for ease of connecting
- Up to 4 clients supported
- Access via: http://192.168.4.1/

**UI behavior:**
- Real-time graph plotting
- SSE-based updates
- Offline detection via watchdog

<img src="doc/system_overview.drawio.svg" width="700" />

---

## 🔩 Hardware

### Parts List

|Count|Item|Example|Cost|
|-:|-|-|-:|
|1|Load cell|500 kg|~30 €|
|2|Eye bolts|M12|~6 €|
|1|HX711 ADC||~1 €|
|1|ESP32 dev board|DevKit C (CP2102)|~7 €|
|1|OLED (optional)|1.3" SH1106 128×64|~3 €|
|1|Temp sensor (optional)|DS18B20|~1 €|
|1|Battery holder|4×AA|~1 €|
|1|Power switch||~1 €|
|2|D-Sub connectors|9-pin|~3 €|
|1|3D-printed case (optional)||~7 €|
|||**Total**|~50–60 €|


---

### 🔌 Wiring

Designed for a standard 38-pin ESP32:

- HX711
  - SCK → GPIO 23
  - DT → GPIO 19
- OLED (I²C)
  - SDA → GPIO 21
  - SCL → GPIO 22
- DS18B20
  - DQ → GPIO 18 (pull-up active in ESP)

<img src="doc/img/esp32_wiring.png" width="500" />

---

## 💻 Software Setup

### Requirements

- Visual Studio Code
- PlatformIO extension
- Python 3.12+
- npm

### Build Steps

1. Open project in VS Code (PlatformIO auto-detects config)
1. Run custom targets:
   - Generate secrets (Wi-Fi password)
   - Generate Wi-Fi QR code
   - Build web interface
1. Run standard targets:
   - Build filesystem image
   - Upload filesystem
   - Build firmware
   - Upload & monitor

---

## 📦 Web Interface

- Built with Vite + React
  - Built using custom build targets in PlatformIO (s. above)
- Development web-server (instead of ESP32's web server)
- React app uses Material Design
- Communicates with ESP via:
  - REST API
  - Server-Sent Events (SSE)

---

## 🧱 Case

- Designed in FreeCAD  
- File: `mech/case.FCStd`
- STL: `mech/export/case.stl`

<img src="doc/img/case_v1.png" />

---

## 📐 3D Design Resources

- [Amphenol D-Sub DE09P064TXLF](https://www.amphenol-cs.com/product/de09p064txlf.html)
- [GOOBAY 10013 switch datasheet](https://cdn-reichelt.de/documents/datenblatt/C200/GOOBAY_10013DB_DE.pdf)
- [Spax 3×12 screw](https://www.spax.com/de-de/p/universalschraube-vollgewinde-senkkopf-kreuzschlitz-z-4cut-yellox.html?variant=1081020300123)
- [1.3" OLED display](https://www.makershop.de/display/oled/oled-1-3-zoll-weiss/), especially [Datasheet](https://www.makershop.de/download/datasheet-oled-13.pdf)
- [ESP32 3D model (GrabCAD)](https://grabcad.com/library/esp32-38-pines-esp-wroom-32-1)  
  ⚠️ Requires scaling (X=0.976923, Z=1.016000)

---

## 🏗️ Example Build: Tensile Tester

<img src="doc/img/rope_strength_tester.jpg" width="600" />

### Shopping List

|Count|Item|Example|Cost|
|-:|-|-|-:|
|1|_esp-load-tester_|_(see above)_|60 €|
|2|Side bars|Wood bar (8 x 6)cm, length 130 cm, softwood sufficient|20 €|
|2|End bars|Wood bar (8 x 6)cm, length 46 cm, softwood sufficient|7 €|
|1|Hand winch, steel cable|E.g. Einhell TC-WI 500|31 €|
|1|Screw for load cell|M12 x 100mm, [carriage bolt](https://en.wikipedia.org/wiki/Carriage_bolt)|1 €|
|2|Screws for winch|Wood screw ⌀ 6 x 80mm|1 €|
|2|Washers for winch|Washer ⌀ 6.4mm x ⌀ 18mm|1 €|
|8|Screws for bars|Wood screw ⌀ 5 x 100mm|1 €|
|||||
|||**Total**|122 €|

---

## ⚠️ Limitations

- Low accuracy (~±5%)
- Low sampling rate (<5 Hz)
- No certified calibration
- Not suitable for safety-critical measurements

---

## 📌 TODO

- [ ] Add cross-platform setup instructions

---


## 📄 License

This project uses a dual-license approach:

### Software
All software components (firmware, web interface) are licensed under the  
**GNU General Public License v3.0 (GPL-3.0)**.

### Hardware
All hardware-related files (CAD models, mechanical designs, schematics) are licensed under the  
**CERN Open Hardware License v2 – Strongly Reciprocal (CERN-OHL-S-2.0)**.

This ensures that:
- Software modifications remain open (GPL-3.0)
- Hardware design improvements must also be shared (CERN-OHL-S)

See the respective license files for details:
- `LICENSE` (GPL-3.0)
- `LICENSE-HARDWARE` (CERN-OHL-S-2.0)