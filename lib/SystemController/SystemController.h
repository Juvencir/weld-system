#pragma once

#include <cstdint>

#include "AxisController.h"
#include "HMI.h"
#include "Types.h"

class SystemController {
   public:
    SystemController(HMI& hmi, AxisController& axisController);

    void begin();
    void update(uint32_t now);

   private:
    void home(uint32_t now, Direction direction);

    HMI& _hmi;
    AxisController& _axisController;
};
