#pragma once

#include "Arduino.h"

class Timeout {
 private:
  unsigned long time_reference;
  unsigned long delay;

 public:
  Timeout() : time_reference(0), delay(0) {};
  Timeout(unsigned long delay) : time_reference(millis()), delay(delay) {};
  bool is_over() {
    unsigned int elaped = millis() - time_reference;
    return elaped > delay;
  };
  void restart() { time_reference = millis(); };
};
