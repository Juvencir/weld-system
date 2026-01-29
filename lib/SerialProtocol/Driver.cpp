#include "Driver.h"

namespace {
constexpr uart_config_t UART_CONFIG = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_APB,
};
}

Driver::Driver(uart_port_t uartPort, int rxPin, int txPin)
    : _uartPort(uartPort), _rxPin(rxPin), _txPin(txPin), _rxTaskHandle(nullptr) {
    _rxQueue = xQueueCreate(PACKET_POOL_SIZE, sizeof(Packet*));
    _freeQueue = xQueueCreate(PACKET_POOL_SIZE, sizeof(Packet*));

    for (int i = 0; i < PACKET_POOL_SIZE; i++) {
        Packet* ptr = &_packetPool[i];
        xQueueSend(_freeQueue, &ptr, 0);
    }

    _txMutex = xSemaphoreCreateMutex();
}

Packet* Driver::getFromPool() {
    Packet* p = nullptr;
    if (xQueueReceive(_freeQueue, &p, 0) == pdTRUE) {
        return p;
    }
    return nullptr;
}

void Driver::returnToPool(Packet* p) {
    if (p) {
        xQueueSend(_freeQueue, &p, portMAX_DELAY);
    }
}

void Driver::begin() {
    if (_rxTaskHandle != nullptr) return;

    uart_driver_install(_uartPort, 2048, 0, 0, NULL, 0);
    uart_param_config(_uartPort, &UART_CONFIG);
    uart_set_pin(_uartPort, _txPin, _rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_set_rx_timeout(_uartPort, 20);

    xTaskCreate(rxTaskEntry, "DriverRxTask", 4096, this, 2, &_rxTaskHandle);
}

bool Driver::send(const Packet& packet) {
    if (!_txMutex) return false;

    uint8_t buffer[Packet::MAX_PACKET_SIZE];

    if (xSemaphoreTake(_txMutex, portMAX_DELAY) == pdTRUE) {
        size_t len = packet.serialize(buffer, sizeof(buffer));
        if (len > 0) {
            uart_write_bytes(_uartPort, buffer, len);
        }
        xSemaphoreGive(_txMutex);
        return len > 0;
    }
    return false;
}

bool Driver::receive(Packet& packet, TickType_t waitTicks) {
    Packet* ptrInPool = nullptr;
    if (xQueueReceive(_rxQueue, &ptrInPool, waitTicks) == pdTRUE) {
        packet = *ptrInPool;
        returnToPool(ptrInPool);
        return true;
    }
    return false;
}

void Driver::rxTaskLoop() {
    Packet* currentPacket = getFromPool();
    uint8_t data[300];

    while (true) {
        int length = uart_read_bytes(_uartPort, data, sizeof(data), portMAX_DELAY);

        if (length > 0) {
            for (int i = 0; i < length; i++) {
                if (currentPacket == nullptr) {
                    currentPacket = getFromPool();
                    if (!currentPacket) continue;
                }

                if (_decoder.feed(data[i], *currentPacket)) {
                    if (xQueueSend(_rxQueue, &currentPacket, 0) == pdTRUE) {
                        currentPacket = getFromPool();
                        _decoder.reset();
                    } else {
                        _decoder.reset();
                    }
                }
            }
        }
    }
}