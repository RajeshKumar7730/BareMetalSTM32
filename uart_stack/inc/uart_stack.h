

#ifndef UART_STACK_INC_H
#define UART_STACK_INC_H
#include"common.h"
#include "isotp.h"
#include "queue.h"
typedef struct{
    USART_TypeDef *uart;
    uint32_t baudrate;
}uart_stack_config_t;



typedef struct{
    USART_TypeDef *port;
    queue_t *queue;
    isotp_ctx_t *recv_active_q;
    isotp_ctx_t *recv_done_q;
    isotp_ctx_t *send_active_q;
    isotp_ctx_t *recv_free_q;
    isotp_ctx_t *send_free_q;
    isotp_ctx_t recv_ctxs[RECV_Q_SIZE];
    isotp_ctx_t send_ctxs[SEND_Q_SIZE];
}my_uart_t;

bool uart_stack_init(uart_stack_config_t *config, queue_t *queue);
void uart_stack_task();
int uart_stack_send_msg(uint8_t *data,uint8_t len);
int uart_stack_get_msg(uint8_t *dest_buff,uint8_t *msg_len);
void send_isotp_frame(isotp_frame_t *frame);
extern my_uart_t uart_inst;

#endif /* UART_STACK_INC_H */