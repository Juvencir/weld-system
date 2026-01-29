#pragma once

#include <Arduino.h>

class Button {
   public:
    enum class ButtonEvent { PRESS, RELEASE, LONG_PRESS };

    using EventHandler = void (*)(Button&, ButtonEvent);

    explicit Button(uint8_t pin, EventHandler callback = nullptr) : pin(pin), onEvent(callback) {}
    void begin();
    void update();
    bool isPressed();
    bool isLongPressed();
    uint32_t getPressDuration();
    void setCallback(EventHandler);

   private:
    const uint8_t pin;
    EventHandler onEvent;

    bool confirmedState = true;
    bool longPressState = false;
    uint32_t pressStartTime = 0;
    bool lastReadState = true;
    uint32_t lastDebounceTime = 0;
};