#include "Packet.h"

#include <esp_crc.h>

void PacketReceiver::reset() {
    _state = WAIT_START;
    _bytesReadCount = 0;
    _idleTicksCount = 0;
}

bool PacketReceiver::update(Stream& stream, Packet& out) {
    if (stream.available() == 0) {
        if (_state != WAIT_START) {
            _idleTicksCount++;
            if (_idleTicksCount > _timeoutTicks) {
                reset();
            }
        }
        return false;
    }
    while (stream.available() > 0) {
        uint8_t byte = stream.read();
        _idleTicksCount = 0;

        switch (_state) {
            case WAIT_START:
                if (byte == START_BYTE) {
                    _state = READ_TYPE;
                }
                break;

            case READ_TYPE:
                out.type = (MsgType)byte;
                _state = READ_LENGTH;
                break;

            case READ_LENGTH:
                out.length = byte;
                _bytesReadCount = 0;
                if (out.length > 0) {
                    _state = READ_PAYLOAD;
                } else {
                    _state = READ_CHECKSUM;
                }

                break;

            case READ_PAYLOAD:
                out.payload[_bytesReadCount++] = byte;
                if (_bytesReadCount >= out.length) {
                    _bytesReadCount = 0;
                    _state = READ_CHECKSUM;
                }
                break;

            case READ_CHECKSUM:
                _checksumBuf[_bytesReadCount++] = byte;
                if (_bytesReadCount >= 4) {
                    uint32_t receivedCrc =
                        ((uint32_t)_checksumBuf[0] << 0) | ((uint32_t)_checksumBuf[1] << 8) |
                        ((uint32_t)_checksumBuf[2] << 16) | ((uint32_t)_checksumBuf[3] << 24);

                    uint32_t calcCrc = calculateChecksum(out);

                    reset();

                    if (receivedCrc == calcCrc) {
                        return true;
                    }
                }
                break;
        }
    }
}

uint32_t calculateChecksum(const Packet& packet) {
    uint32_t crc = 0x0;  // IEEE init = 0xFFFFFFFF but esp_crc32_le expects ~init so 0x0
    crc = esp_crc32_le(crc, (uint8_t*)&packet.type, 1);
    crc = esp_crc32_le(crc, &packet.length, 1);
    crc = esp_crc32_le(crc, packet.payload, packet.length);
    return ~crc;
}

bool writePacket(Stream& out, const Packet& packet) {
    size_t written = 0;
    size_t expected = 3 + packet.length + sizeof(uint32_t);

    if (out.availableForWrite() < expected) {
        return false;
    }

    written += out.write(START_BYTE);
    written += out.write((byte)packet.type);
    written += out.write((uint8_t)packet.length);
    written += out.write(packet.payload, packet.length);

    uint32_t checksum = calculateChecksum(packet);
    written += out.write((uint8_t*)&checksum, sizeof(checksum));

    return (written == expected);
}