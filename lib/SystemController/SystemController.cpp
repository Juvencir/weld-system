#include "SystemController.h"

SystemController::SystemController(HMI& hmi, AxisController& axisController)
    : _hmi(hmi), _axisController(axisController) {}

void SystemController::begin() {
    _hmi.begin();
    _axisController.begin();
}

void SystemController::update(uint32_t now) {
    _hmi.update(now);
    _axisController.update(now);

    if (_axisController.stateChanged()) {
        if (_axisController.getState() == AxisController::State::HOME) {
            _hmi.setStatus(HMI::Status::READY);
        } else {
            _hmi.setStatus(HMI::Status::IDLE);
        }
    }

    bool isLeft = _hmi.isLeftPressed();
    bool isRight = _hmi.isRightPressed();

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

void SystemController::home(uint32_t now, Direction direction) {
    _axisController.move(now, direction);
}
