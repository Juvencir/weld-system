#include "HumanInterface.h"

Button HumanInterface::leftButton(LEFT_BUTTON_PIN);
Button HumanInterface::middleButton(MIDDLE_BUTTON_PIN);
Button HumanInterface::rightButton(RIGHT_BUTTON_PIN);

bool HumanInterface::welderState = false;

void HumanInterface::begin() {
  pinMode(LED_PIN, OUTPUT);
  setWelderState(false);
  middleButton.setCallbacks(
      nullptr, []() { HumanInterface::middleButtonReleaseHandler(); }, []() { HumanInterface::middleButtonLongPressHandler(); });
  leftButton.begin();
  middleButton.begin();
  rightButton.begin();
}

void HumanInterface::middleButtonReleaseHandler() {
  if(middleButton.isLongPressed()) {
    // Para a solda
    return;
  }
  if (!leftButton.isPressed() && !rightButton.isPressed()) {
    toggleWelderState();
  }
}
void HumanInterface::middleButtonLongPressHandler() {
  if (!leftButton.isPressed() && !rightButton.isPressed()) {
    // Inicia a solda sem mover o carrinho
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