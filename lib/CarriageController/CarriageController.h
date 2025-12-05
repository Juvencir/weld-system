#pragma once

#include <Arduino.h>

#include "BypassPin.h"

#define FORWARD true
#define BACKWARD false

class CarriageController {
private:
    static BypassPin startPin;
    static BypassPin stopPin;
    static BypassPin dirPin;

public:

};