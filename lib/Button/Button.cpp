#include "Button.h"

Button::Button(uint8_t pin) : pin(pin) {}

void Button::begin() {
  pinMode(pin, INPUT_PULLUP);
  bool state = digitalRead(pin);
  confirmedState = state;
  lastReadState = state;
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
      if (!confirmedState) {
        pressStartTime = millis();
        if (onPress) onPress();
      } else {
        if (onRelease) onRelease();
        longPressState = false;
      }
    }
  }
  if (!confirmedState && onLongPress && !longPressState) {
    if (millis() - pressStartTime > BUTTON_LONG_PRESS_DELAY) {
      onLongPress();
      longPressState = true;
    }
  }
  lastReadState = currentState;
}

bool Button::isPressed() { return !confirmedState; }

unsigned long Button::getPressDuration() {
  if (confirmedState) {
    return 0;
  }
  return millis() - pressStartTime;
}

bool Button::isLongPressed() {
  return !confirmedState && longPressState;
}

void Button::setCallbacks(void (*onPress)(), void (*onRelease)(),
                          void (*onLongPress)()) {
  this->onPress = onPress;
  this->onRelease = onRelease;
  this->onLongPress = onLongPress;
}