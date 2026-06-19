#include "Button.h"

void Button::begin(callback_function_t isrHandler) {
    pinMode(_pin, INPUT);
    // Interrupção em ambas as bordas para medir duração do pressionamento
    attachInterrupt(digitalPinToInterrupt(_pin), isrHandler, CHANGE);
}

void Button::handleISRChange() {
    uint32_t now = millis();
    if (digitalRead(_pin) == LOW) {
        // Borda de descida — início do pressionamento
        _pressStartTime = now;
    } else {
        // Borda de subida — fim do pressionamento
        uint32_t duration = now - _pressStartTime;
        // Filtro rejeita ruído (pressionamentos mais curtos que o mínimo)
        if (duration >= _minPressDuration) {
            _lastPressDuration = duration;
            _wasPressedEvent = true;
        }
    }
}

bool Button::wasPressed() const { return _wasPressedEvent; }

uint32_t Button::getPressDuration() {
    // Desabilita IRQs para leitura/limpeza atômica da flag compartilhada com a ISR
    noInterrupts();
    if (!_wasPressedEvent) {
        interrupts();
        return 0;
    }

    uint32_t duration = _lastPressDuration;
    _wasPressedEvent = false;  // Consome o evento
    interrupts();

    return duration;
}
