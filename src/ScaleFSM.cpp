#include "ScaleFSM.h"

ScaleFSM::ScaleFSM(StreamSSE& stream, InfoDisplay& info_display,
                   uint8_t hx711_dout_pin, uint8_t hx711_sck_pin)
    : stream(stream),
      info_display(info_display),
      hx711_dout_pin(hx711_dout_pin),
      hx711_sck_pin(hx711_sck_pin),
      scale(),
      state(t_state::INITIALIZE),
      calib_known_mass(1),
      timeout(),
      stream_state_timeout(STREAM_STATE_PERIOD),
      raw_averager(),
      stream_averager(STREAM_AVERAGE_FACTOR) {}

void ScaleFSM::set_state(t_state new_state) {
  state = new_state;
  String stateStr = get_state_string();
  stream.send("state", stateStr);
  info_display.main_status.set(stateStr);
}

void ScaleFSM::set_error(String new_error) {
  Serial.println(new_error);
  stream.send("fsm_error", new_error);
}

void ScaleFSM::set_measurement(float value) {
  stream.send("weight", value);
  info_display.main_weight.set(value);
}

t_state ScaleFSM::get_state() { return state; }

String ScaleFSM::get_state_string() {
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

void ScaleFSM::load_calibration_from_littlefs() {
  String serial_json;
  JsonDocument settings;
  DeserializationError error;

  if (LittleFS.exists(fn_calibration)) {
    File file = LittleFS.open(fn_calibration, "r");
    if (!file) {
      Serial.println("Error: Failed to open '" + String(fn_calibration) +
                     "' for reading.");
      return;
    }
    serial_json = file.readString();
    file.close();
    deserializeJson(settings, serial_json);

    error = deserializeJson(settings, serial_json);
    if (error) {
      Serial.print("Error: deserializeJson() : ");
      Serial.println(error.c_str());
      return;
    }
    if (!settings["offset"].is<long>() || !settings["scale"].is<float>()) {
      Serial.println(
          "Error: Calibration JSON format error (Json: " + serial_json + ")");
      return;
    }
    Serial.println("Loading calibration data");
    scale.set_offset(settings["offset"]);
    scale.set_scale(settings["scale"]);
  }
}

void ScaleFSM::store_calibration_to_littlefs() {
  File file = LittleFS.open(fn_calibration, "w");
  if (!file) {
    Serial.println("Error: Failed to open '" + String(fn_calibration) +
                   "' for writing.");
    return;
  }
  Serial.println("Writing calibration to '" + String(fn_calibration) + "'");
  if (!file.println(get_calibration_json())) {
    Serial.println("Error: Failed to write settings");
    return;
  }
  file.close();
}

String ScaleFSM::get_calibration_json() {
  JsonDocument response;
  String serialized;
  response["offset"] = scale.get_offset();
  response["scale"] = scale.get_scale();
  serializeJson(response, serialized);
  return serialized;
}

void ScaleFSM::set_calibration(const long offset, const float scale_factor) {
  scale.set_offset(offset);
  scale.set_scale(scale_factor);
  store_calibration_to_littlefs();
}

void ScaleFSM::setup() {
  scale.begin(HX711_DOUT_PIN, HX711_SCK_PIN);
  return_to_streaming();
  load_calibration_from_littlefs();
}

bool ScaleFSM::start_tare() {
  if (state == t_state::STREAM) {
    abort_streaming();
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

bool ScaleFSM::start_calib(float knownMass) {
  if (state == t_state::STREAM) {
    abort_streaming();
  }
  if (state == t_state::READY) {
    calib_known_mass = knownMass;
    Serial.println("Starting calibration");
    raw_averager = Averager<long>(CALIBRATION_AVERAGE_FACTOR);
    timeout = Timeout(CALIB_TIMEOUT);
    set_state(t_state::CALIB);
    return true;
  } else {
    return false;
  }
}

void ScaleFSM::return_to_streaming() {
  if (AUTO_START_STREAMING) {
    stream_averager.clear();
    set_state(t_state::STREAM);
  } else {
    set_state(t_state::READY);
  }
}

bool ScaleFSM::abort_streaming() {
  if (state == t_state::STREAM) {
    set_state(t_state::READY);
    return true;
  } else {
    return false;
  }
}

void ScaleFSM::handle_events() {
  if (stream_state_timeout.is_over()) {
    set_state(state);
    stream_state_timeout.restart();
  }

  switch (state) {
    case t_state::READY:
      break;

    case t_state::TARE:
      if (timeout.is_over()) {
        set_error("Tare timed out");
        return_to_streaming();
      }
      if (scale.is_ready()) {
        raw_averager.add(scale.read());
        if (raw_averager.is_complete()) {
          Serial.printf("Tare done, set offset to %li\n",
                        raw_averager.average());
          scale.set_offset(raw_averager.average());
          store_calibration_to_littlefs();
          return_to_streaming();
        }
      }
      break;

    case t_state::CALIB:
      if (timeout.is_over()) {
        set_error("Calibration timed out");
        return_to_streaming();
      }
      if (scale.is_ready()) {
        raw_averager.add(scale.read() - scale.get_offset());
        if (raw_averager.is_complete()) {
          float scaling_factor =
              (float)raw_averager.average() / calib_known_mass;
          Serial.printf("Calibration done, set scale to %f\n", scaling_factor);
          scale.set_scale(scaling_factor);
          store_calibration_to_littlefs();
          return_to_streaming();
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
