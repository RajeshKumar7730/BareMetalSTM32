
#include "common.h"
#define CRC8_POLYNOMIAL 0x31

uint8_t calculate_crc8(const uint8_t *data, uint32_t length);