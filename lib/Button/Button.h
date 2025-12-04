#pragma once

#include <Arduino.h>

#define BUTTON_DEFAULT_DEBOUNCE_DELAY 50
#define BUTTON_LONG_PRESS_DELAY 1000

enum ButtonEvent {
  BUTTON_EVENT_PRESS,
  BUTTON_EVENT_RELEASE,
  BUTTON_EVENT_LONG_PRESS
};

class Button {
 private:
  uint8_t pin;
  bool confirmedState = true;
  bool longPressState = false;
  unsigned long pressStartTime = 0;
  bool lastReadState = true;
  unsigned long lastDebounceTime = 0;
  void (*onEvent)(ButtonEvent) = nullptr;

 public:
  explicit Button(uint8_t pin);
  void begin();
  void update();
  bool isPressed();
  bool isLongPressed();
  unsigned long getPressDuration();
  void setCallback(void (*callback)(ButtonEvent));
};