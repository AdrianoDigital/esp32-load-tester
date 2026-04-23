#pragma once
#include <ArduinoJson.h>

template <class T>
String simple_json(String key, T value) {
  JsonDocument response;
  String json;
  response[key] = value;
  serializeJson(response, json);
  return json;
}

