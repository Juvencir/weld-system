#pragma once

#include <Arduino.h>

#define BUTTON_DEFAULT_DEBOUNCE_DELAY 50
#define BUTTON_LONG_PRESS_DELAY 1000

class Button {
 private:
  uint8_t pin;
  bool confirmedState = true;
  bool lastReadState = true;
  bool longPressState = false;
  unsigned long lastDebounceTime = 0;
  unsigned long pressStartTime = 0;
  void (*onPress)() = nullptr;
  void (*onRelease)() = nullptr;
  void (*onLongPress)() = nullptr;

 public:
  Button(uint8_t pin);
  void setCallbacks(
      void (*onPress)(),
      void (*onRelease)(),
      void (*onLongPress)());
  void begin();
  void update();
  bool isPressed();
  bool isLongPressed();
  unsigned long getPressDuration();
};