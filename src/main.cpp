#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "InfoDisplay.h"
#include "LittleFS.h"
#include "ScaleFSM.h"
#include "ScaleWebAPI.h"
#include "SimpleWifiAP.h"
#include "StreamSSE.h"
#include "TempSens.h"

InfoDisplay info_display("RopeSnap");
AsyncWebServer server(80);
StreamSSE stream(server, "/stream");
SimpleWifiAP wifi_ap(info_display);
ScaleFSM scale_fsm(stream, info_display, HX711_DOUT_PIN, HX711_SCK_PIN);

ScaleWebAPI web_api(server, scale_fsm);
TempSens temp_sens(stream, info_display, DS18B20_PIN);

void setup_little_fs() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to initialize LittleFS system");
    // TODO: proper error state handling
  }
}

void setup_static_file_server() {
  server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
}

void setup() {
  info_display.setup();
  info_display.boot("Init Serial ...");
  
  Serial.begin(115200);
  Serial.println();
  
  info_display.boot("Init LittleFS ...");
  setup_little_fs();

  info_display.boot("Init temp-sensor ...");
  temp_sens.setup();

  info_display.boot("Init wifi ...");
  wifi_ap.setup();
  info_display.boot("Init web ...");
  web_api.setup();
  setup_static_file_server();

  info_display.boot("Init load cell ...");
  scale_fsm.setup();

  info_display.boot("Finished booting");
  info_display.set_screen(InfoDisplay::SCREEN::MAIN);
}

void loop() {
  scale_fsm.handle_events();
  temp_sens.handle_events();
  wifi_ap.handle_events();
  info_display.handle_events();
}
