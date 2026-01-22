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
        if (event == Button::BUTTON_EVENT_PRESS) {
            _activeButton = &button;
        } else {
            return;
        }
    }

    if (&button == &midButton) {
        if (event == Button::BUTTON_EVENT_LONG_PRESS) {
            // Inicia a solda
        }
        if (event == Button::BUTTON_EVENT_RELEASE) {
            if (midButton.isLongPressed()) {
                // Para a solda
            } else {
                toggleWelderState();
            }
        }
    }

    if (&button == &topButton) {
        if (event == Button::BUTTON_EVENT_PRESS) {
            // Move para a esquerda
        }
        if (event == Button::BUTTON_EVENT_RELEASE) {
            // Para o movimento
        }
    }
    if (&button == &botButton) {
        if (event == Button::BUTTON_EVENT_PRESS) {
            // Move para a direita
        }
        if (event == Button::BUTTON_EVENT_RELEASE) {
            // Para o movimento
        }
    }

    if (event == Button::BUTTON_EVENT_RELEASE) {
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