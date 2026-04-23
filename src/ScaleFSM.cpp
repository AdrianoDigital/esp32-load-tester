#include "ScaleFSM.h"

ScaleFSM::ScaleFSM(StreamSSE& stream, InfoDisplay& info_display,
                   uint8_t hx711_dout_pin, uint8_t hx711_sck_pin)
    : stream(stream),
      info_display(info_display),
      hx711_dout_pin(hx711_dout_pin),
      hx711_sck_pin(hx711_sck_pin),
      scale(),
      state(t_state::INITIALIZE),
      calibKnownMass(1),
      timeout(),
      raw_averager(),
      stream_averager(STREAM_AVERAGE_FACTOR) {
  EEPROM.begin(EEPROM_EMULATION_SIZE);
}

void ScaleFSM::set_state(t_state new_state) {
  state = new_state;
  String stateStr = getStateString();
  stream.send("state", stateStr);
  info_display.main_status.set(stateStr);
}

void ScaleFSM::set_error(String new_error) {
  Serial.println(new_error);
  stream.send("fsm_error", new_error);
}

void ScaleFSM::set_measurement(float value) {
  lastMeasurement = value;
  stream.send("weight", lastMeasurement);
  info_display.main_weight.set(value);
}

t_state ScaleFSM::getState() { return state; }

String ScaleFSM::getStateString() {
  switch (state) {
    case t_state::INITIALIZE:
      return "INITIALIZE";
    case t_state::READY:
      return "READY";
    case t_state::TARE:
      return "TARE";
    case t_state::CALIB:
      return "CALIB";
    case t_state::STREAM:
      return "STREAM";
    default:
      return "";
  }
}

void ScaleFSM::load_calibration_from_eeprom() {
  t_settings settings;
  EEPROM.get(EEPROM_SETTINGS_ADDR, settings);
  if (settings.magic == EEPROM_MAGIC) {
    Serial.println("Loading calibration data from EEPROM");
    scale.set_offset(settings.offset);
    scale.set_scale(settings.scale);
  } else {
    Serial.println("No valid calibration found in EEPROM");
  }
}

void ScaleFSM::store_calibration_to_eeprom() {
  t_settings settings;
  settings.magic = EEPROM_MAGIC;
  settings.offset = scale.get_offset();
  settings.scale = scale.get_scale();
  EEPROM.put(EEPROM_SETTINGS_ADDR, settings);
  Serial.println("Writing calibration data to EEPROM");
  EEPROM.commit();
}

void ScaleFSM::setup() {
  scale.begin(HX711_DOUT_PIN, HX711_SCK_PIN);
  if (AUTO_START_STREAMING) {
    set_state(t_state::STREAM);
  } else {
    set_state(t_state::READY);
  }
  load_calibration_from_eeprom();
}

bool ScaleFSM::startTare() {
  if (state == t_state::STREAM) {
    stopStreaming();
  }
  if (state == t_state::READY) {
    Serial.println("Starting tare");
    raw_averager = Averager<long>(TARE_AVERAGE_FACTOR);
    timeout = Timeout(TARE_TIMEOUT);
    set_state(t_state::TARE);
    return true;
  } else {
    return false;
  }
}

bool ScaleFSM::startCalib(float knownMass) {
  if (state == t_state::STREAM) {
    stopStreaming();
  }
  if (state == t_state::READY) {
    calibKnownMass = knownMass;
    Serial.println("Starting calibration");
    raw_averager = Averager<long>(CALIBRATION_AVERAGE_FACTOR);
    timeout = Timeout(CALIB_TIMEOUT);
    set_state(t_state::CALIB);
    return true;
  } else {
    return false;
  }
}

bool ScaleFSM::startStreaming() {
  if (state == t_state::READY) {
    stream_averager.clear();
    set_state(t_state::STREAM);
    return true;
  } else {
    return false;
  }
}

bool ScaleFSM::stopStreaming() {
  if (state == t_state::STREAM) {
    set_state(t_state::READY);
    return true;
  } else {
    return false;
  }
}

void ScaleFSM::handleEvents() {
  switch (state) {
    case t_state::READY:
      break;

    case t_state::TARE:
      if (timeout.is_over()) {
        set_error("Tare timed out");
        if (AUTO_START_STREAMING) {
          set_state(t_state::STREAM);
        } else {
          set_state(t_state::READY);
        }
      }
      if (scale.is_ready()) {
        raw_averager.add(scale.read());
        if (raw_averager.is_complete()) {
          Serial.printf("Tare done, set offset to %li\n",
                        raw_averager.average());
          scale.set_offset(raw_averager.average());
          store_calibration_to_eeprom();
          if (AUTO_START_STREAMING) {
            set_state(t_state::STREAM);
          } else {
            set_state(t_state::READY);
          }
        }
      }
      break;

    case t_state::CALIB:
      if (timeout.is_over()) {
        set_error("Calibration timed out");
        if (AUTO_START_STREAMING) {
          set_state(t_state::STREAM);
        } else {
          set_state(t_state::READY);
        }
      }
      if (scale.is_ready()) {
        raw_averager.add(scale.read() - scale.get_offset());
        if (raw_averager.is_complete()) {
          float scaling_factor = (float)raw_averager.average() / calibKnownMass;
          Serial.printf("Calibration done, set scale to %f\n", scaling_factor);
          scale.set_scale(scaling_factor);
          store_calibration_to_eeprom();
          if (AUTO_START_STREAMING) {
            set_state(t_state::STREAM);
          } else {
            set_state(t_state::READY);
          }
        }
      }
      break;

    case t_state::STREAM:
      if (scale.is_ready()) {
        float value = scale.get_units(1);
        stream_averager.add(value);
        if (stream_averager.is_complete()) {
          set_measurement(stream_averager.average());
          stream_averager.clear();
        }
      }
      break;
    default:
      break;
  }
}
