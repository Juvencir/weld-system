#pragma once

#include <algorithm>

constexpr uint8_t START_BYTE = 0x7E;

enum class MsgType : uint8_t { TELEMETRY = 0x01 };

class Packet {
   public:
    static constexpr size_t HEADER_SIZE = 3;  // Start byte(1) + Type(1) + Length(1)
    static constexpr size_t MAX_PAYLOAD_SIZE = UINT8_MAX;
    static constexpr size_t CHECKSUM_SIZE = 4;
    static constexpr size_t MAX_PACKET_SIZE = HEADER_SIZE + MAX_PAYLOAD_SIZE + CHECKSUM_SIZE;

    MsgType type;
    uint8_t length;
    uint8_t payload[MAX_PAYLOAD_SIZE];

    uint32_t checksum() const;
    size_t serialize(uint8_t* buffer, size_t bufferSize) const;
};

class PacketDecoder {
   public:
    PacketDecoder();

    bool feed(uint8_t byte, Packet& out);

    void reset();

   private:
    enum class State { WAIT_START, READ_TYPE, READ_LENGTH, READ_PAYLOAD, READ_CHECKSUM };

    State _state;
    uint8_t _bytesReadCount;
    uint8_t _checksumBuf[4];
};