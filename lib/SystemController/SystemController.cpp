#include "SystemController.h"

#include "Log.h"

SystemController::SystemController(AxisController& axisController, PulseOutput& weldingController,
                                   uint32_t moveStartOffsetMs, uint32_t weldStopOffsetMs)
    : _axisController(axisController),
      _weldingController(weldingController),
      _moveStartOffsetMs(moveStartOffsetMs),
      _weldStopOffsetMs(weldStopOffsetMs) {}

void SystemController::begin() {
    _axisController.begin();
    _weldingController.begin();
}

void SystemController::update(uint32_t now) {
    _axisController.update(now);
    _weldingController.update(now);

    handleDeposit(now);
}

void SystemController::setOffsets(uint32_t moveStartOffsetMs, uint32_t weldStopOffsetMs) {
    _moveStartOffsetMs = moveStartOffsetMs;
    _weldStopOffsetMs = weldStopOffsetMs;
}

bool SystemController::home(uint32_t now, Direction direction) {
    if (_depositState != DepositState::IDLE) {
        return false;
    }
    if (_axisController.getState() == AxisController::State::IDLE ||
        _axisController.getState() == AxisController::State::HOME) {
        LOG_SYSTEM("Homing");
        _axisController.move(now, direction);
        return true;
    }
    return false;
}

bool SystemController::deposit(uint32_t now) {
    if (_axisController.getState() != AxisController::State::HOME ||
        _depositState != DepositState::IDLE || _axisController.isPending() ||
        _weldingController.isPending()) {
        return false;
    }
    LOG_SYSTEM("Deposit: started -> WELD_TRIGGERING_START");
    _weldingController.trigger(now);
    _depositPhaseStart = now;
    _depositState = DepositState::WELD_TRIGGERING_START;
    return true;
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
            if (_axisController.getState() != AxisController::State::MOVING &&
                _axisController.getState() != AxisController::State::STARTING) {
                LOG_SYSTEM("Deposit: axis stopped -> stopping weld");
                _depositPhaseStart = now;
                _depositState = DepositState::WELD_STOPPING;
            }
            break;
        case DepositState::WELD_STOPPING:
            if (now - _depositPhaseStart >= weldStopDelayMs()) {
                LOG_SYSTEM("Deposit: triggering weld stop");
                _weldingController.trigger(now);
                _depositState = DepositState::WELD_TRIGGERING_STOP;
            }
            break;
        case DepositState::WELD_TRIGGERING_STOP:
            if (!_weldingController.isPending()) {
                LOG_SYSTEM("Deposit: complete -> IDLE");
                _depositState = DepositState::IDLE;
            }
            break;
    }
}
