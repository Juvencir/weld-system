#include "CarriageController.h"

void (*CarriageController::onStall)() = CarriageController::stop;

BypassPin CarriageController::startPin(START_PIN, onStall);
BypassPin CarriageController::stopPin(STOP_PIN, onStall);
BypassPin CarriageController::dirPin(DIR_PIN, onStall);
enum CarriageState CarriageController::state = IDLE;
unsigned long CarriageController::lastStateChangeTime = 0;

void CarriageController::begin() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  startPin.begin();
  stopPin.begin();
  dirPin.begin();
  state = IDLE;
}

void CarriageController::update() {
  startPin.update();
  stopPin.update();
  dirPin.update();

  if (state != IDLE && state != MOVING) {
    unsigned long currentTime = millis();
    if (currentTime - lastStateChangeTime >= STATE_CHANGE_DELAY) {
      switch (state) {
        case STARTING:
          startPin.setState(HIGH);
          state = MOVING;
          break;
        case STOPPING:
          stopPin.setState(HIGH);
          state = IDLE;
          break;
        case CHANGING_DIRECTION:
          setState(STARTING);
          break;
      }
    }
  }
}

void CarriageController::setState(CarriageState newState) {
  state = newState;
  lastStateChangeTime = millis();
}

void CarriageController::stop() {
  digitalWrite(RELAY_PIN, HIGH);

  startPin.end();
  stopPin.end();
  dirPin.end();
}

bool CarriageController::isStalled() {
  return startPin.isStalled() || stopPin.isStalled() || dirPin.isStalled();
}

void CarriageController::startMovement(bool direction) {
  if (isStalled()) return;
  if (state != IDLE) return;

  dirPin.setState(direction);
  setState(CHANGING_DIRECTION);
}

void CarriageController::stopMovement() {
  if (isStalled()) return;
  if (state != MOVING) return;

  stopPin.setState(LOW);
  setState(STOPPING);
}