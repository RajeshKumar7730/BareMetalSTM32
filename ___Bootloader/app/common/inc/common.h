
#ifndef COMMON_H
#define COMMON_H
#include "stm32l476xx.h"
#include <stdio.h>
#include "stdint.h"
#include "stdbool.h"
#include "printf.h"




#include "queue.h"
void delay();
typedef void(*task_fn)();
void mycopy(uint8_t *dest, uint8_t *src, int n);
#endif
