#pragma once

#include <Arduino.h>

#include "BypassPin.h"

constexpr uint8_t RELAY_PIN = 4;
constexpr uint8_t START_PIN = 5;
constexpr uint8_t STOP_PIN = 6;
constexpr uint8_t DIR_PIN = 7;

constexpr uint32_t STATE_CHANGE_DELAY = 20;

class CarriageController {
   public:
    enum State { IDLE, STARTING, STOPPING, CHANGING_DIRECTION, MOVING };

    static void begin();
    static void update();
    static void stop(BypassPin& pin);
    static bool isStalled();
    static void startMovement(bool direction);
    static void stopMovement();

   private:
    static inline BypassPin startPin = BypassPin(START_PIN, stop);
    static inline BypassPin stopPin = BypassPin(STOP_PIN, stop);
    static inline BypassPin dirPin = BypassPin(DIR_PIN, stop);
    static inline State state = IDLE;
    static inline uint32_t lastStateChangeTime = 0;
    static void setState(State newState);
};