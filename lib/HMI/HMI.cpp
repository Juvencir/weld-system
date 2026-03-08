#include "HMI.h"

#include <Arduino.h>

#include "Pins.h"

HMI::HMI()
    : _buttonLeft(Pins::HMI_BTN_LEFT_PIN),
      _buttonRight(Pins::HMI_BTN_RIGHT_PIN),
      _buttonTrigger(Pins::HMI_BTN_TRIGGER_PIN) {}

void HMI::begin() {
    _buttonLeft.begin(isrLeft);
    _buttonRight.begin(isrRight);
    _buttonTrigger.begin(isrTrigger);

    pinMode(Pins::HMI_LED_READY_PIN, OUTPUT);
    pinMode(Pins::HMI_LED_ERROR_PIN, OUTPUT);
    setReadyLED(false);
    setErrorLED(false);
}

bool HMI::isLeftPressed() {
    if (_buttonLeft.wasPressed()) {
        _buttonLeft.getPressDuration();
        return true;
    }
    return false;
}

bool HMI::isRightPressed() {
    if (_buttonRight.wasPressed()) {
        _buttonRight.getPressDuration();
        return true;
    }
    return false;
}

bool HMI::isTriggerPressed() {
    constexpr uint32_t LONG_PRESS_DURATION = 1000;
    if (_buttonTrigger.wasPressed() && _buttonTrigger.getPressDuration() > LONG_PRESS_DURATION) {
        return true;
    }
    return false;
}

void HMI::setReadyLED(bool state) { digitalWrite(Pins::HMI_LED_READY_PIN, state); }

void HMI::setErrorLED(bool state) { digitalWrite(Pins::HMI_LED_ERROR_PIN, state); }
