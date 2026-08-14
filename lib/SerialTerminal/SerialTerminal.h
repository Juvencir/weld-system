#pragma once

#include <Arduino.h>

#include "SystemController.h"

/**
 * @file SerialTerminal.h
 * @brief Interface de terminal serial para controle interativo do sistema WAAM.
 *
 * Desacoplada do SystemController, lê caracteres via Serial, processa comandos
 * de menu (L, R, T, S, I, H) e exibe logs de transição de estado e status do sistema.
 */

/**
 * @class SerialTerminal
 * @brief Interface de usuário via terminal Serial.
 */
class SerialTerminal {
   public:
    /**
     * @param systemController Referência ao orquestrador do sistema
     * @param stream           Stream de comunicação (padrão: Serial)
     */
    explicit SerialTerminal(SystemController& systemController, Stream& stream = Serial);

    /**
     * Inicializa a interface serial, exibindo mensagem de boas-vindas, ajuda e status inicial.
     */
    void begin();

    /**
     * Processa a entrada de dados seriais e monitora alterações de estado para exibição de logs.
     * Deve ser chamado continuamente no loop principal.
     *
     * @param now Tempo atual em milissegundos (millis())
     */
    void update(uint32_t now);

    /** Imprime o menu de ajuda com os comandos seriais disponíveis. */
    void printHelp();

    /** Imprime o status atual dos subsistemas (eixo, solda e depósito). */
    void printStatus();

   private:
    /**
     * Processa os caracteres recebidos pela interface Serial.
     *
     * @param now Tempo atual em milissegundos (millis())
     */
    void handleSerialInput(uint32_t now);

    SystemController& _systemController;  ///< Referência ao orquestrador do sistema
    Stream& _stream;                      ///< Referência ao fluxo de comunicação serial

    AxisController::State _prevAxisState = AxisController::State::IDLE;                   ///< Estado anterior do eixo para log
    SystemController::DepositState _prevDepositState = SystemController::DepositState::IDLE; ///< Estado anterior do depósito para log
};
