#pragma once

#include <ArduinoJson.h>
#include <HX711.h>
#include <LittleFS.h>

#include "Averager.h"
#include "InfoDisplay.h"
#include "StreamSSE.h"
#include "Timeout.h"

enum class t_state { INITIALIZE, READY, TARE, CALIB, STREAM };

class ScaleFSM {
 private:
  const char* fn_calibration = "/calib.json";
  const unsigned int TARE_TIMEOUT = 10000;
  const unsigned int CALIB_TIMEOUT = 10000;
  const unsigned int TARE_AVERAGE_FACTOR = 40;
  const unsigned int CALIBRATION_AVERAGE_FACTOR = 40;
  const unsigned int STREAM_AVERAGE_FACTOR = 3;
  const bool AUTO_START_STREAMING = true;
  const long STREAM_STATE_PERIOD = 5000;

  StreamSSE& stream;
  InfoDisplay& info_display;
  uint8_t hx711_dout_pin;
  uint8_t hx711_sck_pin;

  HX711 scale;
  t_state state;
  float calib_known_mass;

  Timeout timeout;
  Timeout stream_state_timeout;
  Averager<long> raw_averager;
  Averager<float> stream_averager;

  void set_state(t_state new_state);
  void set_error(String error);
  void set_measurement(float value);

  void load_calibration_from_littlefs();
  void store_calibration_to_littlefs();

  void return_to_streaming();
  bool abort_streaming();
  
 public:
  ScaleFSM(StreamSSE& stream, InfoDisplay& info_display, uint8_t hx711_dout_pin,
           uint8_t hx711_sck_pin

  );

  t_state get_state();
  String get_state_string();

  String get_calibration_json();
  void set_calibration(const long offset, const float scale_factor);

  void setup();

  bool start_tare();
  bool start_calib(float knownMass);
  
  void handle_events();
};
