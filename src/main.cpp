#include <Arduino.h>

#include "AxisController.h"
#include "EndStop.h"
#include "HMI.h"
#include "Pins.h"
#include "SystemController.h"

EndStop endStop(Pins::ENDSTOP_SENSOR_PIN);
AxisController axisController(Pins::CARRIAGE_START_PIN, Pins::CARRIAGE_STOP_PIN,
                              Pins::CARRIAGE_DIR_PIN, endStop);
PulseOutput weldingController(Pins::WELDING_PIN, 200, LOW, OUTPUT);
SystemController systemController(HMI::getInstance(), axisController, weldingController, 3000, 0);

void setup() {
    Serial.begin(115200);
    systemController.begin();
}

void loop() { systemController.update(millis()); }
