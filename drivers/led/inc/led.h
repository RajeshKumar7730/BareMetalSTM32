

#ifndef LED_H
#define LED_H

typedef enum{
    LED_ON,
    LED_OFF,
    LED_TOGGLE
}led_status_t;


void led_init();
void led_task();
void ledOperate(led_status_t set_state);
led_status_t getLedState();

#endif


