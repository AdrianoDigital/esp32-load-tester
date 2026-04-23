#include <DallasTemperature.h>
#include <OneWire.h>

#include "StreamSSE.h"
#include "Timeout.h"

class TempSens {
 private:
  const int GPIO_ONE_WIRE_DEVICE = 14;  // GPIO14 = D5
  const unsigned long MEASUREMENT_PERIOD_MS = 5000;

  StreamSSE* stream;
  OneWire one_wire;
  DallasTemperature dallas_temp;
  Timeout measurement_timeout;

 public:
  TempSens(StreamSSE* stream);
  void setup();
  void handle_events();

  float readDSTemperatureC();
};
