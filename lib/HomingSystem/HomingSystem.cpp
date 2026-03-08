#include "HomingSystem.h"

void HomingSystem::begin() {
    pinMode(SENSOR_LEFT_PIN, INPUT_PULLUP);
    pinMode(SENSOR_RIGHT_PIN, INPUT_PULLUP);
}

HomingSystem::State HomingSystem::getState() const {
    bool s1_triggered = (digitalRead(SENSOR_LEFT_PIN) == LOW);
    bool s2_triggered = (digitalRead(SENSOR_RIGHT_PIN) == LOW);

    if (s1_triggered && s2_triggered) {
        return State::BOTH;
    } else if (s1_triggered) {
        return State::LEFT;
    } else if (s2_triggered) {
        return State::RIGHT;
    }
    
    return State::NONE;
}