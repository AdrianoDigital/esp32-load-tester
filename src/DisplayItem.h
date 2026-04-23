#pragma once

template <class T>
class DisplayItem {
 private:
  T value;
  bool modified;

 public:
  DisplayItem(T initial) : value(initial), modified(true) {};

  void set(T new_value) {
    modified = true;
    value = new_value;
  }
  T get() { return value; }
  T consume() {
    modified = false;
    return value;
  }
  void set_modified() { modified = true; };
  bool is_modified() { return modified; };
};
