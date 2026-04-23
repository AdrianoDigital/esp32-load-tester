#pragma once

#include <EEPROM.h>
#include <HX711.h>

#include "Averager.h"
#include "StreamSSE.h"
#include "Timeout.h"

enum class t_state { INITIALIZE, READY, TARE, CALIB, STREAM };

class ScaleFSM {
 private:
  const int HX711_DOUT_PIN = 12;
  const int HX711_SCK_PIN = 13;
  const int EEPROM_SETTINGS_ADDR = 0;
  const int EEPROM_EMULATION_SIZE = sizeof(t_settings);
  const unsigned long EEPROM_MAGIC = 0xDEADBEEF;
  const unsigned int TARE_TIMEOUT = 10000;
  const unsigned int CALIB_TIMEOUT = 10000;
  const unsigned int TARE_AVERAGE_FACTOR = 40;
  const unsigned int CALIBRATION_AVERAGE_FACTOR = 40;
  const unsigned int STREAM_AVERAGE_FACTOR = 10;
  const bool AUTO_START_STREAMING = true;
  
  typedef struct {
    unsigned long magic;
    long offset;
    float scale;
  } t_settings;

  StreamSSE* stream;
  HX711 scale;
  t_state state;
  String lastError;
  float lastMeasurement;
  float calibKnownMass;

  Timeout timeout;
  Averager<long> raw_averager;
  Averager<float> stream_averager;

  void set_state(t_state new_state);
  void set_error(String error);
  void set_measurement(float value);

  void load_calibration_from_eeprom();
  void store_calibration_to_eeprom();

 public:
  ScaleFSM(StreamSSE* stream);
  
  t_state getState();
  String getStateString();

  void setup();

  bool startTare();
  bool startCalib(float knownMass);
  bool startStreaming();
  bool stopStreaming();

  void handleEvents();
};
