#include "CarriageController.h"

#include "Pins.h"

namespace {
constexpr uint32_t CHANGE_DELAY = 20;
}

CarriageController::CarriageController()
    : _start(Pins::CARRIAGE_START_PIN),
      _stop(Pins::CARRIAGE_STOP_PIN),
      _direction(Pins::CARRIAGE_DIR_PIN) {}

void CarriageController::begin() {
    _start.begin();
    _stop.begin();
    _direction.begin();
}

void CarriageController::update(uint32_t now) {
    _start.update();
    _stop.update();
    _direction.update();

    if (isStalled()) {
        state = State::STOPPED;
        return;
    }

    switch (state) {
        case State::STARTING:
            if (now - _changeTime >= CHANGE_DELAY) {
                state = State::STARTED;
                _start.setState(HIGH);
            }
            break;
        case State::STOPPING:
            if (now - _changeTime >= CHANGE_DELAY) {
                state = State::STOPPED;
                _stop.setState(HIGH);
            }
            break;
        default:
            break;
    }
}

bool CarriageController::start() {
    if (state != State::STOPPED || isStalled()) {
        return false;
    }
    _start.setState(LOW);
    state = State::STARTING;
    _changeTime = millis();
    return true;
}

bool CarriageController::stop() {
    if (state != State::STARTED || isStalled()) {
        return false;
    }
    _stop.setState(LOW);
    state = State::STOPPING;
    _changeTime = millis();
    return true;
}