#include "BypassPin.h"

void BypassPin::begin() {
  state = HIGH;
  stalled = false;
  digitalWrite(pin, HIGH);
  lastHighTime = millis();
  pinMode(pin, OUTPUT_OPEN_DRAIN);
}

void BypassPin::update() {
  if (!isStalled()) checkStall();
}

void BypassPin::checkStall() {
  if (isStalled()) return;

  if (state == HIGH && digitalRead(pin) == LOW) {
    unsigned long currentTime = millis();
    if (currentTime - lastHighTime > BYPASS_PIN_STALL_THRESHOLD) {
      end();
    }
  }
}

void BypassPin::end() {
  stalled = true;
  pinMode(pin, INPUT);
  if (onStall) onStall();
}

void BypassPin::setState(bool state) {
  if (isStalled()) return;

  this->state = state;
  digitalWrite(pin, state ? HIGH : LOW);
  if (state == HIGH) {
    lastHighTime = millis();
  }
}