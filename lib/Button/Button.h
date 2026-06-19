#pragma once
#include <Arduino.h>

/**
 * @brief Debounce de botão via software usando interrupção externa.
 *
 * Detecta bordas de descida e subida no pino, mede a duração do pressionamento
 * e só considera válido se exceder @p minPressDuration (evita ruído).
 *
 * O consumo do evento é explícito: após wasPressed() retornar true,
 * deve-se chamar getPressDuration() para limpar a flag.
 */
class Button {
   public:
    /**
     * @param pin               Pino GPIO conectado ao botão (pull-up externo esperado)
     * @param minPressDuration  Duração mínima em ms para considerar pressionamento válido
     */
    explicit Button(uint32_t pin, uint32_t minPressDuration = 50)
        : _pin(pin), _minPressDuration(minPressDuration) {};

    /**
     * Configura o pino como entrada e anexa a interrupção externa.
     * @param isrHandler Função callback chamada nas bordas de subida/descida.
     *                   Deve chamar handleISRChange() internamente.
     */
    void begin(callback_function_t isrHandler = nullptr);

    /**
     * Verifica se houve um pressionamento válido desde a última leitura.
     * @return true se um pressionamento completo (fall + rise) foi detectado.
     * @note Não consome o evento — use getPressDuration() para isso.
     */
    bool wasPressed() const;

    /**
     * Retorna a duração do último pressionamento válido e consome o evento.
     * O consumo é feito com interrupções desabilitadas (critical section)
     * para evitar race condition com a ISR.
     * @return Duração em ms, ou 0 se não houve evento.
     */
    uint32_t getPressDuration();

    /** Deve ser chamado pela ISR externa em cada borda (CHANGE) do pino. */
    void handleISRChange();

   private:
    const uint32_t _pin;
    const uint32_t _minPressDuration;

    volatile uint32_t _pressStartTime = 0;
    volatile uint32_t _lastPressDuration = 0;
    volatile bool _wasPressedEvent = false;
};
