#pragma once
#include <Arduino.h>

class Button {
   public:
    explicit Button(uint32_t pin, uint32_t minPressDuration = 50)
        : _pin(pin), _minPressDuration(minPressDuration) {};

    void begin(callback_function_t isrHandler = nullptr);

    bool wasPressed() const;

    uint32_t getPressDuration();

    void handleISRChange();

   private:
    const uint32_t _pin;
    const uint32_t _minPressDuration;

    volatile uint32_t _pressStartTime = 0;
    volatile uint32_t _lastPressDuration = 0;
    volatile bool _wasPressedEvent = false;
};