#include "AxisController.h"

namespace {
constexpr uint32_t PULSE_DURATION_MS = 400;
}

bool AxisController::stateChanged() {
    if (_stateChanged) {
        _stateChanged = false;
        return true;
    }
    return false;
}

void AxisController::setState(State newState) {
    if (newState != _state) {
        _state = newState;
        _stateChanged = true;
    }
}

AxisController::AxisController(uint32_t startPin, uint32_t stopPin, uint32_t dirPin,
                               EndStop& endStop)
    : _start(startPin, PULSE_DURATION_MS, HIGH),
      _stop(stopPin, PULSE_DURATION_MS, HIGH),
      _dirPin(dirPin),
      _endStop(endStop) {}

void AxisController::begin() {
    _endStop.begin();
    _start.begin(OUTPUT_OPEN_DRAIN);
    _stop.begin(OUTPUT_OPEN_DRAIN);

    digitalWrite(_dirPin, LOW);
    pinMode(_dirPin, OUTPUT_OPEN_DRAIN);
}

void AxisController::update(uint32_t now) {
    _start.update(now);
    _stop.update(now);

    if (isPending()) {
        return;
    }

    if (_ignoringEndStop && !_endStop.getState()) {
        _ignoringEndStop = false;
    }

    if (_state == State::STARTING) {
        setState(State::MOVING);
    } else if (_state == State::STOPPING) {
        setState(State::HOME);
    } else if (_state == State::ABORTING) {
        setState(State::IDLE);
    } else if (_state == State::MOVING) {
        if (_endStop.getState() && !_ignoringEndStop) {
            _stop.trigger(now);
            setState(State::STOPPING);
        }
    }
}

void AxisController::move(uint32_t now, Direction direction) {
    if ((_state != State::IDLE && _state != State::HOME) || isPending()) {
        return;
    }

    if (direction == _currentDir && _state == State::HOME) {
        return;
    }

    if (_endStop.getState()) {
        _ignoringEndStop = true;
    }

    _currentDir = direction;
    digitalWrite(_dirPin, (direction == Direction::LEFT) ? LOW : HIGH);

    _start.trigger(now);
    setState(State::STARTING);
}

void AxisController::abort(uint32_t now) {
    if (_state != State::MOVING || isPending()) {
        return;
    }

    _stop.trigger(now);
    setState(State::ABORTING);
}
