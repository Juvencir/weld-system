#pragma once

#include <Arduino.h>

#include "EndStop.h"
#include "PulseOutput.h"
#include "Types.h"

class AxisController {
   public:
    static constexpr uint32_t PULSE_DURATION_MS = 450;
    enum class State { IDLE, STARTING, MOVING, STOPPING, ABORTING, HOME };

    AxisController(uint32_t startPin, uint32_t stopPin, uint32_t dirPin, EndStop& endStop);

    void begin();
    void update(uint32_t now);

    void move(uint32_t now, Direction direction);
    void abort(uint32_t now);

    State getState() const { return _state; };
    Direction getDirection() const { return _currentDir; };
    bool stateChanged();
    bool isPending() const { return _start.isPending() || _stop.isPending(); }

   private:
    Direction _currentDir = Direction::RIGHT;
    State _state = State::IDLE;
    bool _stateChanged = false;

    bool _ignoringEndStop = false;

    PulseOutput _start;
    PulseOutput _stop;
    uint32_t _dirPin;

    EndStop& _endStop;

    void setState(State newState);
};
