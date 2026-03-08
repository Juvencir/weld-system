#pragma once

#include <functional>

#include "Button.h"

class HMI {
   public:
    static HMI& getInstance() {
        static HMI instance;
        return instance;
    }

    void begin();

    bool isLeftPressed();
    bool isRightPressed();
    bool isTriggerPressed();

    void setReadyLED(bool state);
    void setErrorLED(bool state);

   private:
    HMI();

    static void isrLeft() { getInstance()._buttonLeft.handleISRChange(); }
    static void isrRight() { getInstance()._buttonRight.handleISRChange(); }
    static void isrTrigger() { getInstance()._buttonTrigger.handleISRChange(); }

    Button _buttonLeft;
    Button _buttonRight;
    Button _buttonTrigger;
};