#pragma once

#include <ESPAsyncWebServer.h>

#include "misc_utils.h"

class StreamSSE {
  // Implements streaming to clients via Server Side Events (SSE)
 private:
 protected:
  AsyncEventSource sse;
  static void on_connect(AsyncEventSourceClient* client);
  static void on_disconnect(AsyncEventSourceClient* client);

  void send_raw(String channel, String value);

 public:
  StreamSSE(AsyncWebServer& server, const String url = "/stream");

  template <class T>
  void send(String channel, T value) {
    Serial.printf("Streaming %s: ", channel.c_str());
    Serial.println(value);
    send_raw(channel, (simple_json("value", value)).c_str());
  }
};
