#include "EndStop.h"

void EndStop::begin() {
    pinMode(_pin, INPUT);
    _instance = this;

    attachInterrupt(digitalPinToInterrupt(_pin), isrHandler, CHANGE);

    _state = digitalRead(_pin);
}
void EndStop::isrHandler() { _instance->isrChange(); }

void EndStop::isrChange() { _state = digitalRead(_pin); }
