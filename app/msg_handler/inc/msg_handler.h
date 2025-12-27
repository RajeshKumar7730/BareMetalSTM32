


#ifndef MSG_HANDLER_H
#define MSG_HANDLER_H
#include "common.h"

typedef struct {
    uint8_t msg_id;
    uint8_t flags;
    uint8_t msg_len;
    uint8_t data[120];
}msg_t;


typedef enum{
    MSG_ID_DEFAULT = 0,
    MSG_ID_SW_INFO=1,
    MSG_ID_STATS=2,
    MSG_ID_ADDITION=3,
    MSG_ID_OWNER_NAME=4,
    MSG_ID_TOGGLE_LED=5
}msg_id_e;

typedef enum{
    MSG_FLAG_REQUEST = 0,
    MSG_FLAG_RESPONSE = 1,
}msg_flag_e;


typedef enum{
    PROTOCOL_VERSION_V1 = 0
}protocol_version_e;


typedef enum{
    FW_UPDATER_CMD_ERASE_REGION = 1,
    FW_UPDATER_CMD_WRITE_CHUNK_TO_FLASH = 2,
    FW_UPDATER_CMD_RESET_YOURSELF =3
}fw_upd_cmd_e;

typedef struct{
    uint32_t prevState;
    uint32_t currState;
    char errors[5];
    float num;
}stats_t;

// typedef struct {
//     bool active;
//     uint8_t major;
//     uint8_t minor;
//     uint8_t version;
//     uint8_t hash;
// }bankinfo_t;

// typedef struct{
//     bankinfo_t bank1;
//     bankinfo_t bank2;
//     bankinfo_t bank3;
//     bankinfo_t bank4;  
// }sw_info;


typedef struct{
    uint8_t prevState;
    uint8_t currState;
}led_info_t;



typedef struct{
    uint8_t cmd;    // FW Update cmd refer fw_upd_cmd_e
    uint8_t data[16];
}fw_up_pkt_t;






void handle_request_msgs(msg_id_e msg_id,uint8_t *buffer,uint8_t size);
void process_rx_msg();
#endif