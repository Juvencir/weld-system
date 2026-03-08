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

    void begin();

    State getState() const;

   private:
    HomingSystem() = default;
};