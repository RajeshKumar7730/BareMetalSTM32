
#ifndef ISOTP_H
#define ISOTP_H

#define MAX_DATA_SIZE (200)
#define RECV_Q_SIZE   (3)
#define SEND_Q_SIZE   (3)

#define ISOTP_FRAME_TYPE_SF (0x1)
#define ISOTP_FRAME_TYPE_FF (0x2)
#define ISOTP_FRAME_TYPE_CF (0x3)
#define ISOTP_FRAME_TYPE_FC (0x4)
#define ISOTP_MAX_SF_DATA_LENGTH (15)
#define ISOTP_MAX_FF_DATA_LENGTH (15)
#define ISOTP_MAX_CF_DATA_LENGTH (15)


#include "common.h"
typedef enum{
    ISOTP_STATE_TX_PENDING,
    ISOTP_STATE_WAIT_FOR_FC,
    ISOTP_STATE_SEND_CF,
    ISOTP_STATE_SEND_FC,
    ISOTP_STATE_WAIT_FOR_CF,
    ISOTP_STATE_TX_DONE,
    ISOTP_STATE_RX_DONE
}isotp_state_e;
typedef struct isotp_ctx_t{
    isotp_state_e isotp_state;
    union{
        uint8_t sent_len;
        uint8_t recv_len;
    };
    uint8_t data_len;
    uint8_t data[MAX_DATA_SIZE];
    struct isotp_ctx_t * next;
}isotp_ctx_t;

typedef struct __attribute__((packed))
{
    
    uint8_t PCI ;
    union{
        uint8_t data_len;
        uint8_t seq_num;
    };
    uint8_t *data;
}isotp_frame_t;

uint16_t get_frame_len(isotp_frame_t *frame);
#endif /* ISOTP_H */