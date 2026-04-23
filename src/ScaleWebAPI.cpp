#include "ScaleWebAPI.h"

ScaleWebAPI::ScaleWebAPI(AsyncWebServer& server, ScaleFSM& scale_fsm)
    : RestAPI(server), scale_fsm(scale_fsm) {}



void ScaleWebAPI::handle_api(AsyncWebServerRequest* request) {
  Serial.printf("handle_api, URL %s\n", request->url().c_str());

    if (request->url() == "/api/v1/scale/getState") {
      api_get_state(request);
    } else if (request->url() == "/api/v1/scale/startTare") {
      api_start_tare(request);
    } else if (request->url() == "/api/v1/scale/startCalib") {
      api_start_calib(request);
    } else if (request->url() == "/api/v1/scale/startStream") {
      api_start_stream(request);
    } else if (request->url() == "/api/v1/scale/stopStream") {
      api_stop_stream(request);
    } else {
      request->send(404, "text/plain", "Not found");
    } 
}

void ScaleWebAPI::api_get_state(AsyncWebServerRequest* request) {
  if (assert_method_else_reply(request, HTTP_GET)) {
    request->send(200, "application/json",
                  simple_json("state", scale_fsm.getStateString()));
  }
}

void ScaleWebAPI::api_start_tare(AsyncWebServerRequest* request) {
  if (assert_method_else_reply(request, HTTP_GET)) {
    if (scale_fsm.startTare()) {
      request->send(202, "application/json",
                    simple_json("status", "Taring started"));
    } else {
      request->send(
          412, "application/json",
          simple_json("status", "FSM currently unable to start Tare"));
    }
  }
}

void ScaleWebAPI::api_start_calib(AsyncWebServerRequest* request) {
  if (assert_method_else_reply(request, HTTP_GET)) {
    float known_mass;
    bool bad_request = false;
    if (!request->hasParam("known_mass")) {
      bad_request = true;
    } else {
      String known_mass_param_string = request->getParam("known_mass")->value();
      known_mass = known_mass_param_string.toFloat();
      if (known_mass == 0.0) {
        bad_request = true;
      }
    }
    if (bad_request) {
      request->send(400, "application/json",
                    simple_json("status", "known_mass missing or illegal"));
    } else {
      if (scale_fsm.startCalib(known_mass)) {
        request->send(202, "application/json",
                      simple_json("status", "Calibration started"));
      } else {
        request->send(
            412, "application/json",
            simple_json("status", "FSM currently unable to start Calibration"));
      }
    }
  }
}

void ScaleWebAPI::api_start_stream(AsyncWebServerRequest* request) {
  if (assert_method_else_reply(request, HTTP_GET)) {
    if (scale_fsm.startStreaming()) {
      request->send(202, "application/json",
                    simple_json("status", "Streaming started"));
    } else {
      request->send(
          412, "application/json",
          simple_json("status", "FSM currently unable to start Streaming"));
    }
  }
}

void ScaleWebAPI::api_stop_stream(AsyncWebServerRequest* request) {
  if (assert_method_else_reply(request, HTTP_GET)) {
    if (scale_fsm.stopStreaming()) {
      request->send(202, "application/json",
                    simple_json("status", "Streaming stopped"));
    } else {
      request->send(
          412, "application/json",
          simple_json("status", "FSM currently unable to stop Streaming"));
    }
  }
}
