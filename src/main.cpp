#include <Arduino.h>

#include "AxisController.h"
#include "Config.h"
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
 * e a interface desacoplada SerialTerminal utilizando os parâmetros de
 * Config.h.
 */

// Instanciação do sensor de fim de curso por efeito Hall
EndStop endStop(Pins::ENDSTOP_SENSOR_PIN);

// Instanciação do controlador de eixo com pinos de start, stop e direção
AxisController axisController(Pins::CARRIAGE_START_PIN, Pins::CARRIAGE_STOP_PIN,
                              Pins::CARRIAGE_DIR_PIN, endStop);

// Saída de pulso para acionamento do gatilho da solda
PulseOutput weldingController(Pins::WELDING_PIN, Config::WELD_PULSE_DURATION_MS, LOW, OUTPUT);

// Orquestrador do sistema coordenando eixo, solda e offsets de sincronização
SystemController systemController(axisController, weldingController,
                                  Config::DEFAULT_MOVE_START_OFFSET_MS,
                                  Config::DEFAULT_WELD_STOP_OFFSET_MS);

// Interface de terminal serial desacoplada conectada ao SystemController
SerialTerminal serialTerminal(systemController);

/**
 * Inicializa a comunicação serial e dispara o setup dos subsistemas.
 */
void setup() {
    Serial.begin(Config::SERIAL_BAUD_RATE);
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
