#include "HMI.h"

void HMI::begin() {
    _buttonLeft.begin(isrLeft);
    _buttonRight.begin(isrRight);
    _buttonTrigger.begin(isrTrigger);

    pinMode(LED_READY_PIN, OUTPUT);
    pinMode(LED_ERROR_PIN, OUTPUT);
    setTriggerReadyLED(false);
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

void HMI::setTriggerReadyLED(bool state) { digitalWrite(LED_READY_PIN, state); }

void HMI::setErrorLED(bool state) { digitalWrite(LED_ERROR_PIN, state); }
