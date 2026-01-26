#pragma once

#include <Arduino.h>
#include <driver/uart.h>
#include "Packet.h"

constexpr int PACKET_POOL_SIZE = 4;

class Driver {
   public:
    Driver(uart_port_t uartPort, int rxPin, int txPin);

    void begin();
    bool send(const Packet& packet);

    bool receive(Packet& packet, TickType_t waitTicks = portMAX_DELAY);

   private:
    static void rxTaskEntry(void* arg) {
        static_cast<Driver*>(arg)->rxTaskLoop();
    }

    void rxTaskLoop();
    Packet* getFromPool();
    void returnToPool(Packet* p);

    uart_port_t _uartPort;
    int _rxPin;
    int _txPin;

    PacketDecoder _decoder;

    Packet _packetPool[PACKET_POOL_SIZE];

    QueueHandle_t _rxQueue;
    QueueHandle_t _freeQueue;

    SemaphoreHandle_t _txMutex;
    TaskHandle_t _rxTaskHandle;
};