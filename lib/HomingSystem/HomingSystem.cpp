#include "HomingSystem.h"

#include <Arduino.h>

void HomingSystem::begin() {
    pinMode(_pinLeft, INPUT);
    pinMode(_pinRight, INPUT);
}

HomingSystem::State HomingSystem::getState() const {
    bool left_triggered = (digitalRead(_pinLeft) == LOW);
    bool right_triggered = (digitalRead(_pinRight) == LOW);

    if (left_triggered && right_triggered) {
        return State::BOTH;
    } else if (left_triggered) {
        return State::LEFT;
    } else if (right_triggered) {
        return State::RIGHT;
    }

    return State::NONE;
}