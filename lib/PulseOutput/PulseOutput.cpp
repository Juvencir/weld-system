#include "PulseOutput.h"

void PulseOutput::begin() {
    // Estado de repouso: inverso do activeState
    digitalWrite(_pin, _activeState == HIGH ? LOW : HIGH);
    pinMode(_pin, _mode);
}

void PulseOutput::update(uint32_t now) {
    if (_isPending && (now - _startTime >= _durationMs)) {
        // Tempo expirou — retorna ao repouso
        digitalWrite(_pin, _activeState == HIGH ? LOW : HIGH);
        _isPending = false;
    }
}

void PulseOutput::trigger(uint32_t now) {
    if (!_isPending) {
        // Inicia o pulso: vai para activeState e marca pendência
        digitalWrite(_pin, _activeState);
        _startTime = now;
        _isPending = true;
    }
    // Se já pendente, ignora (não re-triggers)
}
