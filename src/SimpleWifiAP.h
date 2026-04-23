#pragma once

#include <Arduino.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>

#include "Timeout.h"

class SimpleWifiAP {
 private:
  const String ap_essid = ACCESS_POINT_ESSID;
  const String ap_psk = ACCESS_POINT_PSK;
  const IPAddress ap_local_ip = ACCESS_POINT_IP;
  const IPAddress ap_gateway = ACCESS_POINT_GATEWAY;
  const IPAddress ap_subnet = ACCESS_POINT_SUBNET;
  const int DNS_PORT = 53;
  const int DNS_PERIOD_MS = 1000;

  DNSServer dns_server;
  Timeout dns_periodic_timeout;

 public:
  SimpleWifiAP();
  ~SimpleWifiAP();

  void setup();
  void handle_events();
};

SimpleWifiAP::SimpleWifiAP()
    : dns_server(), dns_periodic_timeout(DNS_PERIOD_MS) {}

SimpleWifiAP::~SimpleWifiAP() {}

void SimpleWifiAP::setup() {
  WiFi.mode(WIFI_AP);

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(ap_local_ip, ap_gateway, ap_subnet)
                     ? "Ready"
                     : "Failed!");
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ap_essid, ap_psk) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  Serial.print("Starting DNS catch-all ... ");
  Serial.println(dns_server.start(DNS_PORT, "*", ap_local_ip) ? "Ready"
                                                              : "Failed");
}

void SimpleWifiAP::handle_events() {
    if(dns_periodic_timeout.is_over()) {
        dns_server.processNextRequest();
        dns_periodic_timeout.restart();
    }
 }
