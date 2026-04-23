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
  // calVal_eepromAdress = 0;
  // 
  // EEPROM.put(calVal_eepromAdress, newCalibrationValue);
  // EEPROM.commit();
  // EEPROM.get(calVal_eepromAdress, newCalibrationValue);
  
  const unsigned int tare_timeout = 10000;
  const unsigned int calib_timeout = 10000;
  const unsigned int tare_average_factor = 40;
  const unsigned int calibration_average_factor = 40;
  const unsigned int stream_average_factor = 10;
  
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
