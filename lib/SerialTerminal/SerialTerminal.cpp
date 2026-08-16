#include "SerialTerminal.h"

// ─── Helpers estáticos para nomear estados em logs
// ───────────────────────────────

/** Retorna a representação em texto do estado do AxisController. */
static const char* axisStateName(AxisController::State s) {
    switch (s) {
        case AxisController::State::IDLE: return "IDLE";
        case AxisController::State::STARTING: return "STARTING";
        case AxisController::State::MOVING: return "MOVING";
        case AxisController::State::STOPPING: return "STOPPING";
        case AxisController::State::ABORTING: return "ABORTING";
        case AxisController::State::HOME: return "HOME";
        default: return "?";
    }
}

static const char* depositStateName(SystemController::DepositState s) {
    switch (s) {
        case SystemController::DepositState::IDLE: return "IDLE";
        case SystemController::DepositState::TRIGGERING_START: return "TRIGGERING_START";
        case SystemController::DepositState::MOVING: return "MOVING";
        case SystemController::DepositState::WAITING_STOP: return "WAITING_STOP";
        case SystemController::DepositState::TRIGGERING_STOP: return "TRIGGERING_STOP";
        default: return "?";
    }
}

/** Retorna a representação em texto da direção de movimento. */
static const char* dirName(Direction d) {
    return (d == Direction::LEFT) ? "LEFT" : "RIGHT";
}

SerialTerminal::SerialTerminal(SystemController& systemController, Stream& stream)
    : _systemController(systemController), _stream(stream) {}

void SerialTerminal::begin() {
    _stream.println();
    _stream.println("========================================");
    _stream.println("  WAAM System Controller - Serial Mode");
    _stream.println("========================================");
    printHelp();
    printStatus();
}

void SerialTerminal::update(uint32_t now) {
    handleSerialInput(now);

    // Log de mudanças de estado do eixo
    AxisController::State axisState = _systemController.getAxisController().getState();
    if (axisState != _prevAxisState) {
        _stream.print("[STATUS] Axis: ");
        _stream.print(axisStateName(_prevAxisState));
        _stream.print(" -> ");
        _stream.print(axisStateName(axisState));
        _stream.print("  |  Dir: ");
        _stream.print(dirName(_systemController.getAxisController().getDirection()));
        _stream.print("  |  Deposit: ");
        _stream.println(depositStateName(_systemController.getDepositState()));
        _prevAxisState = axisState;
    }

    // Log de mudanças de estado do depósito
    SystemController::DepositState depositState = _systemController.getDepositState();
    if (depositState != _prevDepositState) {
        _stream.print("[STATUS] Deposit: ");
        _stream.print(depositStateName(_prevDepositState));
        _stream.print(" -> ");
        _stream.print(depositStateName(depositState));
        _stream.print("  |  Axis: ");
        _stream.print(axisStateName(axisState));
        _stream.print("  |  Dir: ");
        _stream.println(dirName(_systemController.getAxisController().getDirection()));
        _prevDepositState = depositState;
    }
}

void SerialTerminal::handleSerialInput(uint32_t now) {
    if (!_stream.available()) return;

    char cmd = _stream.read();

    // Ignora whitespace / newline
    if (cmd == '\r' || cmd == '\n' || cmd == ' ') return;

    switch (cmd) {
        case 'L':
        case 'l': {
            _stream.println("[CMD] LEFT");
            if (_systemController.getDepositState() != SystemController::DepositState::IDLE) {
                _stream.println("[CMD] IGNORED - deposit in progress");
                return;
            }
            if (!_systemController.home(now, Direction::LEFT)) {
                _stream.println("[CMD] IGNORED - system busy");
            }
            break;
        }
        case 'R':
        case 'r': {
            _stream.println("[CMD] RIGHT");
            if (_systemController.getDepositState() != SystemController::DepositState::IDLE) {
                _stream.println("[CMD] IGNORED - deposit in progress");
                return;
            }
            if (!_systemController.home(now, Direction::RIGHT)) {
                _stream.println("[CMD] IGNORED - system busy");
            }
            break;
        }
        case 'T':
        case 't': {
            _stream.println("[CMD] TRIGGER / START DEPOSIT");
            if (_systemController.getDepositState() != SystemController::DepositState::IDLE) {
                _stream.println("[CMD] IGNORED - already in deposit");
                return;
            }
            if (!_systemController.deposit(now)) {
                _stream.println("[CMD] IGNORED - cannot start deposit");
            }
            break;
        }
        case 'S':
        case 's': {
            _stream.println("[CMD] CONFIGURE OFFSETS");
            _stream.print("  [Info] Pulse Durations -> Carriage: ");
            _stream.print(_systemController.getCarriagePulseDurationMs());
            _stream.print(" ms | Weld: ");
            _stream.print(_systemController.getWeldPulseDurationMs());
            _stream.println(" ms");

            _stream.print("  Enter moveStartOffsetMs (current=");
            _stream.print(_systemController.getMoveStartOffsetMs());
            _stream.print(" ms, min=");
            _stream.print(_systemController.getMinMoveStartOffsetMs());
            _stream.print(" ms): ");
            _stream.setTimeout(15000);
            int32_t val1 = _stream.parseInt();
            _stream.println(val1);
            if (val1 < _systemController.getMinMoveStartOffsetMs()) {
                _stream.print("  [Notice] moveStartOffsetMs clamped to min: ");
                _stream.print(_systemController.getMinMoveStartOffsetMs());
                _stream.println(" ms");
                val1 = _systemController.getMinMoveStartOffsetMs();
            }

            _stream.print("  Enter weldStopOffsetMs  (current=");
            _stream.print(_systemController.getWeldStopOffsetMs());
            _stream.print(" ms, immediate at endstop: ");
            _stream.print(_systemController.getImmediateWeldStopOffsetMs());
            _stream.print(" ms, sync: 0 ms): ");
            int32_t val2 = _stream.parseInt();
            _stream.println(val2);

            _systemController.setOffsets(val1, val2);
            _stream.println("[CMD] Offsets Updated:");
            _stream.print("  -> moveStartOffsetMs: ");
            _stream.print(_systemController.getMoveStartOffsetMs());
            _stream.print(" ms (arc dwells for ");
            _stream.print(_systemController.getMoveStartOffsetMs());
            _stream.print(" ms; axis trigger after ");
            _stream.print(_systemController.moveStartDelayMs());
            _stream.println(" ms)");

            _stream.print("  -> weldStopOffsetMs:  ");
            _stream.print(_systemController.getWeldStopOffsetMs());
            _stream.print(" ms (weld stop trigger after ");
            _stream.print(_systemController.weldStopDelayMs());
            _stream.println(" ms from endstop sensor)");

            // Limpa buffer residual
            while (_stream.available()) _stream.read();
            break;
        }
        case 'I':
        case 'i': {
            _stream.println("[CMD] STATUS");
            printStatus();
            break;
        }
        case 'H':
        case 'h':
        case '?': {
            _stream.println("[CMD] HELP");
            printHelp();
            break;
        }
        default: {
            _stream.print("[CMD] Unknown: '");
            _stream.print(cmd);
            _stream.println("' — type H for help");
            break;
        }
    }
}

void SerialTerminal::printHelp() {
    _stream.println();
    _stream.println("Commands:");
    _stream.println("  L/l  - Move LEFT  (home)");
    _stream.println("  R/r  - Move RIGHT (home)");
    _stream.println("  T/t  - Trigger deposit (start cycle)");
    _stream.println("  S/s  - Set moveStartOffsetMs & weldStopOffsetMs");
    _stream.println("  I/i  - Show current status");
    _stream.println("  H/h/?- This help");
    _stream.println();
}

void SerialTerminal::printStatus() {
    _stream.println();
    _stream.println("─── System Status ───");
    _stream.print("  Axis state:        ");
    _stream.println(axisStateName(_systemController.getAxisController().getState()));
    _stream.print("  Axis direction:    ");
    _stream.println(dirName(_systemController.getAxisController().getDirection()));
    _stream.print("  Axis pending:      ");
    _stream.println(_systemController.getAxisController().isPending() ? "YES" : "NO");
    _stream.print("  Deposit state:     ");
    _stream.println(depositStateName(_systemController.getDepositState()));
    _stream.print("  Weld pending:      ");
    _stream.println(_systemController.getWeldingController().isPending() ? "YES" : "NO");
    _stream.print("  Pulse durations:   Carriage=");
    _stream.print(_systemController.getCarriagePulseDurationMs());
    _stream.print(" ms, Weld=");
    _stream.print(_systemController.getWeldPulseDurationMs());
    _stream.println(" ms");
    _stream.print("  moveStartOffsetMs: ");
    _stream.print(_systemController.getMoveStartOffsetMs());
    _stream.print(" ms  [Arc dwell: ");
    _stream.print(_systemController.getMoveStartOffsetMs());
    _stream.print(" ms | Trigger delay: ");
    _stream.print(_systemController.moveStartDelayMs());
    _stream.print(" ms | Min: ");
    _stream.print(_systemController.getMinMoveStartOffsetMs());
    _stream.println(" ms]");
    _stream.print("  weldStopOffsetMs:  ");
    _stream.print(_systemController.getWeldStopOffsetMs());
    _stream.print(" ms  [Effective delay: ");
    _stream.print(_systemController.weldStopDelayMs());
    _stream.print(" ms | Immediate: ");
    _stream.print(_systemController.getImmediateWeldStopOffsetMs());
    _stream.println(" ms]");
    _stream.println("──────────────────────");
    _stream.println();
}
