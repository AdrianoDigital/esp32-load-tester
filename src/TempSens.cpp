#include "TempSens.h"

TempSens::TempSens(StreamSSE& stream)
    : stream(stream),
      one_wire(GPIO_ONE_WIRE_DEVICE),
      dallas_temp(&one_wire),
      measurement_timeout(MEASUREMENT_PERIOD_MS) {}

void TempSens::setup() { dallas_temp.begin(); }

void TempSens::handle_events() {
  if (measurement_timeout.is_over()) {
    stream.send("temperature", readDSTemperatureC());
    measurement_timeout.restart();
  }
}

float TempSens::readDSTemperatureC() {
  // Call sensors.requestTemperatures() to issue a global temperature and
  // Requests to all devices on the bus
  dallas_temp.requestTemperatures();
  float tempC = dallas_temp.getTempCByIndex(0);

  if (tempC == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return -999;
  }
  return tempC;
}
