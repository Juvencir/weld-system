#pragma once

#include <Arduino.h>

#include <cstdint>

class EndStop {
   public:
    EndStop(uint32_t pin) : _pin(pin) {};

    void begin();
    bool getState() const { return _state; };

   private:
    const uint32_t _pin;
    volatile bool _state = false;

    void isrChange();

    inline static EndStop* _instance = nullptr;
    static void isrHandler();
};
