#include "Button.h"

namespace {
constexpr uint32_t BUTTON_DEFAULT_DEBOUNCE_DELAY = 50;
constexpr uint32_t BUTTON_LONG_PRESS_DELAY = 1000;
}

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
                if (onEvent) onEvent(*this, Button::BUTTON_EVENT_PRESS);
            } else {
                if (onEvent) onEvent(*this, Button::BUTTON_EVENT_RELEASE);
                longPressState = false;
            }
        }
    }
    if (isPressed() && !longPressState) {
        if (millis() - pressStartTime > BUTTON_LONG_PRESS_DELAY) {
            if (onEvent) onEvent(*this, Button::BUTTON_EVENT_LONG_PRESS);
            longPressState = true;
        }
    }
    lastReadState = currentState;
}

bool Button::isPressed() { return !confirmedState; }

u_int32_t Button::getPressDuration() {
    if (!isPressed()) {
        return 0;
    }
    return millis() - pressStartTime;
}

bool Button::isLongPressed() { return !confirmedState && longPressState; }

void Button::setCallback(EventHandler callback) { this->onEvent = callback; }