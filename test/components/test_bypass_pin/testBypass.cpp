#include <Arduino.h>
#include <unity.h>

#include "BypassPin.h"
#include "MockBypass.cpp"

#define BYPASS_PIN 4

#define MOCK_SWITCH_PIN 6
#define MOCK_INPUT_PIN 5

/*
 * TESTE DA LIB BYPASSPIN COM INTERAÇÃO DE HARDWARE SIMULADA:
 *
 * Testa funcionalidades da classe BypassPin utilizando hardware simulado pelo
 * próprio MCU por meio do MockBypass.
 *
 * [BYPASS_PIN] <---[ R1 ]---+---> [MOCK_SWITCH_PIN]
 *                           |
 *                           +---> [MOCK_INPUT_PIN]
 *
 * BYPASS_PIN: Pino controlado pela classe BypassPin que será testada.
 * R1: Resistor para proteção (Entre 330 ohms e 1K).
 * MOCK_SWITCH_PIN: Pino que simula o switch do tartilope.
 * MOCK_INPUT_PIN: Pino que simula entrada em pull up do MCU do tartilope.
 */

BypassPin pin = BypassPin(BYPASS_PIN);

void setUp() {
  setupMockBypassPin(MOCK_SWITCH_PIN, MOCK_INPUT_PIN);
  pin.begin();
  delay(100);
  pin.update();
}

void testBypassPinInitialState() {
  TEST_ASSERT_FALSE(pin.isStalled());
  TEST_ASSERT_EQUAL(HIGH, pin.getState());
  TEST_ASSERT_EQUAL(pin.getState(), readMockInput());
}

void testBypassPinSetStateTransitions() {
  pin.setState(LOW);
  delay(100);
  pin.update();
  TEST_ASSERT_EQUAL(LOW, pin.getState());
  TEST_ASSERT_FALSE(pin.isStalled());
  TEST_ASSERT_EQUAL(pin.getState(), readMockInput());

  pin.setState(HIGH);
  delay(100);
  pin.update();
  TEST_ASSERT_EQUAL(HIGH, pin.getState());
  TEST_ASSERT_FALSE(pin.isStalled());
  TEST_ASSERT_EQUAL(pin.getState(), readMockInput());
}

void testBypassPinStall() {
  TEST_ASSERT_FALSE(pin.isStalled());
  setMockSwitchState(LOW);
  for (int i = 0; i < 100; i++) {
    pin.update();
    if (pin.isStalled()) {
      break;
    }
    delay(10);
  }
  TEST_ASSERT_TRUE(pin.isStalled());
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(testBypassPinInitialState);
  RUN_TEST(testBypassPinSetStateTransitions);
  RUN_TEST(testBypassPinStall);
  UNITY_END();
}