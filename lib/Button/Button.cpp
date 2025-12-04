#include "Button.h"

Button::Button(uint8_t pin) : pin(pin) {}

void Button::begin() {
  pinMode(pin, INPUT_PULLUP);
  lastDebounceTime = millis();
}

void Button::update() {
  bool currentState = digitalRead(pin);
  if (currentState != lastReadState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > BUTTON_DEFAULT_DEBOUNCE_DELAY) {
    if (currentState != confirmedState) {
      confirmedState = currentState;
      if (isPressed()) {
        pressStartTime = millis();
        if (onEvent) onEvent(BUTTON_EVENT_PRESS);
      } else {
        if (onEvent) onEvent(BUTTON_EVENT_RELEASE);
        longPressState = false;
      }
    }
  }
  if (isPressed() && !longPressState) {
    if (millis() - pressStartTime > BUTTON_LONG_PRESS_DELAY) {
      if (onEvent) onEvent(BUTTON_EVENT_LONG_PRESS);
      longPressState = true;
    }
  }
  lastReadState = currentState;
}

bool Button::isPressed() { return !confirmedState; }

unsigned long Button::getPressDuration() {
  if (!isPressed()) {
    return 0;
  }
  return millis() - pressStartTime;
}

bool Button::isLongPressed() {
  return !confirmedState && longPressState;
}

void Button::setCallback(void (*callback)(ButtonEvent)) {
  this->onEvent = callback;
}