#include "PulseOutput.h"

void PulseOutput::begin(uint32_t mode) {
    digitalWrite(_pin, _activeState == HIGH ? LOW : HIGH);
    pinMode(_pin, mode);
}

void PulseOutput::update(uint32_t now) {
    if (_isPending && (now - _startTime >= _durationMs)) {
        digitalWrite(_pin, _activeState == HIGH ? LOW : HIGH);
        _isPending = false;
    }
}

void PulseOutput::trigger(uint32_t now) {
    if (!_isPending) {
        digitalWrite(_pin, _activeState);
        _startTime = now;
        _isPending = true;
    }
}
