#include <DallasTemperature.h>
#include <OneWire.h>

#include "StreamSSE.h"
#include "Timeout.h"
#include "InfoDisplay.h"

class TempSens {
 private:
  const int GPIO_ONE_WIRE_DEVICE = 14;  // GPIO14 = D5
  const unsigned long MEASUREMENT_PERIOD_MS = 5000;

  StreamSSE& stream;
  InfoDisplay& info_display;
  OneWire one_wire;
  DallasTemperature dallas_temp;
  Timeout measurement_timeout;

 public:
  TempSens(StreamSSE& stream, InfoDisplay& info_display);
  void setup();
  void handle_events();

  float readDSTemperatureC();
};
