#include "WeldingController.h"

#include <Arduino.h>

#include "Pins.h"

namespace {
constexpr uint32_t CHANGE_DELAY = 100;
}

void WeldingController::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
}

void WeldingController::update(uint32_t now) {
    if (_changePending && (now - _lastChangeTime >= CHANGE_DELAY)) {
        digitalWrite(_pin, HIGH);
        _changePending = false;
    }
}

void WeldingController::toggle() {
    if (!_changePending) {
        digitalWrite(_pin, LOW);
        _lastChangeTime = millis();
        _changePending = true;
    }
}
