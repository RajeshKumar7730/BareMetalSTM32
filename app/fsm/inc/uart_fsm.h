



#ifndef UART_FSM_H
#define UART_FSM_H
#include<stdint.h>
#include "common.h"
#include "rb.h"
#include "crc.h"

typedef enum{
    COMMS_STATE_IDLE,
    COMMS_STATE_GET_PKT_HEADER,
    COMMS_STATE_GET_CRC,
    COMMS_STATE_GET_PAYLOAD_LEN,
    COMMS_STATE_GET_PAYLOAD,
    COMMS_STATE_VERIFY_PKT
}fsm_state_t;


typedef struct{
    uint32_t state;
    rb_t *rb;
    queue_t *queue;
}fsm_t;

typedef struct{
    uint8_t state;
    uint8_t prevState;
    uint32_t error_flags;
    uint32_t warning_flags;
}telemetry_t;

#define SOF_BYTE (0x7E)
#define MAX_REQ_PAYLOAD_SIZE 20
void fsm_init(queue_t *queue);
extern rb_t rx_uart_rb;
extern rb_t tx_uart_rb;
void uart_fsm_task();

#endif