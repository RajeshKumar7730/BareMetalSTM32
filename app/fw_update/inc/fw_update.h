
#ifndef FW_UPDATE_H
#define FW_UPDATE_H

#include "common.h"

#define ACK_FOR_NUM_CHUNKS (1)
#define INVALID_ADDRESS (0xFFFFFFFF)
typedef enum{
    FW_UPD_STATE_IDLE,
    FW_UPD_STATE_GET_CHUNKS,
    FW_UPD_STATE_SEND_ACK,
    FW_UPD_STATE_VERIFY_FW_INTEGRITY,
    FW_UPD_STATE_WAIT_FOR_REBOOT,
}fw_upd_state_t;

typedef struct{
    uint8_t state;
    uint16_t num_chunks;
    uint32_t current_address;
}fw_upd_status_t;


void fw_update_init();
void handle_fw_update_msg();



#endif /* FW_UPDATE_H */