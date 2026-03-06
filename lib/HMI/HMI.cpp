#include "HMI.h"

void HMI::begin(callback isrLeft, callback isrRight, callback isrTrigger) {
    _buttonLeft.begin(isrLeft);
    _buttonRight.begin(isrRight);
    _buttonTrigger.begin(isrTrigger);

    pinMode(LED_READY_PIN, OUTPUT);
    pinMode(LED_STALLED_PIN, OUTPUT);
}

void HMI::update() {
    if(_buttonLeft.wasPressed() && _leftCallback) {
        _buttonLeft.getPressDuration();
        _leftCallback();
    }
    if(_buttonRight.wasPressed() && _rightCallback) {
        _buttonRight.getPressDuration();
        _rightCallback();
    }
    if(_buttonTrigger.wasPressed() && _triggerCallback) {
        constexpr uint32_t LONG_PRESS_DURATION = 1000;
        if(_buttonTrigger.getPressDuration() > LONG_PRESS_DURATION){
            _triggerCallback();
        }
    }
}

void HMI::setTriggerReadyLED(bool state) {
    digitalWrite(LED_READY_PIN, state);
}

void HMI::setBypassStalledLED(bool state) {
    digitalWrite(LED_STALLED_PIN, state);
}
