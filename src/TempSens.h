#include <DallasTemperature.h>
#include <OneWire.h>
#include <cmath>

#include "InfoDisplay.h"
#include "StreamSSE.h"
#include "Timeout.h"

class TempSens {
 private:
  const unsigned long MEASUREMENT_PERIOD_MS = 5000;

  StreamSSE& stream;
  InfoDisplay& info_display;
  OneWire one_wire;
  DallasTemperature dallas_temp;
  Timeout measurement_timeout;

 public:
  TempSens(StreamSSE& stream, InfoDisplay& info_display, uint8_t ds18b20_pin);
  void setup();
  void handle_events();

  float readDSTemperatureC();
};
