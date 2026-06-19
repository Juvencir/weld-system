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
    bool isLeft = _hmi.isLeftPressed();
    bool isRight = _hmi.isRightPressed();
    bool isTrigger = _hmi.isTriggerPressed();
    
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
