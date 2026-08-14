#pragma once

#include <Arduino.h>

#include <cstdint>

/**
 * @brief Sensor de fim de curso por efeito Hall.
 *
 * O sensor já possui histerese no hardware, portanto não há tratamento
 * de debounce por software — a leitura do pino é considerada direta.
 *
 * A lógica é invertida: o sinal do sensor é ativo baixo (LOW = acionado),
 * então internamente @c _state armazena o inverso da leitura do pino.
 * getState() retorna @c true quando o sensor está acionado.
 *
 * Como a ISR da Arduino/STM32 exige uma função livre (não membro),
 * usa-se um ponteiro estático @c _instance para redirecionar ao objeto.
 */
class EndStop {
   public:
    /**
     * @param pin Pino GPIO onde o sensor de fim de curso está conectado
     */
    EndStop(uint32_t pin) : _pin(pin) {};

    /**
     * Configura o pino como entrada e anexa interrupção CHANGE.
     * Faz uma leitura inicial para sincronizar @c _state com o hardware.
     */
    void begin();

    /**
     * Retorna o estado atual do sensor.
     * @return true  = sensor acionado (ímã próximo)
     * @return false = sensor livre
     */
    bool getState() const { return _state; };

   private:
    const uint32_t _pin;           ///< Pino GPIO do sensor
    volatile bool _state = false;  ///< Estado do sensor (true = acionado)

    /** ISR não-estática executada pela instância ao detectar alteração no pino. */
    void isrChange();

    /** Ponteiro estático para redirecionar a ISR C-style ao método da instância. */
    inline static EndStop* _instance = nullptr;

    /** Handler estático da ISR registrado no attachInterrupt. */
    static void isrHandler();
};
