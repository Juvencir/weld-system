#include "HumanInterface.h"

void HumanInterface::begin() {
    pinMode(LED_PIN, OUTPUT);
    setWelderState(false);
    topButton.begin();
    midButton.begin();
    botButton.begin();
}

void HumanInterface::buttonEventHandler(Button& button, Button::ButtonEvent event) {
    if (_activeButton != nullptr && _activeButton != &button) {
        return;
    }

    if (_activeButton == nullptr) {
        if (event == Button::ButtonEvent::PRESS) {
            _activeButton = &button;
        } else {
            return;
        }
    }

    if (&button == &midButton) {
        if (event == Button::ButtonEvent::LONG_PRESS) {
            // Inicia a solda
        }
        if (event == Button::ButtonEvent::RELEASE) {
            if (midButton.isLongPressed()) {
                // Para a solda
            } else {
                toggleWelderState();
            }
        }
    }

    if (&button == &topButton) {
        if (event == Button::ButtonEvent::PRESS) {
            // Move para a esquerda
        }
        if (event == Button::ButtonEvent::RELEASE) {
            // Para o movimento
        }
    }
    if (&button == &botButton) {
        if (event == Button::ButtonEvent::PRESS) {
            // Move para a direita
        }
        if (event == Button::ButtonEvent::RELEASE) {
            // Para o movimento
        }
    }

    if (event == Button::ButtonEvent::RELEASE) {
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
    digitalWrite(LED_PIN, welderState);
}

void HumanInterface::toggleWelderState() { setWelderState(!welderState); }

bool HumanInterface::getWelderState() { return welderState; }