
#include "stdio.h"
#include "stdint.h"

void delay(){
    for(int i=0;i<1000000;i++);
}



void mycopy(uint8_t *dest, uint8_t *src, int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) {
        *d++ = *s++;
    }
}