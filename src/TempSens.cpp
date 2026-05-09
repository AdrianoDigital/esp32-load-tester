#include "TempSens.h"

TempSens::TempSens(StreamSSE& stream, InfoDisplay& info_display,
                   uint8_t ds18b20_pin)
    : stream(stream),
      info_display(info_display),
      one_wire(ds18b20_pin),
      dallas_temp(&one_wire),
      measurement_timeout(MEASUREMENT_PERIOD_MS) {
        pinMode(ds18b20_pin, INPUT_PULLUP);
      }

void TempSens::setup() { dallas_temp.begin(); }

void TempSens::handle_events() {
  if (measurement_timeout.is_over()) {
    float temperature = read_temperature_c();
    stream.send("temperature", temperature);
    info_display.main_temperature.set(temperature);
    measurement_timeout.restart();
  }
}

float TempSens::read_temperature_c() {
  dallas_temp.requestTemperatures();
  float tempC = dallas_temp.getTempCByIndex(0);
  if (tempC == -127.00) {
    Serial.println("Failed to read from DS18B20 sensor");
    return std::nanf("");
  }
  return tempC;
}
