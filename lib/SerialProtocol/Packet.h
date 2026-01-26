#pragma once

#include <Arduino.h>
#include <algorithm>

constexpr uint8_t START_BYTE = 0x7E;

enum class MsgType : uint8_t { TELEMETRY = 0x01 };

class Packet {
   public:
    MsgType type;
    uint8_t length;
    uint8_t payload[UINT8_MAX];

    uint32_t checksum() const;
    size_t serialize(uint8_t* buffer, size_t bufferSize) const;
};

class PacketDecoder {
   public:
    PacketDecoder();

    bool feed(uint8_t byte, Packet& out);

    void reset();

   private:
    enum State { WAIT_START, READ_TYPE, READ_LENGTH, READ_PAYLOAD, READ_CHECKSUM };

    State _state;
    uint8_t _bytesReadCount;
    uint8_t _checksumBuf[4];
};