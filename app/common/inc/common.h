
#ifndef COMMON_H
#define COMMON_H
#include "stm32l476xx.h"
#include <stdio.h>
#include "stdint.h"
#include "stdbool.h"
#include "printf.h"


#define FAULT_MAGIC  0xDEADBEEF
typedef struct
{
    uint32_t magic;   // == FAULT_MAGIC if fault captured

    uint32_t pc;
    uint32_t lr;
    uint32_t psr;
    uint32_t ipsr;
    uint32_t cfsr;
    uint32_t hfsr;
    uint32_t irqn;

} fault_info_t;

#include "queue.h"
void delay();
typedef void(*task_fn)();
void mycopy(uint8_t *dest, uint8_t *src, int n);
#endif
