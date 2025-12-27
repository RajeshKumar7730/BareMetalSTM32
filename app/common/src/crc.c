
#include "crc.h"


uint8_t calculate_crc8(const uint8_t *data, uint32_t length) 
{
    uint8_t crc = 0x00; // Initial CRC value

    for (size_t i = 0; i < length; i++) {
        crc ^= data[i]; // XOR current byte with CRC

        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) { // If MSB is set
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}