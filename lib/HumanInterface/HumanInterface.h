#pragma once

#include <Arduino.h>

#include "Button.h"

constexpr uint8_t TOP_BUTTON_PIN = 2;
constexpr uint8_t MID_BUTTON_PIN = 3;
constexpr uint8_t BOT_BUTTON_PIN = 4;
constexpr uint8_t LED_PIN = 13;

class HumanInterface {
   public:
    static void begin();
    static void update();
    static void setWelderState(bool state);
    static void toggleWelderState();
    static bool getWelderState();

   private:
    static void buttonEventHandler(Button& button, Button::ButtonEvent event);
    static inline Button* _activeButton = nullptr;
    static inline Button topButton = Button(TOP_BUTTON_PIN, buttonEventHandler);
    static inline Button midButton = Button(MID_BUTTON_PIN, buttonEventHandler);
    static inline Button botButton = Button(BOT_BUTTON_PIN, buttonEventHandler);
    static inline bool welderState = false;
};