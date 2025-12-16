#pragma once

#include <Arduino.h>

#include "BypassPin.h"

#define RELAY_PIN 4
#define START_PIN 5
#define STOP_PIN 6
#define DIR_PIN 7

#define STATE_CHANGE_DELAY 20

enum CarriageState { IDLE, STARTING, STOPPING, CHANGING_DIRECTION, MOVING };

class CarriageController {
 private:
  static BypassPin startPin;
  static BypassPin stopPin;
  static BypassPin dirPin;
  static enum CarriageState state;
  static void setState(CarriageState newState);
  static unsigned long lastStateChangeTime;
  static void (*onStall)();

 public:
  static void begin();
  static void update();
  static void stop();
  static bool isStalled();
  static void startMovement(bool direction);
  static void stopMovement();
};