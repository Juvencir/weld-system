#include <Arduino.h>

#include "AxisController.h"
#include "EndStop.h"
#include "Pins.h"
#include "SerialTerminal.h"
#include "SystemController.h"

/**
 * @file main.cpp
 * @brief Ponto de entrada do firmware do sistema de soldagem WAAM.
 *
 * Instancia os objetos controladores de hardware (sensor de fim de curso,
 * controlador de eixo e gatilho de solda), o orquestrador SystemController
 * e a interface desacoplada SerialTerminal.
 */

// Instanciação do sensor de fim de curso por efeito Hall
EndStop endStop(Pins::ENDSTOP_SENSOR_PIN);

// Instanciação do controlador de eixo com pinos de start, stop e direção
AxisController axisController(Pins::CARRIAGE_START_PIN, Pins::CARRIAGE_STOP_PIN,
                              Pins::CARRIAGE_DIR_PIN, endStop);

// Saída de pulso para acionamento do gatilho da solda (200ms de pulso ativo baixo)
PulseOutput weldingController(Pins::WELDING_PIN, 200, LOW, OUTPUT);

// Orquestrador do sistema coordenando eixo, solda e offsets de sincronização (500ms / 500ms)
SystemController systemController(axisController, weldingController, 500, 500);

// Interface de terminal serial desacoplada conectada ao SystemController
SerialTerminal serialTerminal(systemController);

/**
 * Inicializa a comunicação serial a 115200 baud e dispara o setup dos subsistemas.
 */
void setup() {
    Serial.begin(115200);
    systemController.begin();
    serialTerminal.begin();
}

/**
 * Loop principal de execução não-bloqueante.
 * Atualiza o orquestrador do sistema e a interface serial continuamente.
 */
void loop() {
    uint32_t now = millis();
    systemController.update(now);
    serialTerminal.update(now);
}
