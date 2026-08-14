#pragma once

#include <Arduino.h>

#include "Config.h"
#include "EndStop.h"
#include "PulseOutput.h"
#include "Types.h"

/**
 * @brief Controlador de eixo linear motorizado.
 *
 * Gerencia o movimento do carrinho por meio de pulsos de partida (start) e parada (stop),
 * controle do pino de direção e monitoramento do sensor de fim de curso (EndStop).
 *
 * Implementa uma máquina de estados (IDLE, STARTING, MOVING, STOPPING, ABORTING, HOME)
 * para garantir transições seguras entre acionamento, movimento, desaceleração e estado de repouso.
 */
class AxisController {
   public:
    /** Duração padrão do pulso de start/stop em milissegundos. */
    static constexpr uint32_t PULSE_DURATION_MS = Config::CARRIAGE_PULSE_DURATION_MS;


    /** Estados da máquina de estados do eixo. */
    enum class State {
        IDLE,      ///< Eixo parado e em repouso
        STARTING,  ///< Pulso de start acionado, aguardando conclusão
        MOVING,    ///< Eixo em movimento ativo
        STOPPING,  ///< Fim de curso atingido, pulso de stop acionado
        ABORTING,  ///< Movimento abortado, pulso de stop acionado
        HOME       ///< Eixo parado no fim de curso
    };

    /**
     * @param startPin Pino GPIO para comando de partida (start)
     * @param stopPin  Pino GPIO para comando de parada (stop)
     * @param dirPin   Pino GPIO para controle de direção
     * @param endStop  Referência para o sensor de fim de curso
     */
    AxisController(uint32_t startPin, uint32_t stopPin, uint32_t dirPin, EndStop& endStop);

    /**
     * Inicializa os pinos de controle, saídas pulsadas e o sensor de fim de curso.
     * Configura o pino de direção como saída em dreno aberto.
     */
    void begin();

    /**
     * Atualiza a máquina de estados e os temporizadores de pulso.
     * Deve ser chamado a cada iteração do loop principal.
     *
     * @param now Tempo atual em milissegundos (millis())
     */
    void update(uint32_t now);

    /**
     * Inicia o movimento do eixo na direção especificada.
     *
     * Chamada ignorada se o eixo não estiver em IDLE ou HOME, ou se houver pulso pendente.
     * Impede movimento na direção do fim de curso se ele já estiver acionado.
     *
     * @param now       Tempo atual em milissegundos (millis())
     * @param direction Direção do movimento (LEFT ou RIGHT)
     */
    void move(uint32_t now, Direction direction);

    /**
     * Aborta o movimento em andamento.
     *
     * Dispara o pulso de parada (stop) e altera o estado para ABORTING.
     * Ignorado se o eixo não estiver no estado MOVING ou se houver pulso pendente.
     *
     * @param now Tempo atual em milissegundos (millis())
     */
    void abort(uint32_t now);

    /** @return Estado atual da máquina de estados do eixo */
    State getState() const { return _state; };

    /** @return Direção atual do movimento */
    Direction getDirection() const { return _currentDir; };

    /**
     * Verifica se houve alteração de estado desde a última consulta.
     * Reseta a flag interna de mudança de estado ao ser chamado.
     *
     * @return true se o estado mudou
     */
    bool stateChanged();

    /** @return true se algum pulso (start ou stop) estiver pendente/ativo */
    bool isPending() const { return _start.isPending() || _stop.isPending(); }

   private:
    Direction _currentDir = Direction::RIGHT;
    State _state = State::IDLE;
    bool _stateChanged = false;

    // Flag para ignorar o sensor de fim de curso enquanto ele desativa após partir de HOME
    bool _ignoringEndStop = false;

    PulseOutput _start;  // Gerador de pulso para partida do eixo
    PulseOutput _stop;   // Gerador de pulso para parada do eixo
    uint32_t _dirPin;    // Pino de direção (LOW = LEFT, HIGH = RIGHT)

    EndStop& _endStop;   // Referência ao sensor de fim de curso

    /**
     * Atualiza o estado interno do eixo e sinaliza a alteração caso o novo estado seja diferente.
     *
     * @param newState Novo estado a ser atribuído
     */
    void setState(State newState);
};

