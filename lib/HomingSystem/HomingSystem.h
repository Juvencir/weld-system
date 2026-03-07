#pragma once

#include <Arduino.h>

#include <functional>

class HomingSystem {
   public:
    enum class State : uint8_t { NONE, LEFT, RIGHT, BOTH };
    using Callback = std::function<void(State state)>;

    static HomingSystem& getInstance() {
        static HomingSystem instance;
        return instance;
    }

    static const uint32_t SENSOR_LEFT_PIN = PA0;
    static const uint32_t SENSOR_RIGHT_PIN = PA1;

    void begin();
    void update();

    void onEndstopReached(Callback cb) { _callback = cb; };

    State getState() const { return _currentState; };

    bool HomingSystem::isLeftReached() const {
        return _currentState == State::LEFT || _currentState == State::BOTH;
    };

    bool HomingSystem::isRightReached() const {
        return _currentState == State::RIGHT || _currentState == State::BOTH;
    };

   private:
    HomingSystem() = default;

    State _currentState = State::NONE;
    Callback _callback = nullptr;

    State readSensors();
};