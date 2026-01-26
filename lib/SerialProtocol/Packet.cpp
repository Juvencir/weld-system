#include "Packet.h"

#include <esp_crc.h>

PacketDecoder::PacketDecoder() { reset(); }

void PacketDecoder::reset() {
    _state = WAIT_START;
    _bytesReadCount = 0;
}

bool PacketDecoder::feed(uint8_t byte, Packet& out) {
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

                uint32_t calcCrc = out.checksum();

                reset();

                if (receivedCrc == calcCrc) {
                    return true;
                }
            }
            break;
    }
    return false;
}

uint32_t Packet::checksum() const {
    uint32_t crc = 0x0;  // IEEE init = 0xFFFFFFFF but esp_crc32_le expects ~init so 0x0
    crc = esp_crc32_le(crc, (uint8_t*)&type, 1);
    crc = esp_crc32_le(crc, &length, 1);
    crc = esp_crc32_le(crc, payload, length);
    return ~crc;
}

size_t Packet::serialize(uint8_t* buffer, size_t bufferSize) const {
    if(bufferSize < 3 + length + 4) {
        return 0;
    }
    size_t written = 0;

    buffer[written++] = START_BYTE;
    buffer[written++] = (byte)type;
    buffer[written++] = length;

    if(length > 0) {
        std::copy(payload, payload + length, buffer + written);
        written += length;
    }

    uint32_t crc = checksum();

    buffer[written++] = (uint8_t)crc;
    buffer[written++] = (uint8_t)(crc >> 8);
    buffer[written++] = (uint8_t)(crc >> 16);
    buffer[written++] = (uint8_t)(crc >> 24);

    return written;
}