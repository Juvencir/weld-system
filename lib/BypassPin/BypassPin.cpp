#include "BypassPin.h"

namespace {
constexpr uint32_t STALL_THRESHOLD = 100;
}

void BypassPin::begin() {
    state = HIGH;
    stalled = false;
    digitalWrite(pin, HIGH);
    lastHighTime = millis();
    pinMode(pin, OUTPUT_OPEN_DRAIN);
}

void BypassPin::update() {
    if (!isStalled()) checkStall();
}

void BypassPin::checkStall() {
    if (isStalled()) return;

    if (state == HIGH && digitalRead(pin) == LOW) {
        uint32_t currentTime = millis();
        if (currentTime - lastHighTime > STALL_THRESHOLD) {
            end();
        }
    }
}

void BypassPin::end() {
    stalled = true;
    pinMode(pin, INPUT);
    if (onStall) onStall(*this);
}

void BypassPin::setState(bool state) {
    if (isStalled()) return;

    this->state = state;
    digitalWrite(pin, state ? HIGH : LOW);
    if (state == HIGH) {
        lastHighTime = millis();
    }
}