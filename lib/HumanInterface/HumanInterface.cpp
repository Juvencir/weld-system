#include "HumanInterface.h"

Button HumanInterface::leftButton(LEFT_BUTTON_PIN);
Button HumanInterface::middleButton(MIDDLE_BUTTON_PIN);
Button HumanInterface::rightButton(RIGHT_BUTTON_PIN);
Button* HumanInterface::_activeButton = nullptr;
bool HumanInterface::welderState = false;

void HumanInterface::begin() {
  pinMode(LED_PIN, OUTPUT);
  setWelderState(false);
  leftButton.setCallback(
      [](ButtonEvent event) { buttonEventHandler(event, leftButton); });
  middleButton.setCallback(
      [](ButtonEvent event) { buttonEventHandler(event, middleButton); });
  rightButton.setCallback(
      [](ButtonEvent event) { buttonEventHandler(event, rightButton); });
  leftButton.begin();
  middleButton.begin();
  rightButton.begin();
}

void HumanInterface::buttonEventHandler(ButtonEvent event, Button& button) {
  if (_activeButton != nullptr && _activeButton != &button) {
    return;
  }

  if (_activeButton == nullptr) {
    if (event == BUTTON_EVENT_PRESS) {
      _activeButton = &button;
    } else {
      return;
    }
  }

  if (&button == &middleButton) {
    if (event == BUTTON_EVENT_LONG_PRESS) {
      // Inicia a solda
    }
    if (event == BUTTON_EVENT_RELEASE) {
      if (middleButton.isLongPressed()) {
        // Para a solda
      } else {
        toggleWelderState();
      }
    }
  }

  if (event == BUTTON_EVENT_RELEASE) {
    _activeButton = nullptr;
  }
}

void HumanInterface::update() {
  leftButton.update();
  middleButton.update();
  rightButton.update();
}

void HumanInterface::setWelderState(bool state) {
  welderState = state;
  digitalWrite(LED_PIN, welderState ? HIGH : LOW);
}

void HumanInterface::toggleWelderState() { setWelderState(!welderState); }

bool HumanInterface::getWelderState() { return welderState; }