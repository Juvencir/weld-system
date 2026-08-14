#pragma once

#include <Arduino.h>

#include <cstdint>

/**
 * @file Config.h
 * @brief Parâmetros globais de configuração e constantes do sistema WAAM.
 *
 * Centraliza as configurações de comunicação serial, temporizações de pulso do eixo,
 * duração do pulso de solda e offsets padrão de sincronização.
 */
namespace Config {
// Comunicação Serial
constexpr uint32_t SERIAL_BAUD_RATE = 115200;

// Controlador do Eixo (AxisController)
constexpr uint32_t CARRIAGE_PULSE_DURATION_MS = 450;

// Controlador da Solda (WeldingController / PulseOutput)
constexpr uint32_t WELD_PULSE_DURATION_MS  = 200;

// Offsets padrão de sincronização do ciclo de depósito (SystemController)
constexpr uint32_t DEFAULT_MOVE_START_OFFSET_MS = 500;
constexpr uint32_t DEFAULT_WELD_STOP_OFFSET_MS  = 500;
}  // namespace Config
