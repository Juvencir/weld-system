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

    pinMode(Pins::HMI_LED_PIN, OUTPUT);
    digitalWrite(Pins::HMI_LED_PIN, LOW);
}

void HMI::update(uint32_t now) {
    if (_currentStatus == Status::ERROR) {
        if (now - _statusTimer >= 300) {
            _statusTimer = now;
            digitalWrite(Pins::HMI_LED_PIN, !digitalRead(Pins::HMI_LED_PIN));
        }
    }
}

void HMI::setStatus(Status status) {
    _currentStatus = status;
    switch (status) {
        case Status::IDLE:
            digitalWrite(Pins::HMI_LED_PIN, LOW);
            break;
        case Status::READY:
            digitalWrite(Pins::HMI_LED_PIN, HIGH);
            break;
        case Status::ERROR:
            _statusTimer = millis();
            digitalWrite(Pins::HMI_LED_PIN, HIGH);
            break;
    }
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
