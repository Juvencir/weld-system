#include <Arduino.h>

#include "AxisController.h"
#include "EndStop.h"
#include "HMI.h"
#include "Pins.h"
#include "SystemController.h"

EndStop endStop(Pins::ENDSTOP_SENSOR_PIN);
AxisController axisController(Pins::CARRIAGE_START_PIN, Pins::CARRIAGE_STOP_PIN,
                              Pins::CARRIAGE_DIR_PIN, endStop);
SystemController systemController(HMI::getInstance(), axisController);

void setup() {
    Serial.begin(115200);
    Serial.print("Inicio");
    systemController.begin();
}

void loop() { systemController.update(millis()); }
