#pragma once

#include <Arduino.h>

enum class EndstopState : uint8_t {
    NONE = 0,
    MIN_REACHED, 
    MAX_REACHED,
    ERROR_BOTH
};

using EndstopCallback = void (*)(EndstopState state);

class HomingSystem {
   public:
    static HomingSystem& getInstance() {
        static HomingSystem instance;
        return instance;
    }

    static const uint32_t SENSOR1_PIN = PA0;
    static const uint32_t SENSOR2_PIN = PA1;

    void begin();
    void update();
    
    void onEndstopReached(EndstopCallback callback);

    EndstopState getState() const;
    bool isMinReached() const;
    bool isMaxReached() const;

   private:
    HomingSystem() = default;

    EndstopState _currentState = EndstopState::NONE;
    EndstopCallback _callback = nullptr;

    EndstopState readSensors();
};