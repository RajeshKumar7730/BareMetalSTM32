
#include "common.h"

void delay(){
    for(int i=0;i<100000;i++);
}


fw_meta_t *metadata = (fw_meta_t *)(SRAM_START_ADDRESSS);
void mycopy(uint8_t *dest, uint8_t *src, int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) {
        *d++ = *s++;
    }
}


uint32_t get_fw_version()
{
    return metadata->version;
}
uint32_t get_fw_crc()
{
    return metadata->crc;
}