#include "SystemController.h"

#include "Log.h"

// ─── Helpers estáticos para nomear estados ───────────────────────────────

static const char* axisStateName(AxisController::State s) {
    switch (s) {
        case AxisController::State::IDLE:     return "IDLE";
        case AxisController::State::STARTING: return "STARTING";
        case AxisController::State::MOVING:   return "MOVING";
        case AxisController::State::STOPPING: return "STOPPING";
        case AxisController::State::ABORTING: return "ABORTING";
        case AxisController::State::HOME:     return "HOME";
        default:                              return "?";
    }
}

static const char* depositStateName(SystemController::DepositState s) {
    switch (s) {
        case SystemController::DepositState::IDLE:                   return "IDLE";
        case SystemController::DepositState::WELD_TRIGGERING_START:  return "WELD_TRIGGERING_START";
        case SystemController::DepositState::MOVING:                 return "MOVING";
        case SystemController::DepositState::WELD_STOPPING:          return "WELD_STOPPING";
        case SystemController::DepositState::WELD_TRIGGERING_STOP:   return "WELD_TRIGGERING_STOP";
        default:                                                     return "?";
    }
}

static const char* dirName(Direction d) {
    return (d == Direction::LEFT) ? "LEFT" : "RIGHT";
}

// ─── Implementação ───────────────────────────────────────────────────────

void SystemController::begin() {
    _axisController.begin();
    _weldingController.begin();

    Serial.println();
    Serial.println("========================================");
    Serial.println("  WAAM System Controller - Serial Mode");
    Serial.println("========================================");
    printHelp();
    printStatus();
}

void SystemController::update(uint32_t now) {
    _axisController.update(now);
    _weldingController.update(now);

    handleSerialInput(now);
    handleDeposit(now);

    // Log de mudanças de estado do eixo (sempre)
    AxisController::State axisState = _axisController.getState();
    if (axisState != _prevAxisState) {
        Serial.print("[STATUS] Axis: ");
        Serial.print(axisStateName(_prevAxisState));
        Serial.print(" -> ");
        Serial.print(axisStateName(axisState));
        Serial.print("  |  Dir: ");
        Serial.print(dirName(_axisController.getDirection()));
        Serial.print("  |  Deposit: ");
        Serial.println(depositStateName(_depositState));
        _prevAxisState = axisState;
    }

    // Log de mudanças de estado do depósito
    if (_depositState != _prevDepositState) {
        Serial.print("[STATUS] Deposit: ");
        Serial.print(depositStateName(_prevDepositState));
        Serial.print(" -> ");
        Serial.print(depositStateName(_depositState));
        Serial.print("  |  Axis: ");
        Serial.print(axisStateName(axisState));
        Serial.print("  |  Dir: ");
        Serial.println(dirName(_axisController.getDirection()));
        _prevDepositState = _depositState;
    }
}

// ─── Máquina de estados do depósito ──────────────────────────────────────

void SystemController::handleDeposit(uint32_t now) {
    switch (_depositState) {
        case DepositState::IDLE:
            break;
        case DepositState::WELD_TRIGGERING_START:
            if (now - _depositPhaseStart >= moveStartDelayMs()) {
                Direction dir = (_axisController.getDirection() == Direction::LEFT)
                                    ? Direction::RIGHT
                                    : Direction::LEFT;

                LOG_SYSTEM("Deposit: weld started -> moving axis");
                _axisController.move(now, dir);
                _depositState = DepositState::MOVING;
            }
            break;
        case DepositState::MOVING:
            if (_axisController.getState() != AxisController::State::MOVING &&
                _axisController.getState() != AxisController::State::STARTING) {
                LOG_SYSTEM("Deposit: axis stopped -> stopping weld");
                _depositPhaseStart = now;
                _depositState = DepositState::WELD_STOPPING;
            }
            break;
        case DepositState::WELD_STOPPING:
            if (now - _depositPhaseStart >= weldStopDelayMs()) {
                LOG_SYSTEM("Deposit: triggering weld stop");
                _weldingController.trigger(now);
                _depositState = DepositState::WELD_TRIGGERING_STOP;
            }
            break;
        case DepositState::WELD_TRIGGERING_STOP:
            if (!_weldingController.isPending()) {
                LOG_SYSTEM("Deposit: complete -> IDLE");
                _depositState = DepositState::IDLE;
            }
    }
}

// ─── Comandos Seriais ────────────────────────────────────────────────────

void SystemController::handleSerialInput(uint32_t now) {
    if (!Serial.available()) return;

    char cmd = Serial.read();

    // Ignora whitespace / newline
    if (cmd == '\r' || cmd == '\n' || cmd == ' ') return;

    switch (cmd) {
        case 'L': case 'l': {
            Serial.println("[CMD] LEFT");
            if (_depositState != DepositState::IDLE) {
                Serial.println("[CMD] IGNORED - deposit in progress");
                return;
            }
            if (_axisController.getState() == AxisController::State::IDLE ||
                _axisController.getState() == AxisController::State::HOME) {
                home(now, Direction::LEFT);
            }
            break;
        }
        case 'R': case 'r': {
            Serial.println("[CMD] RIGHT");
            if (_depositState != DepositState::IDLE) {
                Serial.println("[CMD] IGNORED - deposit in progress");
                return;
            }
            if (_axisController.getState() == AxisController::State::IDLE ||
                _axisController.getState() == AxisController::State::HOME) {
                home(now, Direction::RIGHT);
            }
            break;
        }
        case 'T': case 't': {
            Serial.println("[CMD] TRIGGER / START DEPOSIT");
            if (_depositState != DepositState::IDLE) {
                Serial.println("[CMD] IGNORED - already in deposit");
                return;
            }
            deposit(now);
            break;
        }
        case 'S': case 's': {
            Serial.println("[CMD] CONFIGURE OFFSETS");
            Serial.print("  Enter moveStartOffsetMs (current=");
            Serial.print(_moveStartOffsetMs);
            Serial.print("): ");
            Serial.setTimeout(15000);
            int32_t val1 = Serial.parseInt();
            if (val1 < 0) { Serial.println("  ABORTED"); break; }
            Serial.println(val1);

            Serial.print("  Enter weldStopOffsetMs  (current=");
            Serial.print(_weldStopOffsetMs);
            Serial.print("): ");
            int32_t val2 = Serial.parseInt();
            if (val2 < 0) { Serial.println("  ABORTED"); break; }
            Serial.println(val2);

            _moveStartOffsetMs = (uint32_t)val1;
            _weldStopOffsetMs  = (uint32_t)val2;
            Serial.print("[CMD] Updated: moveStartOffsetMs=");
            Serial.print(_moveStartOffsetMs);
            Serial.print(" ms, weldStopOffsetMs=");
            Serial.print(_weldStopOffsetMs);
            Serial.println(" ms");

            // Limpa buffer residual
            while (Serial.available()) Serial.read();
            break;
        }
        case 'I': case 'i': {
            Serial.println("[CMD] STATUS");
            printStatus();
            break;
        }
        case 'H': case 'h': case '?': {
            Serial.println("[CMD] HELP");
            printHelp();
            break;
        }
        default: {
            Serial.print("[CMD] Unknown: '");
            Serial.print(cmd);
            Serial.println("' — type H for help");
            break;
        }
    }
}

// ─── Help / Status ───────────────────────────────────────────────────────

void SystemController::printHelp() {
    Serial.println();
    Serial.println("Commands:");
    Serial.println("  L/l  - Move LEFT  (home)");
    Serial.println("  R/r  - Move RIGHT (home)");
    Serial.println("  T/t  - Trigger deposit (start cycle)");
    Serial.println("  S/s  - Set moveStartOffsetMs & weldStopOffsetMs");
    Serial.println("  I/i  - Show current status");
    Serial.println("  H/h/?- This help");
    Serial.println();
}

void SystemController::printStatus() {
    Serial.println();
    Serial.println("─── System Status ───");
    Serial.print("  Axis state:     ");
    Serial.println(axisStateName(_axisController.getState()));
    Serial.print("  Axis direction: ");
    Serial.println(dirName(_axisController.getDirection()));
    Serial.print("  Axis pending:   ");
    Serial.println(_axisController.isPending() ? "YES" : "NO");
    Serial.print("  Deposit state:  ");
    Serial.println(depositStateName(_depositState));
    Serial.print("  Weld pending:   ");
    Serial.println(_weldingController.isPending() ? "YES" : "NO");
    Serial.print("  moveStartOffsetMs: ");
    Serial.print(_moveStartOffsetMs);
    Serial.println(" ms");
    Serial.print("  weldStopOffsetMs:  ");
    Serial.print(_weldStopOffsetMs);
    Serial.println(" ms");
    Serial.println("──────────────────────");
    Serial.println();
}

// ─── Comandos de movimento / depósito ────────────────────────────────────

void SystemController::home(uint32_t now, Direction direction) {
    LOG_SYSTEM("Homing");
    _axisController.move(now, direction);
}

void SystemController::deposit(uint32_t now) {
    if (_axisController.getState() != AxisController::State::HOME ||
        _depositState != DepositState::IDLE || _axisController.isPending() ||
        _weldingController.isPending()) {
        Serial.print("[CMD] Cannot start deposit: axis=");
        Serial.print(axisStateName(_axisController.getState()));
        Serial.print(" deposit=");
        Serial.print(depositStateName(_depositState));
        Serial.print(" axisPending=");
        Serial.print(_axisController.isPending() ? "Y" : "N");
        Serial.print(" weldPending=");
        Serial.println(_weldingController.isPending() ? "Y" : "N");
        return;
    }
    LOG_SYSTEM("Deposit: started -> WELD_TRIGGERING_START");
    _weldingController.trigger(now);
    _depositPhaseStart = now;
    _depositState = DepositState::WELD_TRIGGERING_START;
}
