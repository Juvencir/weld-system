#include "Button.h"

namespace {
    constexpr uint32_t DEBOUNCE_DELAY = 50;
}

Button::Button(uint32_t pin) : _pin(pin) {}

void Button::begin(callback_function_t isrHandler) { 
    pinMode(_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_pin), isrHandler, CHANGE);
}

void Button::handleISRChange() {
    if (digitalRead(_pin) == LOW) {
        // Falling - Pressionou o botão
        _pressStartTime = millis();
    } else {
        // Rising - Soltou o botão
        uint32_t duration = millis() - _pressStartTime;
        if (duration >= DEBOUNCE_DELAY) {
            _lastPressDuration = duration;
            _wasPressedEvent = true;
        }
    }
}

bool Button::wasPressed() const {
    return _wasPressedEvent;
}

uint32_t Button::getPressDuration() {
    noInterrupts();
    if (!_wasPressedEvent) {
        interrupts();
        return 0;
    }
    
    uint32_t duration = _lastPressDuration;
    _wasPressedEvent = false;
    interrupts();
    
    return duration;
}