#include "HumanInterface.h"

Button HumanInterface::topButton(TOP_BUTTON_PIN);
Button HumanInterface::midButton(MID_BUTTON_PIN);
Button HumanInterface::botButton(BOT_BUTTON_PIN);
Button* HumanInterface::_activeButton = nullptr;
bool HumanInterface::welderState = false;

void HumanInterface::begin() {
  pinMode(LED_PIN, OUTPUT);
  setWelderState(false);
  topButton.setCallback(
      [](ButtonEvent event) { buttonEventHandler(event, topButton); });
  midButton.setCallback(
      [](ButtonEvent event) { buttonEventHandler(event, midButton); });
  botButton.setCallback(
      [](ButtonEvent event) { buttonEventHandler(event, botButton); });
  topButton.begin();
  midButton.begin();
  botButton.begin();
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

  if (&button == &midButton) {
    if (event == BUTTON_EVENT_LONG_PRESS) {
      // Inicia a solda
    }
    if (event == BUTTON_EVENT_RELEASE) {
      if (midButton.isLongPressed()) {
        // Para a solda
      } else {
        toggleWelderState();
      }
    }
  }

  if (&button == &topButton) {
    if (event == BUTTON_EVENT_PRESS) {
      // Move para a esquerda
    }
    if (event == BUTTON_EVENT_RELEASE) {
      // Para o movimento
    }
  }
  if (&button == &botButton) {
    if (event == BUTTON_EVENT_PRESS) {
      // Move para a direita
    }
    if (event == BUTTON_EVENT_RELEASE) {
      // Para o movimento
    }
  }

  if (event == BUTTON_EVENT_RELEASE) {
    _activeButton = nullptr;
  }
}

void HumanInterface::update() {
  topButton.update();
  midButton.update();
  botButton.update();
}

void HumanInterface::setWelderState(bool state) {
  welderState = state;
  digitalWrite(LED_PIN, welderState ? HIGH : LOW);
}

void HumanInterface::toggleWelderState() { setWelderState(!welderState); }

bool HumanInterface::getWelderState() { return welderState; }