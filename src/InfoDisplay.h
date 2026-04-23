#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Arduino.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <SPI.h>
#include <Wire.h>

#include "DisplayItem.h"
#include "Timeout.h"

class InfoDisplay {
 public:
  enum class SCREEN { BLANK, BOOT, MAIN };

 private:
  const long REFRESH_PERIOD_MS = 200;
  const int I2C_ADDRESS = 0x3c;
  const int SCREEN_WIDTH = 128;
  const int SCREEN_HEIGHT = 64;
  const int OLED_RESET = -1;
  String title;
  Adafruit_SH1106G display;
  Timeout refresh_timout;
  SCREEN active_screen;

  void _init_screen();
  void _redraw_screen();

  void _init_screen_blank();
  void _init_screen_boot();
  void _init_screen_main();

  void _redraw_screen_boot();
  void _redraw_screen_main();

  void _draw_boot_status();
  void _draw_main_weight();
  void _draw_main_status();

 public:
  DisplayItem<String> boot_status;
  DisplayItem<float> main_weight;
  DisplayItem<float> main_temperature;
  DisplayItem<unsigned int> main_clients;
  DisplayItem<String> main_status;

  InfoDisplay(String title);
  void setup();
  void handle_events(bool force = false);
  void explicit_redraw() { handle_events(true); };

  // Convenience functions
  void boot(String status) {
    set_screen(InfoDisplay::SCREEN::BOOT);
    boot_status.set(status);
    explicit_redraw();
  }

  void set_screen(SCREEN new_screen) {
    if (new_screen != active_screen) {
      active_screen = new_screen;
      _init_screen();
    }
  }
};
