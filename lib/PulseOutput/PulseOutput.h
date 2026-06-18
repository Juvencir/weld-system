#pragma once

#include <Arduino.h>

#include <cstdint>

class PulseOutput {
   public:
    /**
     * @param pin Pino do microcontrolador
     * @param durationMs Duração do pulso em milissegundos
     * @param activeState Estado lógico que ativa a carga (HIGH ou LOW)
     */
    PulseOutput(uint32_t pin, uint32_t durationMs, uint8_t activeState = HIGH)
        : _pin(pin), _durationMs(durationMs), _activeState(activeState) {}

    void begin(uint32_t mode);
    void update(uint32_t now);
    void trigger(uint32_t now);

    bool isPending() const { return _isPending; }

   private:
    const uint32_t _pin;
    const uint32_t _durationMs;
    const uint8_t _activeState;

    uint32_t _startTime = 0;
    bool _isPending = false;
};
