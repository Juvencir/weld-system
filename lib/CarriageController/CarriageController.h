#pragma once

#include <Arduino.h>

#include "BypassPin.h"

class CarriageController {
   public:
    enum class State : uint8_t { STOPPED = 0, STARTING, STARTED, STOPPING };

    static constexpr uint32_t PIN_START = PA0;
    static constexpr uint32_t PIN_STOP = PA1;
    static constexpr uint32_t PIN_DIRECTION = PA0;

    static constexpr uint32_t CHANGE_DELAY = 20;

    static CarriageController& getInstance() {
        static CarriageController instance;
        return instance;
    }

    void begin();
    void update();

    bool start();
    bool stop();

    State getState() const { return state; };

    void setDirection(bool forward) { _direction.setState(forward ? HIGH : LOW); }

    bool isStalled() const {
        return _start.isStalled() || _stop.isStalled() || _direction.isStalled();
    }

   private:
    CarriageController() = default;

    State state = State::STOPPED;

    BypassPin _start{PIN_START};
    BypassPin _stop{PIN_STOP};
    BypassPin _direction{PIN_DIRECTION};

    uint32_t _changeTime = 0;
};