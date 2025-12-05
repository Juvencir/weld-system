#pragma once

#include <Arduino.h>

class BypassPin{
    private:
        uint8_t pin;
        bool state = HIGH;
        bool stalled = true;
        void (*onStall)() = nullptr;
        //Possivel bug race condition o update pode ler o pino como LOW mesmo depois de setado para HIGH pois existe um tempo de subida

    public:
        explicit BypassPin(uint8_t pin, void (*onStall)() = nullptr) : pin(pin), onStall(onStall) {};
        void begin();
        void update();
        void end();
        void setState(bool state);
};
