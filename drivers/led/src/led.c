

#include "led.h"
#include "common.h"


void led_init()
{
    /*Turning on the clock*/
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    /*Set GPIO mode to output*/
    GPIOA->MODER |= (GPIO_MODER_MODE5_0);
    GPIOA->MODER &= ~(GPIO_MODER_MODE5_1);

}




void ledOperate(led_status_t set_state)
{
    switch(set_state)
    {
        case LED_ON:
            GPIOA->ODR |= (GPIO_ODR_OD5);
            break;
        case LED_OFF:
            GPIOA->ODR &= ~(GPIO_ODR_OD5);
            break;
        case LED_TOGGLE:
            GPIOA->ODR ^= (GPIO_ODR_OD5);
            break;
        default:
            break;

    }
}


led_status_t getLedState()
{
    return (GPIOA->ODR & GPIO_ODR_OD5) ? LED_ON : LED_OFF;
}



void led_task()
{
    ledOperate(LED_TOGGLE);
}

