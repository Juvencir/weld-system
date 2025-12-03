#include <Arduino.h>

#include "HumanInterface.h"
#include "WelderController.h"
#include "CarriageController.h"

void setup() {
    HumanInterface::begin();
}

void loop() {
    HumanInterface::update();
}