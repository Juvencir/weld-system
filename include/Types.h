#pragma once

#include <cstdint>

/**
 * @file Types.h
 * @brief Definições de tipos de dados globais e enumerações do sistema.
 */

/**
 * @enum Direction
 * @brief Direções possíveis para o movimento do eixo linear.
 */
enum class Direction : uint8_t {
    LEFT,  ///< Movimento no sentido esquerdo (LOW no pino de direção)
    RIGHT  ///< Movimento no sentido direito (HIGH no pino de direção)
};