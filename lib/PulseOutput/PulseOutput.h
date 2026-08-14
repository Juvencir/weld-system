#pragma once

#include <Arduino.h>

#include <cstdint>

/**
 * @brief Gera um pulso lógico por tempo determinado e retorna ao repouso.
 *
 * Ao ser disparado, leva o pino ao @c activeState por @c durationMs ms,
 * depois retorna ao estado de repouso (inverso de @c activeState).
 *
 * Disparos repetidos são ignorados enquanto um pulso estiver pendente.
 * Útil para simular pressionamentos de botão.
 */
class PulseOutput {
   public:
    /**
     * @param pin         Pino GPIO a ser controlado
     * @param durationMs  Duração do pulso em milissegundos
     * @param activeState Estado lógico que ativa a carga (HIGH ou LOW)
     * @param mode        Modo do pino (ex: OUTPUT, OUTPUT_OPEN_DRAIN)
     */
    PulseOutput(uint32_t pin, uint32_t durationMs, uint8_t activeState, uint32_t mode)
        : _pin(pin), _durationMs(durationMs), _activeState(activeState), _mode(mode) {}

    /**
     * Inicializa o pino no estado de repouso (inverso de activeState).
     * O modo do pino foi definido no construtor.
     */
    void begin();

    /**
     * Deve ser chamado a cada iteração do loop.
     * Quando o tempo do pulso expira, retorna o pino ao repouso.
     */
    void update(uint32_t now);

    /**
     * Dispara o pulso.
     * Se já houver um pulso pendente, a chamada é ignorada.
     */
    void trigger(uint32_t now);

    /** @return true enquanto o pulso estiver ativo */
    bool isPending() const { return _isPending; }

    /**
     * @return Duração do pulso em milissegundos
     */
    uint32_t durationMs() const { return _durationMs; }

   private:
    const uint32_t _pin;
    const uint32_t _durationMs;
    const uint8_t  _activeState;
    const uint32_t _mode;

    uint32_t _startTime = 0;
    bool     _isPending = false;
};
