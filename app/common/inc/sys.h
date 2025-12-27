
#ifndef SYS_H
#define SYS_H

#define ONE_M_HZ            (1000000)
#define HSI_CLOCK_FREQUENCY (16 * ONE_M_HZ)
#define SYS_CLOCK_FREQUENCY (HSI_CLOCK_FREQUENCY)
#define AHB_PRESCALER       (1)
#define HCLK_FREQUENCY      (SYS_CLOCK_FREQUENCY / AHB_PRESCALER)
#include "common.h"
void sys_clock_init();
void sys_tick_init(uint32_t frequency ,task_fn task);
uint32_t sys_tick_get_current_ticks();
void sys_reset();



#endif /* SYS_H */