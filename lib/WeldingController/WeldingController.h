#pragma once

#include <Arduino.h>

class WeldingController {
   public:
    static constexpr uint32_t PIN = PA0;

    static WeldingController& getInstance() {
        static WeldingController instance;
        return instance;
    }

    void begin();
    void update();

    void toggle();
    
   private:
    WeldingController() = default;
    
    uint32_t _lastChangeTime = 0;
    bool _changePending = false;
};