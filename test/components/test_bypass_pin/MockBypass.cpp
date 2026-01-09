#include <Arduino.h>

u_int8_t mock_switch_pin;
u_int8_t mock_input_pin;

void setupMockBypassPin(uint8_t mockSwitchPin, uint8_t mockInputPin) {
  mock_switch_pin = mockSwitchPin;
  mock_input_pin = mockInputPin;
  pinMode(mock_input_pin, INPUT_PULLUP);
  pinMode(mock_switch_pin, OUTPUT_OPEN_DRAIN);
  digitalWrite(mock_switch_pin, HIGH);
}

void setMockSwitchState(bool state) { digitalWrite(mock_switch_pin, state); }

bool readMockInput() { return digitalRead(mock_input_pin); }