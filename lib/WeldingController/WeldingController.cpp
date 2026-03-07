#include "WeldingController.h"

namespace {
    constexpr uint32_t CHANGE_DELAY = 100;
}

void WeldingController::begin() {
    pinMode(PIN, OUTPUT);
    digitalWrite(PIN, HIGH);
}

void WeldingController::update() {
    if (_changePending && (millis() - _lastChangeTime >= CHANGE_DELAY)) {
        digitalWrite(PIN, HIGH);
        _changePending = false;
    }
}

void WeldingController::toggle() {
    if (!_changePending) {
        digitalWrite(PIN, LOW);
        _lastChangeTime = millis();
        _changePending = true;
    }
}
