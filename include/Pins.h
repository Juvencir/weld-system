#pragma once

#include <Arduino.h>

/**
 * @file Pins.h
 * @brief Mapeamento de pinos do microcontrolador (STM32 BlackPill F401CE).
 *
 * Contém a definição das conexões dos módulos de entrada e saída
 * (controlador de eixo, gatilho de solda e sensor de fim de curso).
 */

/**
 * @namespace Pins
 * @brief Agrupa os pinos GPIO utilizados no projeto.
 */
namespace Pins {
// Controlador do Eixo / Carrinho (AxisController)
constexpr uint32_t CARRIAGE_START_PIN = PB8;  ///< Pino de pulso para iniciar movimento do eixo
constexpr uint32_t CARRIAGE_STOP_PIN  = PB9;  ///< Pino de pulso para parar movimento do eixo
constexpr uint32_t CARRIAGE_DIR_PIN   = PB7;  ///< Pino de controle de direção do eixo

// Controlador de Solda (WeldingController / PulseOutput)
constexpr uint32_t WELDING_PIN = PA1;  ///< Pino de pulso para gatilho da fonte de solda

// Sensor de Fim de Curso (EndStop)
static const uint32_t ENDSTOP_SENSOR_PIN = PA2;  ///< Pino do sensor de fim de curso (Efeito Hall)
}  // namespace Pins