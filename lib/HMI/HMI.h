#pragma once

#include <Arduino.h>

#include "Button.h"

class HMI {
   public:
    enum class Status : uint8_t { IDLE, READY, RUNNING };

    static HMI& getInstance() {
        static HMI instance;
        return instance;
    }

    void begin();
    void update(uint32_t now);

    bool isLeftPressed();
    bool isRightPressed();
    bool isTriggerPressed();

    void setStatus(Status status);

   private:
    HMI();

    static void isrLeft() { getInstance()._buttonLeft.handleISRChange(); }
    static void isrRight() { getInstance()._buttonRight.handleISRChange(); }
    static void isrTrigger() { getInstance()._buttonTrigger.handleISRChange(); }

    Button _buttonLeft;
    Button _buttonRight;
    Button _buttonTrigger;

    Status _currentStatus = Status::IDLE;
    uint32_t _statusTimer;
};
