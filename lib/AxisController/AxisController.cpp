#include "AxisController.h"

#include "Log.h"

bool AxisController::stateChanged() {
    if (_stateChanged) {
        _stateChanged = false;
        return true;
    }
    return false;
}

void AxisController::setState(State newState) {
    if (newState != _state) {
        LOG_AXIS("state changed");
        _state = newState;
        _stateChanged = true;
    }
}

AxisController::AxisController(uint32_t startPin, uint32_t stopPin, uint32_t dirPin,
                               EndStop& endStop)
    : _start(startPin, PULSE_DURATION_MS, HIGH, OUTPUT_OPEN_DRAIN),
      _stop(stopPin, PULSE_DURATION_MS, HIGH, OUTPUT_OPEN_DRAIN),
      _dirPin(dirPin),
      _endStop(endStop) {}

void AxisController::begin() {
    _endStop.begin();
    _start.begin();
    _stop.begin();

    digitalWrite(_dirPin, LOW);
    pinMode(_dirPin, OUTPUT_OPEN_DRAIN);
}

void AxisController::update(uint32_t now) {
    _start.update(now);
    _stop.update(now);

    // Se houver pulso ativo (start ou stop), aguarda a conclusão antes de transicionar de estado
    if (isPending()) {
        return;
    }

    // Ao se afastar do fim de curso, reseta a flag assim que o sensor for liberado
    if (_ignoringEndStop && !_endStop.getState()) {
        _ignoringEndStop = false;
    }

    // Transições de estado pós-conclusão dos pulsos
    if (_state == State::STARTING) {
        setState(State::MOVING);
    } else if (_state == State::STOPPING) {
        setState(State::HOME);
    } else if (_state == State::ABORTING) {
        setState(State::IDLE);
    } else if (_state == State::MOVING) {
        // Monitora ativamente o fim de curso durante o movimento
        if (_endStop.getState() && !_ignoringEndStop) {
            LOG_AXIS("end stop triggered -> STOPPING");
            _stop.trigger(now);
            setState(State::STOPPING);
        }
    }
}

void AxisController::move(uint32_t now, Direction direction) {
    // Permite movimento apenas se o eixo estiver em repouso (IDLE ou HOME) e sem pulsos pendentes
    if ((_state != State::IDLE && _state != State::HOME) || isPending()) {
        return;
    }

    // Impede movimento na mesma direção se já estiver parado no fim de curso acionado
    if (direction == _currentDir && (_state == State::HOME || _endStop.getState())) {
        return;
    }

    // Se partir de cima do fim de curso, ignora leituras até que o sensor seja liberado
    if (_endStop.getState()) {
        _ignoringEndStop = true;
    }

    _currentDir = direction;
    digitalWrite(_dirPin, (direction == Direction::LEFT) ? LOW : HIGH);

    LOG_AXIS("move -> STARTING");
    _start.trigger(now);
    setState(State::STARTING);
}

void AxisController::abort(uint32_t now) {
    // Permite abortar somente se o eixo estiver em movimento ativo e sem pulso pendente
    if (_state != State::MOVING || isPending()) {
        return;
    }

    LOG_AXIS("abort -> ABORTING");
    _stop.trigger(now);
    setState(State::ABORTING);
}
