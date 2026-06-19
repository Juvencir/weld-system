#include <Arduino.h>

#include "EndStop.h"
#include "Pins.h"

EndStop es(Pins::ENDSTOP_SENSOR_PIN);

void setup() {
    Serial.begin(115200);
    es.begin();
    // systemController.begin();
}

void loop() {}
