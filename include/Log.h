#pragma once

#include <Arduino.h>

/**
 * @brief Macros de logging para depuração via Serial.
 *
 * Formato: [módulo] mensagem
 *
 * Para desabilitar todos os logs, comente a linha abaixo.
 * Cada módulo pode ser desabilitado individualmente.
 */
#define LOG_ENABLED

#ifdef LOG_ENABLED

#define LOG_MODULE_SYSTEM 1
#define LOG_MODULE_AXIS 1
#define LOG_MODULE_HMI 1
#define LOG_MODULE_WELD 1
#define LOG_MODULE_BUTTON 0  // muito verboso, desabilitado por padrão

#if LOG_MODULE_SYSTEM
#define LOG_SYSTEM(msg) Serial.println("[SYSTEM] " msg)
#else
#define LOG_SYSTEM(msg)
#endif

#if LOG_MODULE_AXIS
#define LOG_AXIS(msg) Serial.println("[AXIS] " msg)
#else
#define LOG_AXIS(msg)
#endif

#if LOG_MODULE_HMI
#define LOG_HMI(msg) Serial.println("[HMI] " msg)
#else
#define LOG_HMI(msg)
#endif

#if LOG_MODULE_WELD
#define LOG_WELD(msg) Serial.println("[WELD] " msg)
#else
#define LOG_WELD(msg)
#endif

#if LOG_MODULE_BUTTON
#define LOG_BUTTON(msg) Serial.println("[BUTTON] " msg)
#else
#define LOG_BUTTON(msg)
#endif

#else
// Logs completamente desabilitados
#define LOG_SYSTEM(msg)
#define LOG_AXIS(msg)
#define LOG_HMI(msg)
#define LOG_WELD(msg)
#define LOG_BUTTON(msg)
#endif
