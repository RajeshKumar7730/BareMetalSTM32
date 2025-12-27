#ifndef MSG_DEFINITIONS_H
#define MSG_DEFINITIONS_H

#include "msg_handler.h"


void default_msg_handler(msg_t *request);
void sw_info_request_handler(msg_t *request);
void stats_request_handler(msg_t *request);
void math_request_handler(msg_t *request);
void get_owner_request_handler(msg_t *request);
void toggle_led_request_handler(msg_t *request);
void sw_reset_handler(msg_t * request);
void fw_upgrade_handler(msg_t *request);


#define MAX_MESSAGES_SUPPORTED (7)
typedef void (*msg_handlers)(msg_t *request);
extern msg_handlers request_handler_arr[MAX_MESSAGES_SUPPORTED];

#define IS_MSG_ID_VALID(msg_id)     (msg_id >=0 && msg_id < MAX_MESSAGES_SUPPORTED) ? 1 : 0

#endif /*MSG_DEFINITIONS_H*/