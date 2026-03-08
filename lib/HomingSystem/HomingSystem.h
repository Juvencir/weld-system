#pragma once

#include <Arduino.h>

#include <functional>

class HomingSystem {
   public:
    enum class State : uint8_t { NONE, LEFT, RIGHT, BOTH };

    static HomingSystem& getInstance() {
        static HomingSystem instance;
        return instance;
    }

    static const uint32_t SENSOR_LEFT_PIN = PA0;
    static const uint32_t SENSOR_RIGHT_PIN = PA1;

    void begin();

    State getState() const;

   private:
    HomingSystem() = default;
};