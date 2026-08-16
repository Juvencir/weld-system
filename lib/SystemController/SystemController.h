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
                     uint32_t moveStartOffsetMs, int32_t weldStopOffsetMs);

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
    void setOffsets(uint32_t moveStartOffsetMs, int32_t weldStopOffsetMs);

    /** @return Estado atual do ciclo de depósito */
    DepositState getDepositState() const { return _depositState; }

    /** @return Offset de início do movimento em milissegundos */
    uint32_t getMoveStartOffsetMs() const { return _moveStartOffsetMs; }

    /** @return Offset de parada da solda em milissegundos */
    int32_t getWeldStopOffsetMs() const { return _weldStopOffsetMs; }

    /** @return Duração configurada do pulso de solda em milissegundos */
    uint32_t getWeldPulseDurationMs() const { return _weldingController.durationMs(); }

    /** @return Duração configurada do pulso do eixo em milissegundos */
    uint32_t getCarriagePulseDurationMs() const { return AxisController::PULSE_DURATION_MS; }

    /**
     * @return Offset mínimo de início para que o comando do eixo ocorra em t >= 0 (tempo de abertura de poça).
     * Ex: 450ms (eixo) - 200ms (solda) = 250ms.
     */
    uint32_t getMinMoveStartOffsetMs() const {
        int32_t diff = static_cast<int32_t>(AxisController::PULSE_DURATION_MS) -
                       static_cast<int32_t>(_weldingController.durationMs());
        return (diff > 0) ? static_cast<uint32_t>(diff) : 0;
    }

    /**
     * @return Offset que resulta em disparo imediato de desligamento no fim de curso (delay = 0ms).
     * Ex: 200ms (solda) - 450ms (eixo) = -250ms.
     */
    int32_t getImmediateWeldStopOffsetMs() const {
        return static_cast<int32_t>(_weldingController.durationMs()) -
               static_cast<int32_t>(AxisController::PULSE_DURATION_MS);
    }

    /** @return Referência constante ao controlador do eixo */
    const AxisController& getAxisController() const { return _axisController; }

    /** @return Referência ao controlador do eixo */
    AxisController& getAxisController() { return _axisController; }

    /** @return Referência constante ao controlador da solda */
    const PulseOutput& getWeldingController() const { return _weldingController; }

    /** @return Referência ao controlador da solda */
    PulseOutput& getWeldingController() { return _weldingController; }

    /**
     * Calcula o atraso para disparar o comando de movimento do eixo a partir de t=0,
     * garantindo que o arco queime na chapa pelo tempo exato de moveStartOffsetMs:
     * delay = moveStartOffsetMs - (P_axis - P_weld) = moveStartOffsetMs - 250ms.
     * Mínimo físico: moveStartOffsetMs = 250ms (delay = 0ms).
     */
    uint32_t moveStartDelayMs() const {
        int32_t delay = static_cast<int32_t>(_moveStartOffsetMs) +
                        static_cast<int32_t>(_weldingController.durationMs()) -
                        static_cast<int32_t>(AxisController::PULSE_DURATION_MS);
        return (delay > 0) ? static_cast<uint32_t>(delay) : 0;
    }

    /**
     * Calcula o tempo de agendamento para desligar a solda após a detecção do fim de curso.
     * Sincronizado com a parada completa do eixo (pulso de 450ms):
     * - offset = 0ms: solda termina de desligar junto com a parada do eixo (+250ms delay)
     * - offset = -250ms: solda dispara imediatamente na detecção do sensor (0ms delay)
     * - offset > 0ms: solda desliga X ms após a parada do eixo
     */
    uint32_t weldStopDelayMs() const {
        int32_t delay = _weldStopOffsetMs + static_cast<int32_t>(AxisController::PULSE_DURATION_MS) -
                        static_cast<int32_t>(_weldingController.durationMs());
        return (delay > 0) ? static_cast<uint32_t>(delay) : 0;
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
    int32_t  _weldStopOffsetMs;   ///< Offset entre parada do eixo e desligamento da solda

    DepositState _depositState      = DepositState::IDLE;  ///< Estado atual do ciclo de depósito
    uint32_t     _depositPhaseStart = 0;  ///< Timestamp de início da fase de depósito atual
};
