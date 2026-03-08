#pragma once

#include <functional>

#include "Button.h"

class HMI {
   public:
    static HMI& getInstance() {
        static HMI instance;
        return instance;
    }

    static constexpr uint32_t BTN_LEFT_PIN = PB13;
    static constexpr uint32_t BTN_RIGHT_PIN = PB14;
    static constexpr uint32_t BTN_TRIGGER_PIN = PB15;
    static constexpr uint32_t LED_READY_PIN = PA8;
    static constexpr uint32_t LED_ERROR_PIN = PA9;

    void begin();

    bool isLeftPressed();
    bool isRightPressed();
    bool isTriggerPressed();

    void setTriggerReadyLED(bool state);
    void setBypassStalledLED(bool state);
    void setErrorLED(bool state);

   private:
    HMI() = default;

    static void isrLeft() { getInstance()._buttonLeft.handleISRChange(); }
    static void isrRight() { getInstance()._buttonRight.handleISRChange(); }
    static void isrTrigger() { getInstance()._buttonTrigger.handleISRChange(); }

    Button _buttonLeft{BTN_LEFT_PIN};
    Button _buttonRight{BTN_RIGHT_PIN};
    Button _buttonTrigger{BTN_TRIGGER_PIN};
};