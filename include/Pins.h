#pragma once

#include <Arduino.h>

namespace Pins {
    //HMI
    constexpr uint32_t HMI_BTN_LEFT_PIN = PB14;
    constexpr uint32_t HMI_BTN_RIGHT_PIN = PB13;
    constexpr uint32_t HMI_BTN_TRIGGER_PIN = PB15;
    constexpr uint32_t HMI_LED_PIN = PA15;

    //CarriageController
    constexpr uint32_t CARRIAGE_START_PIN = PB8;
    constexpr uint32_t CARRIAGE_STOP_PIN = PB9;
    constexpr uint32_t CARRIAGE_DIR_PIN = PB7;

    //WeldingController
    constexpr uint32_t WELDING_PIN = PA1;

    //EndStop
    static const uint32_t ENDSTOP_SENSOR_PIN = PA2;
}