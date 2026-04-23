#include "RestAPI.h"

RestAPI::RestAPI(AsyncWebServer& server, bool redirect_onNotFound)
    : server(server), redirect_onNotFound(redirect_onNotFound) {}

bool RestAPI::assert_method_else_reply(AsyncWebServerRequest* request,
                                       AsyncWebRequestMethodType method) {
  if (request->method() != method) {
    reply_method_not_allowed(request);
    return false;
  } else {
    return true;
  }
}

void RestAPI::reply_method_not_allowed(AsyncWebServerRequest* request) {
  request->send(405, "application/json",
                simple_json("status", "Method not allowed"));
}

void RestAPI::handle_api(AsyncWebServerRequest* request) {}

void RestAPI::setup() {
  ArRequestHandlerFunction handle_api_binder =
      std::bind(&RestAPI::handle_api, this, std::placeholders::_1);

  server.on(AsyncURIMatcher::prefix("/api/v1/"), HTTP_ALL, handle_api_binder);
  if (redirect_onNotFound) {
    server.onNotFound(
        [](AsyncWebServerRequest* request) { request->redirect("/"); });
  }

  server.begin();
}
