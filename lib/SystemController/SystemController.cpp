#include "SystemController.h"

#include "Log.h"

void SystemController::begin() {
    _hmi.begin();
    _axisController.begin();
    _weldingController.begin();
}

void SystemController::update(uint32_t now) {
    _hmi.update(now);
    _axisController.update(now);
    _weldingController.update(now);

    handleInputs(now);
    handleDeposit(now);
    if (_axisController.stateChanged() && _depositState == DepositState::IDLE) {
        updateStatus(now);
    }
}

void SystemController::handleDeposit(uint32_t now) {
    switch (_depositState) {
        case DepositState::IDLE:
            break;
        case DepositState::WELD_TRIGGERING_START:
            if (now - _depositPhaseStart >= moveStartDelayMs()) {
                Direction dir = (_axisController.getDirection() == Direction::LEFT)
                                    ? Direction::RIGHT
                                    : Direction::LEFT;

                LOG_SYSTEM("Deposit: weld started -> moving axis");
                _axisController.move(now, dir);
                _depositState = DepositState::MOVING;
            }
            break;
        case DepositState::MOVING:
            if (_axisController.getState() != AxisController::State::MOVING && _axisController.getState() != AxisController::State::STARTING) {
                LOG_SYSTEM("Deposit: axis stopped -> stopping weld");
                _depositPhaseStart = now;
                _depositState = DepositState::WELD_STOPPING;
            }
            break;
        case DepositState::WELD_STOPPING:
            if (now - _depositPhaseStart >= weldStopDelayMs()){
                LOG_SYSTEM("Deposit: triggering weld stop");
                _weldingController.trigger(now);
                _depositState = DepositState::WELD_TRIGGERING_STOP;
            }
            break;
        case DepositState::WELD_TRIGGERING_STOP:
            if (!_weldingController.isPending()) {
                LOG_SYSTEM("Deposit: complete -> IDLE");
                _depositState = DepositState::IDLE;
                updateStatus(now);
            }
    }
}

void SystemController::handleInputs(uint32_t now) {
    bool isLeft = false, isRight = false, isTrigger = false;

    // --- Serial HMI simulation (test hack) ---
    if (Serial.available()) {
        char cmd = Serial.read();

        if (cmd == 'S' || cmd == 's') {
            Serial.println("[SERIAL] Enter moveStartOffsetMs and weldStopOffsetMs (ms):");
            Serial.setTimeout(10000);
            int32_t val1 = Serial.parseInt();
            int32_t val2 = Serial.parseInt();
            if (val1 >= 0 && val2 >= 0) {
                _moveStartOffsetMs = val1;
                _weldStopOffsetMs = val2;
                Serial.print("[SERIAL] Updated: moveStart=");
                Serial.print(_moveStartOffsetMs);
                Serial.print(" weldStop=");
                Serial.println(_weldStopOffsetMs);
            }
            while (Serial.available()) Serial.read();
            return;
        }

        isLeft = (cmd == 'L' || cmd == 'l');
        isRight = (cmd == 'R' || cmd == 'r');
        isTrigger = (cmd == 'T' || cmd == 't');
    }

    if (!isLeft && !isRight && !isTrigger) {
        isLeft = _hmi.isLeftPressed();
        isRight = _hmi.isRightPressed();
        isTrigger = _hmi.isTriggerPressed();
    } else {
        // Consume pending HMI events to prevent stale triggers
        _hmi.isLeftPressed();
        _hmi.isRightPressed();
        _hmi.isTriggerPressed();
    }
    // --- End serial HMI ---

    if (_depositState != DepositState::IDLE) {
        return;
    }
    if (isTrigger) {
        LOG_SYSTEM("Input: trigger pressed");
        deposit(now);
        return;
    }

    if (isLeft || isRight) {
        if (_axisController.getState() == AxisController::State::IDLE) {
            home(now, isLeft ? Direction::LEFT : Direction::RIGHT);
        }
        if (_axisController.getState() == AxisController::State::HOME) {
            if (isLeft) {
                home(now, Direction::LEFT);
            } else if (!isLeft) {
                home(now, Direction::RIGHT);
            }
        }
    }
}

void SystemController::updateStatus(uint32_t now) {
    if (_axisController.getState() == AxisController::State::HOME &&
        _depositState == DepositState::IDLE) {
        _hmi.setStatus(HMI::Status::READY);
    } else {
        _hmi.setStatus(HMI::Status::IDLE);
    }
}

void SystemController::home(uint32_t now, Direction direction) {
    LOG_SYSTEM("Homing");
    _axisController.move(now, direction);
}

void SystemController::deposit(uint32_t now) {
    if (_axisController.getState() != AxisController::State::HOME ||
        _depositState != DepositState::IDLE || _axisController.isPending() ||
        _weldingController.isPending()) {
        return;
    }
    LOG_SYSTEM("Deposit: started -> WELD_TRIGGERING_START");
    _weldingController.trigger(now);
    _depositPhaseStart = now;
    _hmi.setStatus(HMI::Status::RUNNING);
    _depositState = DepositState::WELD_TRIGGERING_START;
}
