#include "StreamSSE.h"

StreamSSE::StreamSSE(AsyncWebServer& server, const String url) : sse(url) {
  sse.onConnect(on_connect);
  sse.onDisconnect(on_disconnect);
  server.addHandler(&sse);
}

void StreamSSE::on_connect(AsyncEventSourceClient* client) {
  if (client->lastId()) {
    Serial.printf(
        "SSE Client reconnected! Last message ID that it got is: %u\n",
        client->lastId());
  }
}

void StreamSSE::on_disconnect(AsyncEventSourceClient* client) {
  Serial.println("SSE client disconnected!");
}

void StreamSSE::send_raw(String channel, String message) {
  sse.send(message.c_str(), channel.c_str(), millis());
}
