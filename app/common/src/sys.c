

#include "common.h"
#include "sys.h"


static uint32_t sys_ticks = 0;
void sys_clock_init(){
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;


    while( (RCC->CFGR & (RCC_CFGR_SWS)) != RCC_CFGR_SWS_HSI);
    RCC->CFGR &= ~(RCC_CFGR_HPRE);
}
static task_fn upd_task;
void sys_tick_init(uint32_t frequency, task_fn task){

    SysTick->LOAD = (SYS_CLOCK_FREQUENCY/frequency) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    upd_task = task;
    /*Enable the tick Interrupt*/
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}


// void systick_handler(void)
// {
//     sys_ticks++;
//     // upd_task();
//     xPortSysTickHandler();
// }


// void svcall_handler()
// {
//     vPortSVCHandler();
// }


// void pendsv_handler()
// {
//     xPortPendSVHandler();
// }


uint32_t sys_tick_get_current_ticks()
{
    return sys_ticks;
}

void sys_reset()
{
    NVIC_SystemReset();
}