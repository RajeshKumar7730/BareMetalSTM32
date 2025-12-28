
#include "queue.h"
#include "string.h"
#include <stdbool.h>
#include "common.h"
bool queue_isEmpty(queue_t *queue)
{
    return queue->head == queue->tail;

}


int queue_init(queue_t *queue,uint8_t *buffer,uint8_t length,uint8_t elem_size)
{
    if(queue == NULL || buffer == NULL) return -1;
    queue->head=0;
    queue->tail=0;
    queue->length = length;
    queue->buffer = buffer;
    queue->curr_usage = 0;
    queue->elem_size = elem_size; 
    return 0; 
}


int queue_enqueue(queue_t *queue,uint8_t *data_ptr,uint8_t data_len)
{

    
    uint8_t next_tail = (queue->tail + 1) % queue->length;
    if(next_tail == queue->head || data_len != queue->elem_size)
    {
        return -1;
    }
    mycopy(&queue->buffer[queue->elem_size*queue->tail],data_ptr,data_len);
    queue->tail = next_tail;
    queue->curr_usage++;
    return 0;
}


int queue_dequeue(queue_t *queue,uint8_t *dest_ptr)
{
    if(queue->head == queue->tail)
    {
        return -1;
    }
    // printf("Q Head %d Tail %d Usage %d\n",queue->head,queue->tail,queue->curr_usage);
    mycopy(dest_ptr,&queue->buffer[queue->elem_size*queue->head],queue->elem_size);
    queue->head = (queue->head + 1) % queue->length;
    queue->curr_usage--;
    return 0;
}


int queue_peek(queue_t *queue,uint8_t *dest_ptr)
{
    if(queue_isEmpty(queue)) return -1;

    mycopy(dest_ptr,&queue->buffer[queue->elem_size*queue->head],queue->elem_size);
    return 0;

}