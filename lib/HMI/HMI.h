#pragma once

#include <functional>

#include "Button.h"

class HMI {
   public:
    static HMI& getInstance() {
        static HMI instance;
        return instance;
    }

    using callback = std::function<void()>;

    static constexpr uint32_t BTN_LEFT_PIN = PB13;
    static constexpr uint32_t BTN_RIGHT_PIN = PB14;
    static constexpr uint32_t BTN_TRIGGER_PIN = PB15;
    static constexpr uint32_t LED_READY_PIN = PA8;
    static constexpr uint32_t LED_STALLED_PIN = PA9;

    void begin(callback isrLeft, callback isrRight,
               callback isrTrigger);
    void update();

    void setTriggerReadyLED(bool state);
    void setBypassStalledLED(bool state);

    void onLeftPress(callback cb) { _leftCallback = cb; }
    void onRightPress(callback cb) { _rightCallback = cb; }
    void onTriggerLongPress(callback cb) { _triggerCallback = cb; }

   private:
    HMI() = default;

    static void isrLeft() { getInstance()._buttonLeft.handleISRChange(); }
    static void isrRight() { getInstance()._buttonRight.handleISRChange(); }
    static void isrTrigger() { getInstance()._buttonTrigger.handleISRChange(); }

    Button _buttonLeft{BTN_LEFT_PIN};
    Button _buttonRight{BTN_RIGHT_PIN};
    Button _buttonTrigger{BTN_TRIGGER_PIN};

    callback _leftCallback = nullptr;
    callback _rightCallback = nullptr;
    callback _triggerCallback = nullptr;
};