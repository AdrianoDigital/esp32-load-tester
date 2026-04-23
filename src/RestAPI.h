#pragma once

#include <ESPAsyncWebServer.h>

#include "misc_utils.h"

class RestAPI {
 private:
 protected:
  AsyncWebServer& server;
  bool redirect_onNotFound;
  
  // Helper functions
  static void reply_method_not_allowed(AsyncWebServerRequest* request);
  static bool assert_method_else_reply(AsyncWebServerRequest* request,
                                       AsyncWebRequestMethodType method);
  virtual void handle_api(AsyncWebServerRequest* request);

 public:
  RestAPI(AsyncWebServer& server, bool redirect_onNotFound = false);

  void setup();
};
