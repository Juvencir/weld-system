#include "Button.h"

void Button::begin(callback_function_t isrHandler) { 
    pinMode(_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(_pin), isrHandler, CHANGE);
}

void Button::handleISRChange() {
    uint32_t now = millis();
    if (digitalRead(_pin) == LOW) {
        // Falling - Pressionou o botão
        _pressStartTime = now;
    } else {
        // Rising - Soltou o botão
        uint32_t duration = now - _pressStartTime;
        if (duration >= _minPressDuration) {
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