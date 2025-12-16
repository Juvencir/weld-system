#pragma once

#include <Arduino.h>

#define BYPASS_PIN_STALL_THRESHOLD 100

class BypassPin {
 private:
  uint8_t pin;
  bool state = HIGH;
  bool stalled = true;
  unsigned long lastHighTime = 0;
  void (*onStall)() = nullptr;
  void checkStall();

 public:
  explicit BypassPin(uint8_t pin, void (*onStall)() = nullptr)
      : pin(pin), onStall(onStall) {};
  void begin();
  void update();
  bool isStalled() const { return stalled; }
  bool getState() const { return state; }
  void setState(bool state);
  void end();
};
