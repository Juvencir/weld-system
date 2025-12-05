#include "BypassPin.h"

void BypassPin::begin() {
    state = HIGH;
    stalled = false;
    digitalWrite(pin, HIGH);
    pinMode(pin, OUTPUT_OPEN_DRAIN); // Possivel bug (substituir por chamada de baixo nivel)
    if(digitalRead(pin) == LOW) {
        end();
    }
}
void BypassPin::update() {
    if(stalled) return;

    if(state == HIGH && digitalRead(pin) == LOW) {
        end();
    }
}
void BypassPin::end() {
    stalled = true;
    pinMode(pin, INPUT);
    if(onStall) onStall();
}

void BypassPin::setState(bool state) {
    if(stalled) return;

    this->state = state;
    digitalWrite(pin, state ? HIGH : LOW);
}