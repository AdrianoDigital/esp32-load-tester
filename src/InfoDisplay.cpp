#include "InfoDisplay.h"

InfoDisplay::InfoDisplay(String title)
    : title(title),
      display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET),
      refresh_timout(REFRESH_PERIOD_MS),
      active_screen(SCREEN::BLANK),
      boot_status(""),
      main_weight(0.0),
      main_temperature(0.0),
      main_clients(0),
      main_status("") {}

void InfoDisplay::setup() {
  display.begin(I2C_ADDRESS, true);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  _init_screen();
}

void InfoDisplay::handle_events(bool force) {
  if (force || refresh_timout.is_over()) {
    _redraw_screen();
    refresh_timout.restart();
  }
}

void InfoDisplay::_init_screen() {
  switch (active_screen) {
    case SCREEN::BLANK:
      _init_screen_blank();
      break;
    case SCREEN::BOOT:
      _init_screen_boot();
      break;
    case SCREEN::MAIN:
      _init_screen_main();
      break;
    default:
      break;
  }
}

void InfoDisplay::_redraw_screen() {
  switch (active_screen) {
    case SCREEN::BOOT:
      _redraw_screen_boot();
      break;
    case SCREEN::MAIN:
      _redraw_screen_main();
      break;
    default:
      break;
  }
}

void InfoDisplay::_init_screen_blank() {
  display.clearDisplay();
  display.display();
}

void InfoDisplay::_init_screen_boot() {
  display.clearDisplay();
  display.drawRect(0, 0, 128, 48, SH110X_WHITE);
  display.fillRect(0, 48, 128, 16, SH110X_BLACK);
  display.setFont(&FreeSerif9pt7b);
  display.setCursor(25, 30);
  display.println(title);
  display.display();
  boot_status.set_modified();
}

void InfoDisplay::_init_screen_main() {
  display.clearDisplay();
  display.drawRect(0, 0, 128, 42, SH110X_WHITE);
  display.fillRect(0, 0, 10, 42, SH110X_WHITE);
  display.fillRect(118, 0, 10, 42, SH110X_WHITE);
  display.setFont();
  display.setCursor(105, 27);
  display.println("kg");
  display.display();
  main_weight.set_modified();
  main_temperature.set_modified();
  main_clients.set_modified();
  main_status.set_modified();
}

void InfoDisplay::_redraw_screen_boot() {
  if (boot_status.is_modified()) {
    _draw_boot_status();
    display.display();
  }
}

void InfoDisplay::_redraw_screen_main() {
  bool do_display = false;
  if (main_weight.is_modified()) {
    _draw_main_weight();
    do_display = true;
  }
  if (main_temperature.is_modified() || main_clients.is_modified() ||
      main_status.is_modified()) {
    _draw_main_status();
    do_display = true;
  }
  if (do_display) {
    display.display();
  }
}

void InfoDisplay::_draw_boot_status() {
  display.fillRect(0, 48, 128, 16, SH110X_BLACK);
  display.setFont();
  display.setCursor(1, 52);
  display.println(boot_status.consume());
}

void InfoDisplay::_draw_main_weight() {
  String num;
  String padded;
  String weight_string;
  float weight = main_weight.consume();
  // Format " 12.1", and above hundred " 561"
  if (int(weight) <= 99) {
    num = String(weight, 1);
  } else {
    num = String(weight, 0);
  }
  padded = String("   ") + num;
  weight_string = padded.substring(padded.length() - 4);
  display.fillRect(10, 1, 95, 40, SH110X_BLACK);

  display.setFont(&FreeSansBold24pt7b);
  display.setCursor(13, 35);
  display.println(weight_string);
}

void InfoDisplay::_draw_main_status() {
  String line1 = title + " " + String(main_temperature.consume(), 1) +
                 String('\xF7') + "C  " + String(main_clients.consume()) + "x";
  String line2 = main_status.consume();

  display.fillRect(0, 42, 128, 22, SH110X_BLACK);
  display.setFont();
  display.setCursor(3, 44);
  display.println(line1);
  display.setCursor(3, 54);
  display.println(line2);
}
