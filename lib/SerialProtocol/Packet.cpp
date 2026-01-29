#include "Packet.h"

#include <esp_crc.h>

PacketDecoder::PacketDecoder() { reset(); }

void PacketDecoder::reset() {
    _state = State::WAIT_START;
    _bytesReadCount = 0;
}

bool PacketDecoder::feed(uint8_t byte, Packet& out) {
    switch (_state) {
        case State::WAIT_START:
            if (byte == START_BYTE) {
                _state = State::READ_TYPE;
            }
            break;

        case State::READ_TYPE:
            out.type = (MsgType)byte;
            _state = State::READ_LENGTH;
            break;

        case State::READ_LENGTH:
            out.length = byte;
            _bytesReadCount = 0;
            if (out.length > 0) {
                _state = State::READ_PAYLOAD;
            } else {
                _state = State::READ_CHECKSUM;
            }

            break;

        case State::READ_PAYLOAD:
            out.payload[_bytesReadCount++] = byte;
            if (_bytesReadCount >= out.length) {
                _bytesReadCount = 0;
                _state = State::READ_CHECKSUM;
            }
            break;

        case State::READ_CHECKSUM:
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
    if (bufferSize < HEADER_SIZE + length + CHECKSUM_SIZE) {
        return 0;
    }
    size_t written = 0;

    buffer[written++] = START_BYTE;
    buffer[written++] = (uint8_t)type;
    buffer[written++] = length;

    if (length > 0) {
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