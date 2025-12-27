


#ifndef RB_H
#define RB_H
#include "common.h"

typedef struct{
    uint32_t head;
    uint32_t tail;
    uint8_t *buffer;
    uint32_t size;

}rb_t;


int rb_put(rb_t *rb,uint8_t data);
int rb_get(rb_t *rb,uint8_t *data);
void rb_init(rb_t *rb,uint8_t *buffer,uint8_t size );
bool rb_is_data_available(rb_t *rb);
#endif