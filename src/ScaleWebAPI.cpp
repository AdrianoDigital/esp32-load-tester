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
  } else if (request->url() == "/api/v1/scale/getCalibration") {
    api_get_calibration(request);
  } else if (request->url() == "/api/v1/scale/setCalibration") {
    api_set_calibration(request);
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

void ScaleWebAPI::api_get_calibration(AsyncWebServerRequest* request) {
  if (assert_method_else_reply(request, HTTP_GET)) {
    request->send(200, "application/json", scale_fsm.get_calibration_json());
  }
}

void ScaleWebAPI::api_set_calibration(AsyncWebServerRequest* request) {
  if (assert_method_else_reply(request, HTTP_GET)) {
    long offset;
    float scale;
    bool bad_request = false;
    if (!request->hasParam("offset") || !request->hasParam("scale")) {
      bad_request = true;
    } else {
      String offset_param_string = request->getParam("offset")->value();
      String scale_param_string = request->getParam("scale")->value();
      offset = offset_param_string.toInt();
      scale = scale_param_string.toFloat();
    }
    if (bad_request) {
      request->send(400, "application/json",
                    simple_json("status", "offset/scale missing or illegal"));
    } else {
      scale_fsm.set_calibration(offset, scale);
      request->send(202, "application/json",
                    simple_json("status", "Set new calibration offset/scale"));
    }
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
