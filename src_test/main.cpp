#include <Arduino.h>

// #include "CarriageController.h"
// #include "HMI.h"
#include "HomingSystem.h"
// #include "SystemController.h"
#include "Pins.h"

// CarriageController carriageController(Pins::CARRIAGE_START_PIN, Pins::CARRIAGE_STOP_PIN,
// Pins::CARRIAGE_DIR_PIN);
HomingSystem homingSystem(Pins::HOMING_SENSOR_LEFT_PIN, Pins::HOMING_SENSOR_RIGHT_PIN);
// SystemController systemController(HMI::getInstance(), carriageController, homingSystem);

void callback() {
    HomingSystem::State state = homingSystem.getState();
    switch (state) {
        case HomingSystem::State::NONE:
            Serial.println("NONE");
            break;
        case HomingSystem::State::BOTH:
            Serial.println("BOTH");
            break;
        case HomingSystem::State::LEFT:
            Serial.println("LEFT");
            break;
        case HomingSystem::State::RIGHT:
            Serial.println("RIGHT");
            break;
        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    homingSystem.begin();
    homingSystem.setCallback(callback);
    // systemController.begin();
}

void loop() {}