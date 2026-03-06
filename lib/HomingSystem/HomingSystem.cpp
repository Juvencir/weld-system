#include "HomingSystem.h"

void HomingSystem::begin() {
    pinMode(SENSOR1_PIN, INPUT_PULLUP);
    pinMode(SENSOR2_PIN, INPUT_PULLUP);
    
    _currentState = readSensors();
}

void HomingSystem::update() {
    EndstopState newState = readSensors();

    if (newState != _currentState) {
        _currentState = newState;
        
        if (_currentState != EndstopState::NONE && _callback != nullptr) {
            _callback(_currentState);
        }
    }
}

void HomingSystem::onEndstopReached(EndstopCallback callback) {
    _callback = callback;
}

EndstopState HomingSystem::readSensors() {
    bool s1_triggered = (digitalRead(SENSOR1_PIN) == LOW);
    bool s2_triggered = (digitalRead(SENSOR2_PIN) == LOW);

    if (s1_triggered && s2_triggered) {
        return EndstopState::ERROR_BOTH;
    } else if (s1_triggered) {
        return EndstopState::MIN_REACHED;
    } else if (s2_triggered) {
        return EndstopState::MAX_REACHED;
    }
    
    return EndstopState::NONE;
}

EndstopState HomingSystem::getState() const {
    return _currentState;
}

bool HomingSystem::isMinReached() const {
    return _currentState == EndstopState::MIN_REACHED || _currentState == EndstopState::ERROR_BOTH;
}

bool HomingSystem::isMaxReached() const {
    return _currentState == EndstopState::MAX_REACHED || _currentState == EndstopState::ERROR_BOTH;
}
