#pragma once

#include <Arduino.h>

constexpr uint32_t BYPASS_PIN_STALL_THRESHOLD = 100;

class BypassPin {
 public:
  using StallHandler = void (*)(BypassPin&);

  explicit BypassPin(uint8_t pin, StallHandler callback = nullptr)
      : pin(pin), onStall(callback) {};
  void begin();
  void update();
  bool isStalled() const { return stalled; }
  bool getState() const { return state; }
  void setState(bool state);
  void end();

 private:
  uint8_t pin;
  bool state = HIGH;
  bool stalled = true;
  uint32_t lastHighTime = 0;
  StallHandler onStall = nullptr;
  void checkStall();
};
