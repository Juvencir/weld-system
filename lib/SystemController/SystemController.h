#pragma once

#include <cstdint>

#include "AxisController.h"
#include "HMI.h"
#include "Types.h"

/**
 * @brief Orquestrador do sistema. Coordena HMI, movimento e soldagem.
 *
 * Gerencia dois modos de operação: homing simples (vai ao fim de curso e para)
 * e depósito WAAM (aciona solda, move ao fim de curso, desliga solda).
 *
 * Os tempos de agendamento são calculados a partir de offsets relativos
 * passados no construtor, compensando automaticamente as durações dos pulsos
 * de solda e do eixo — já que ambos ativam/deativam ao retornar ao repouso.
 */
class SystemController {
   public:
    enum class DepositState { IDLE, WELD_TRIGGERING_START, MOVING, WELD_STOPPING, WELD_TRIGGERING_STOP };

    /**
     * @param hmi                Referência ao HMI (singleton)
     * @param axisController     Referência ao controlador do eixo
     * @param weldingController  Referência ao PulseOutput da solda
     * @param moveStartOffsetMs  Offset entre ativação da solda e ativação do movimento
     * @param weldStopOffsetMs   Offset entre HOME do eixo e desativação da solda
     */
    SystemController(HMI& hmi, AxisController& axisController, PulseOutput& weldingController,
                     uint32_t moveStartOffsetMs, uint32_t weldStopOffsetMs)
        : _hmi(hmi),
          _axisController(axisController),
          _weldingController(weldingController),
          _moveStartOffsetMs(moveStartOffsetMs),
          _weldStopOffsetMs(weldStopOffsetMs) {};

    void begin();
    void update(uint32_t now);

   private:
    /** Move o eixo em uma direção até o fim de curso (homing simples). */
    void home(uint32_t now, Direction direction);

    /** Inicia ciclo de depósito: aciona solda e agenda movimento. */
    void deposit(uint32_t now);

    /** Aborta o depósito em andamento e retorna o eixo ao IDLE. */
    // void abortDeposit(uint32_t now);

    void handleInputs(uint32_t now);
    void handleDeposit(uint32_t now);
    void updateStatus(uint32_t now);

    HMI& _hmi;
    AxisController& _axisController;
    PulseOutput& _weldingController;

    // Offset relativo entre ativação da solda e ativação do movimento
    uint32_t _moveStartOffsetMs;
    // Offset entre HOME do eixo e desativação da solda
    uint32_t _weldStopOffsetMs;

    /**
     * Calcula o tempo absoluto de agendamento do move() a partir do offset.
     *
     * Ambos os pulsos ativam ao retornar ao repouso:
     * - Solda: delay = durationMs (pulso vai HIGH e volta)
     * - Eixo:  delay = PULSE_DURATION_MS (start vai HIGH e volta)
     *
     * Como o agendamento acontece no momento de deposit() e ambos os pulsos
     * estão pendentes: sched = offset + dMs - P
     */
    uint32_t moveStartDelayMs() const {
        uint32_t sum = _moveStartOffsetMs + _weldingController.durationMs();
        return (sum >= AxisController::PULSE_DURATION_MS) ? sum - AxisController::PULSE_DURATION_MS
                                                          : 0;
    }

    /**
     * Calcula o tempo de agendamento para desligar a solda após o eixo parar.
     *
     * No momento do STOPPING o stop já disparou (HIGH por P ms) e a solda
     * será desligada via trigger (HIGH por dMs ms). Ambos os pulsos estão
     * pendentes: sched = offset + P - dMs
     */
    uint32_t weldStopDelayMs() const {
        uint32_t sum = _weldStopOffsetMs + AxisController::PULSE_DURATION_MS;
        return (sum >= _weldingController.durationMs()) ? sum - _weldingController.durationMs() : 0;
    }

    DepositState _depositState = DepositState::IDLE;
    uint32_t _depositPhaseStart;

    bool _abortWeldPending = false;
};
