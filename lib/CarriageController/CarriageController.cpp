#include "CarriageController.h"

void CarriageController::begin() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    startPin.begin();
    stopPin.begin();
    dirPin.begin();
    state = State::IDLE;
}

void CarriageController::update() {
    startPin.update();
    stopPin.update();
    dirPin.update();

    if (state != State::IDLE && state != State::MOVING) {
        unsigned long currentTime = millis();
        if (currentTime - lastStateChangeTime >= STATE_CHANGE_DELAY) {
            switch (state) {
                case State::STARTING:
                    startPin.setState(HIGH);
                    state = State::MOVING;
                    break;
                case State::STOPPING:
                    stopPin.setState(HIGH);
                    state = State::IDLE;
                    break;
                case State::CHANGING_DIRECTION:
                    setState(State::STARTING);
                    break;
            }
        }
    }
}

void CarriageController::setState(State newState) {
    state = newState;
    lastStateChangeTime = millis();
}

void CarriageController::stop(BypassPin& pin) {
    digitalWrite(RELAY_PIN, HIGH);

    startPin.end();
    stopPin.end();
    dirPin.end();
}

bool CarriageController::isStalled() {
    return startPin.isStalled() || stopPin.isStalled() || dirPin.isStalled();
}

void CarriageController::startMovement(bool direction) {
    if (isStalled()) return;
    if (state != State::IDLE) return;

    dirPin.setState(direction);
    setState(State::CHANGING_DIRECTION);
}

void CarriageController::stopMovement() {
    if (isStalled()) return;
    if (state != State::MOVING) return;

    stopPin.setState(LOW);
    setState(State::STOPPING);
}