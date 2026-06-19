#include "EndStop.h"

void EndStop::begin() {
    pinMode(_pin, INPUT);
    _instance = this;

    // Interrupção em ambas as bordas: detecta tanto aproximação quanto afastamento do ímã
    attachInterrupt(digitalPinToInterrupt(_pin), isrHandler, CHANGE);

    // Sincroniza estado inicial (inverte leitura porque sensor é ativo baixo)
    _state = !digitalRead(_pin);
}

void EndStop::isrHandler() { _instance->isrChange(); }

void EndStop::isrChange() { _state = !digitalRead(_pin); }
