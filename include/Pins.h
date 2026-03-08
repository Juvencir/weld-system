#pragma once

#include <Arduino.h>

namespace Pins {
    //HMI
    constexpr uint32_t HMI_BTN_LEFT_PIN = PB13;
    constexpr uint32_t HMI_BTN_RIGHT_PIN = PB14;
    constexpr uint32_t HMI_BTN_TRIGGER_PIN = PB15;
    constexpr uint32_t HMI_LED_READY_PIN = PA8;
    constexpr uint32_t HMI_LED_ERROR_PIN = PA9;

    //CarriageController
    constexpr uint32_t CARRIAGE_START_PIN = PA0;
    constexpr uint32_t CARRIAGE_STOP_PIN = PA1;
    constexpr uint32_t CARRIAGE_DIR_PIN = PA0;

    //WeldingController
    constexpr uint32_t WELDING_PIN = PA0;

    //HomingSystem
    static const uint32_t HOMING_SENSOR_LEFT_PIN = PA0;
    static const uint32_t HOMING_SENSOR_RIGHT_PIN = PA1;
}