#pragma once

#include <cstdint>

class HomingSystem {
   public:
    enum class State : uint8_t { NONE, LEFT, RIGHT, BOTH };

    HomingSystem(uint32_t pinLeft, uint32_t pinRight) : _pinLeft(pinLeft), _pinRight(pinRight) {};

    void begin();

    State getState() const;

   private:
    const uint32_t _pinLeft;
    const uint32_t _pinRight;
};