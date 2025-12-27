


#include "rb.h"
#include "common.h"
#include "printf.h"

void rb_init(rb_t *rb,uint8_t *buffer,uint8_t size )
{
    rb->buffer = buffer;
    rb->head =0;
    rb->tail =0;
    rb->size = size;

}




int rb_put(rb_t *rb,uint8_t data)
{
    uint32_t next_tail = (rb->tail + 1)% rb->size;
    if(next_tail == rb->head)
    {
        printf("RB is full\n");
        return 0;
        
    }
    rb->buffer[rb->tail]=data;
    rb->tail = next_tail;
    return 1;
}


int rb_get(rb_t *rb,uint8_t *data)
{
    if(rb->head == rb->tail)
    {
        // printf("RB is empty\n");
        return 0;
    }

    *data=rb->buffer[rb->head];
    rb->head = (rb->head + 1) % rb->size;
    return 1;
}


bool rb_is_data_available(rb_t *rb)
{
    if(rb->head == rb->tail)
    {
        return false;
    }
    return true;
}
