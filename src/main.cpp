#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include <ArduinoJson.h>

#include "LittleFS.h"
#include "ScaleFSM.h"
#include "ScaleWebAPI.h"
#include "SimpleWifiAP.h"
#include "StreamSSE.h"
#include "TempSens.h"

AsyncWebServer server(80);
StreamSSE stream(server, "/stream");
SimpleWifiAP wifi_ap;
ScaleFSM scale_fsm(&stream);
ScaleWebAPI web_api(server, scale_fsm);
TempSens temp_sens(&stream);


void setup_static_file_server() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to initialize LittleFS system");
    // TODO: proper error state handling
  }
  server.serveStatic("/", LittleFS, "/www/").setDefaultFile("index.html");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  temp_sens.setup();

  wifi_ap.setup();
  web_api.setup();
  setup_static_file_server();
  scale_fsm.setup();
}

void loop() {
  scale_fsm.handleEvents();
  temp_sens.handle_events();
  wifi_ap.handle_events();
}
