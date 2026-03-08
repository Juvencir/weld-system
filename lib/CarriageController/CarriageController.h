#pragma once

#include <Arduino.h>

#include "BypassPin.h"

class CarriageController {
   public:
    enum class State : uint8_t { STOPPED, STARTING, STARTED, STOPPING };

    static CarriageController& getInstance() {
        static CarriageController instance;
        return instance;
    }

    void begin();
    void update(uint32_t now);

    bool start();
    bool stop();

    State getState() const { return state; };

    void setDirection(bool forward) { _direction.setState(forward ? HIGH : LOW); }

    bool isStalled() const {
        return _start.isStalled() || _stop.isStalled() || _direction.isStalled();
    }

   private:
    CarriageController();

    State state = State::STOPPED;

    BypassPin _start;
    BypassPin _stop;
    BypassPin _direction;

    uint32_t _changeTime = 0;
};