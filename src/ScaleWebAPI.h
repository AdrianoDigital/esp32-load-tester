#pragma once

#ifdef ESP32
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>

#include <functional>

#include "RestAPI.h"
#include "ScaleFSM.h"
#include "Timeout.h"
#include "misc_utils.h"

class ScaleWebAPI : public RestAPI {
 private:
  ScaleFSM& scale_fsm;

  void api_stop_stream(AsyncWebServerRequest* request);
  void api_start_stream(AsyncWebServerRequest* request);
  void api_start_calib(AsyncWebServerRequest* request);
  void api_start_tare(AsyncWebServerRequest* request);
  void api_get_last_error(AsyncWebServerRequest* request);
  void api_get_state(AsyncWebServerRequest* request);

 protected:
  void handle_api(AsyncWebServerRequest* request);

 public:
  ScaleWebAPI(AsyncWebServer& server, ScaleFSM& scale_fsm);
};
