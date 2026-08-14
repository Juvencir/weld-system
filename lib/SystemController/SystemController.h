#pragma once

#include <cstdint>

#include "AxisController.h"
#include "PulseOutput.h"
#include "Types.h"

/**
 * @file SystemController.h
 * @brief Orquestrador do sistema. Coordena o movimento do eixo e a temporização
 * da soldagem WAAM.
 *
 * Gerencia dois modos de operação: homing simples (movimento até o fim de
 * curso) e depósito WAAM (aciona solda, aguarda offset, move eixo até o fim de
 * curso, aguarda offset e desliga solda).
 *
 * É uma classe puramente lógica e desacoplada de interfaces de usuário (Serial,
 * botões, etc.), disponibilizando métodos públicos para acionamento e consulta
 * de estado.
 */

/**
 * @class SystemController
 * @brief Controlador central e orquestrador do processo de depósito.
 */
class SystemController {
   public:
    /** Estados da máquina de estados do processo de depósito WAAM. */
    enum class DepositState {
        IDLE,              ///< Processo inativo
        TRIGGERING_START,  ///< Solda acionada, aguardando offset para iniciar movimento do eixo
        MOVING,            ///< Eixo em movimento durante o depósito
        WAITING_STOP,      ///< Eixo atingiu o fim de curso, aguardando offset para desligar a solda
        TRIGGERING_STOP    ///< Pulso de desligamento da solda disparado
    };

    /**
     * @param axisController     Referência ao controlador do eixo
     * @param weldingController  Referência ao PulseOutput da solda
     * @param moveStartOffsetMs  Offset entre ativação da solda e ativação do
     * movimento
     * @param weldStopOffsetMs   Offset entre parada do eixo e desativação da
     * solda
     */
    SystemController(AxisController& axisController, PulseOutput& weldingController,
                     uint32_t moveStartOffsetMs, uint32_t weldStopOffsetMs);

    /**
     * Inicializa os subsistemas de eixo e solda.
     */
    void begin();

    /**
     * Atualiza os subsistemas e gerencia a máquina de estados do ciclo de
     * depósito. Deve ser chamado continuamente no loop principal.
     *
     * @param now Tempo atual em milissegundos (millis())
     */
    void update(uint32_t now);

    /**
     * Move o eixo em uma direção até o fim de curso (homing simples).
     *
     * @param now       Tempo atual em milissegundos (millis())
     * @param direction Direção do movimento (LEFT ou RIGHT)
     * @return true se a solicitação foi aceita, false se o sistema estiver
     * ocupado
     */
    bool home(uint32_t now, Direction direction);

    /**
     * Inicia o ciclo de depósito WAAM: dispara a solda e agenda a partida do
     * eixo.
     *
     * @param now Tempo atual em milissegundos (millis())
     * @return true se o ciclo iniciou com sucesso, false se o sistema estiver
     * ocupado ou fora de HOME
     */
    bool deposit(uint32_t now);

    /**
     * Atualiza os offsets de temporização do depósito.
     *
     * @param moveStartOffsetMs Offset entre ativação da solda e partida do eixo
     * @param weldStopOffsetMs  Offset entre parada do eixo e desligamento da
     * solda
     */
    void setOffsets(uint32_t moveStartOffsetMs, uint32_t weldStopOffsetMs);

    /** @return Estado atual do ciclo de depósito */
    DepositState getDepositState() const { return _depositState; }

    /** @return Offset de início do movimento em milissegundos */
    uint32_t getMoveStartOffsetMs() const { return _moveStartOffsetMs; }

    /** @return Offset de parada da solda em milissegundos */
    uint32_t getWeldStopOffsetMs() const { return _weldStopOffsetMs; }

    /** @return Referência constante ao controlador do eixo */
    const AxisController& getAxisController() const { return _axisController; }

    /** @return Referência ao controlador do eixo */
    AxisController& getAxisController() { return _axisController; }

    /** @return Referência constante ao controlador da solda */
    const PulseOutput& getWeldingController() const { return _weldingController; }

    /** @return Referência ao controlador da solda */
    PulseOutput& getWeldingController() { return _weldingController; }

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

   private:
    /**
     * Gerencia as transições temporizadas do ciclo de depósito.
     *
     * @param now Tempo atual em milissegundos (millis())
     */
    void handleDeposit(uint32_t now);

    AxisController& _axisController;     ///< Controlador do eixo motorizado
    PulseOutput&    _weldingController;  ///< Controlador de pulso da solda

    uint32_t _moveStartOffsetMs;  ///< Offset entre ativação da solda e partida do eixo
    uint32_t _weldStopOffsetMs;   ///< Offset entre parada do eixo e desligamento da
                                  ///< solda

    DepositState _depositState      = DepositState::IDLE;  ///< Estado atual do ciclo de depósito
    uint32_t     _depositPhaseStart = 0;  ///< Timestamp de início da fase de depósito atual
};
