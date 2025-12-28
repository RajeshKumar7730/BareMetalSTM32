
#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>
typedef struct{
    uint8_t head;
    uint8_t tail;
    uint8_t length;
    uint8_t curr_usage;
    uint8_t elem_size;
    uint8_t *buffer;
}queue_t;
int queue_init(queue_t *queue,uint8_t *buffer,uint8_t length,uint8_t elem_size);
int queue_enqueue(queue_t *queue,uint8_t *data_ptr,uint8_t data_len);
int queue_dequeue(queue_t *queue,uint8_t *dest_ptr);
int queue_peek(queue_t *queue,uint8_t *dest_ptr);


#endif