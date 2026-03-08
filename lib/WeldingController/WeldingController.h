#pragma once

#include <cstdint>

class WeldingController {
   public:
    WeldingController(uint32_t pin) : _pin(pin) {}

    void begin();
    void update(uint32_t now);

    void toggle();

   private:
    const uint32_t _pin;

    uint32_t _lastChangeTime = 0;
    bool _changePending = false;
};