#include <Arduino.h>
#include <unity.h>

#include "BypassPin.h"

#define TEST_PIN 4
BypassPin pin = BypassPin(TEST_PIN);

void setUp() { pin.update(); }

void tearDown() {}

void testBypassPinInitialState() {
  TEST_ASSERT_FALSE(pin.isStalled());
  TEST_ASSERT_EQUAL(HIGH, pin.getState());
}

void testBypassPinSetStateLow() {
  pin.setState(LOW);
  TEST_ASSERT_EQUAL(LOW, pin.getState());
  TEST_ASSERT_FALSE(pin.isStalled());
  delay(BYPASS_PIN_STALL_THRESHOLD + 100);
  pin.update();
  TEST_ASSERT_FALSE(pin.isStalled());
  TEST_ASSERT_EQUAL(pin.getState(), digitalRead(TEST_PIN));
}

void testBypassPinSetStateHigh() {
  pin.setState(HIGH);
  TEST_ASSERT_EQUAL(HIGH, pin.getState());
  TEST_ASSERT_FALSE(pin.isStalled());
  delay(BYPASS_PIN_STALL_THRESHOLD + 100);
  pin.update();
  TEST_ASSERT_FALSE(pin.isStalled());
  TEST_ASSERT_EQUAL(pin.getState(), digitalRead(TEST_PIN));
}

void testBypassPinStall() {
  pin.setState(HIGH);
  delay(BYPASS_PIN_STALL_THRESHOLD + 100);
  pin.update();
  TEST_ASSERT_FALSE(pin.isStalled());
  Serial.println(
      "Change the position of the direction switch to simulate a stall.");
  for (int i = 0; i < 200; i++) {
    pin.update();
    if (pin.isStalled()) {
      break;
    }
    delay(100);
  }
  TEST_ASSERT_TRUE(pin.isStalled());
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  pin.begin();
  Serial.println("Bypass pin test on Pin " + String(TEST_PIN) + "...");
  if (digitalRead(TEST_PIN) == LOW) {
    Serial.println(
        "Please change de position of the direction switch to continue.");
    while (digitalRead(TEST_PIN) == LOW) {
      delay(100);
    }
  }
  Serial.println("Starting tests...");
  UNITY_BEGIN();
  RUN_TEST(testBypassPinInitialState);
  RUN_TEST(testBypassPinSetStateLow);
  RUN_TEST(testBypassPinSetStateHigh);
  RUN_TEST(testBypassPinStall);
  UNITY_END();
}