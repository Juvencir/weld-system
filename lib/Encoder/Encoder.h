#pragma once

#include <Arduino.h>

class EncoderDriver {
   public:
    EncoderDriver(int pinA, int pinB) : _pinA(pinA), _pinB(pinB), _count(0) {};
    void begin();
    int32_t getCount() { return _count; };

   private:
    static void IRAM_ATTR handleInterrupt(void *arg);
    int _pinA;
    int _pinB;
    volatile int32_t _count;
};