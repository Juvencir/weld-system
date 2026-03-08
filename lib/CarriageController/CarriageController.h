#pragma once

#include <cstdint>

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

    State getState() const;

    void setDirection(bool forward);

    bool isStalled() const;

   private:
    CarriageController();

    State state = State::STOPPED;

    BypassPin _start;
    BypassPin _stop;
    BypassPin _direction;

    uint32_t _changeTime = 0;
};