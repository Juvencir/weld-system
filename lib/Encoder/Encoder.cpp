#include "Encoder.h"

void EncoderDriver::begin() {
    pinMode(_pinA, INPUT_PULLUP);
    pinMode(_pinB, INPUT_PULLUP);

    attachInterruptArg(digitalPinToInterrupt(_pinA), handleInterrupt, this, CHANGE);
}

void IRAM_ATTR EncoderDriver::handleInterrupt(void *arg) {
    EncoderDriver* instance = static_cast<EncoderDriver*>(arg);
    
    if(digitalRead(instance->_pinB) == HIGH){
        instance->_count++;
    }else{
        instance->_count--;
    }
}