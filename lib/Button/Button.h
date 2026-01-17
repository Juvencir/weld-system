#pragma once

#include <Arduino.h>

constexpr uint32_t BUTTON_DEFAULT_DEBOUNCE_DELAY = 50;
constexpr uint32_t BUTTON_LONG_PRESS_DELAY = 1000;

class Button {
 public:
  enum ButtonEvent {
    BUTTON_EVENT_PRESS,
    BUTTON_EVENT_RELEASE,
    BUTTON_EVENT_LONG_PRESS
  };
  using EventHandler = void (*)(Button&, ButtonEvent);

  explicit Button(uint8_t pin, EventHandler callback = nullptr)
      : pin(pin), onEvent(callback) {}
  void begin();
  void update();
  bool isPressed();
  bool isLongPressed();
  uint32_t getPressDuration();
  void setCallback(EventHandler);

 private:
  const uint8_t pin;
  EventHandler onEvent;

  bool confirmedState = true;
  bool longPressState = false;
  uint32_t pressStartTime = 0;
  bool lastReadState = true;
  uint32_t lastDebounceTime = 0;
};