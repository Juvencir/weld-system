#include "WeldingController.h"

#include "Pins.h"

namespace {
constexpr uint32_t CHANGE_DELAY = 100;
}

void WeldingController::begin() {
    pinMode(Pins::WELDING_PIN, OUTPUT);
    digitalWrite(Pins::WELDING_PIN, HIGH);
}

void WeldingController::update(uint32_t now) {
    if (_changePending && (now - _lastChangeTime >= CHANGE_DELAY)) {
        digitalWrite(Pins::WELDING_PIN, HIGH);
        _changePending = false;
    }
}

void WeldingController::toggle() {
    if (!_changePending) {
        digitalWrite(Pins::WELDING_PIN, LOW);
        _lastChangeTime = millis();
        _changePending = true;
    }
}
