#pragma once

#include <Arduino.h>

constexpr uint8_t START_BYTE = 0x7E;

enum class MsgType : uint8_t { TELEMETRY = 0x01 };

struct Packet {
    MsgType type;
    uint8_t length;
    uint8_t payload[UINT8_MAX];
};

class PacketReceiver {
   public:
    explicit PacketReceiver(uint8_t timeoutTicks = 5) : _timeoutTicks(timeoutTicks) { reset(); };

    bool update(Stream& stream, Packet& out);

    void reset();

   private:
    enum State { WAIT_START, READ_TYPE, READ_LENGTH, READ_PAYLOAD, READ_CHECKSUM };

    State _state;
    uint8_t _bytesReadCount;
    uint8_t _checksumBuf[4];

    uint8_t _idleTicksCount;
    const uint8_t _timeoutTicks;
};

uint32_t calculateChecksum(const Packet& packet);

bool writePacket(Stream& output, const Packet& packet);