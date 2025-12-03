#pragma once

#include <Arduino.h>

#include "Button.h"

#define LEFT_BUTTON_PIN 2
#define MIDDLE_BUTTON_PIN 3
#define RIGHT_BUTTON_PIN 4
#define LED_PIN 13

class HumanInterface {
 private:
  static Button leftButton;
  static Button middleButton;
  static Button rightButton;
  static bool welderState;
  static void middleButtonReleaseHandler();
  static void middleButtonLongPressHandler();
 
 public:
  static void begin();
  static void update();
  static void setWelderState(bool state);
  static void toggleWelderState();
  static bool getWelderState();
};