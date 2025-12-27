

#include "msg_definitions.h"



msg_handlers request_handler_arr[MAX_MESSAGES_SUPPORTED] = {
    math_request_handler,
    sw_info_request_handler,
    stats_request_handler,
    toggle_led_request_handler,
    get_owner_request_handler,
    sw_reset_handler,
    fw_upgrade_handler,
};