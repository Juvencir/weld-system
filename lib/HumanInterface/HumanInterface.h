#pragma once

#include <Arduino.h>

#include "Button.h"

#define TOP_BUTTON_PIN 2
#define MID_BUTTON_PIN 3
#define BOT_BUTTON_PIN 4
#define LED_PIN 13

class HumanInterface {
 private:
  static Button* _activeButton;
  static Button topButton;
  static Button midButton;
  static Button botButton;
  static bool welderState;
  static void buttonEventHandler(ButtonEvent event, Button& button);
 
 public:
  static void begin();
  static void update();
  static void setWelderState(bool state);
  static void toggleWelderState();
  static bool getWelderState();
};