#include "HomingSystem.h"

#include "Pins.h"

void HomingSystem::begin() {
    pinMode(Pins::HOMING_SENSOR_LEFT_PIN, INPUT);
    pinMode(Pins::HOMING_SENSOR_RIGHT_PIN, INPUT);
}

HomingSystem::State HomingSystem::getState() const {
    bool left_triggered = (digitalRead(Pins::HOMING_SENSOR_LEFT_PIN) == LOW);
    bool right_triggered = (digitalRead(Pins::HOMING_SENSOR_RIGHT_PIN) == LOW);

    if (left_triggered && right_triggered) {
        return State::BOTH;
    } else if (left_triggered) {
        return State::LEFT;
    } else if (right_triggered) {
        return State::RIGHT;
    }

    return State::NONE;
}