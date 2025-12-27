

#include "string.h"

#include "msg_handler.h"
#include "led.h"
#include "msg_definitions.h"
#include "printf.h"
#include "sys.h"
typedef void (*msg_handlers)(msg_t *request);
#include "flash.h"
#include "uart_stack.h"
#include "uart.h"
#include "protobuf_handler.h"
static uint8_t resp_buffer[sizeof(msg_t)];

void clear_response()
{
    for (int i = 0; i < sizeof(msg_t); i++) {
        resp_buffer[i] =0;
    }
}



void math_request_handler(msg_t *request)
{   
    msg_t *response = (msg_t *)resp_buffer;
    response->msg_id =MSG_ID_DEFAULT;
    response->flags = MSG_FLAG_RESPONSE;
    response->data[0]=request->data[0]+request->data[1];
    response->msg_len = 1;
    printf("Sum is %d\n",response->data[0]);
    // uint8_t msg_buffer[]="Hi\0";
    // mycopy(response->data,msg_buffer,sizeof(msg_buffer));
    // response->data_length = sizeof(msg_buffer);
    uart_stack_send_msg((uint8_t *)response,4);
}

void stats_request_handler(msg_t *request)
{
    msg_t *response = (msg_t *)resp_buffer;
    response->msg_id = MSG_ID_STATS;
    response->flags = MSG_FLAG_RESPONSE;
    // response->protocol_version = PROTOCOL_VERSION_V1;
    response->msg_len = sizeof(stats_t);    
    stats_t *stats_at_ts=(stats_t *)response->data;
    stats_at_ts->currState =12345678;
    stats_at_ts->prevState =87654321;
    stats_at_ts->errors[0] = 'A';
    stats_at_ts->num = 99.99;
    uart_stack_send_msg((uint8_t *)response,sizeof(stats_t)+3);
}




void sw_info_request_handler(msg_t *request)
{
    // msg_t *response = (msg_t *)resp_buffer;
    // response->msg_id = MSG_ID_SW_INFO;
    // response->flags = MSG_FLAG_RESPONSE;
    // // response->protocol_version = PROTOCOL_VERSION_V1;
    // sw_info *info = (sw_info *)response->data;
    // info->bank1 = (bankinfo_t ) {.active =1,
    //                 .major = 1,
    //                 .minor = 2,
    //                 .version = 3,
    //                 .hash = 4 };
    // info->bank2 = (bankinfo_t){.active = 0,
    //                 .major = 5,
    //                 .minor = 6,
    //                 .version = 7,
    //                 .hash = 8};
    // info->bank3 = (bankinfo_t ) {.active =1,
    //                 .major = 9,
    //                 .minor = 0xa,
    //                 .version = 0xb,
    //                 .hash = 0xc };
    // info->bank4 = (bankinfo_t){.active = 0,
    //                 .major = 0xd,
    //                 .minor = 0xe,
    //                 .version = 0xf,
    //                 .hash = 0xAB};
    // response->msg_len = sizeof(sw_info); 
    
    // uart_stack_send_msg((uint8_t *)response,sizeof(sw_info) + 3);
}


void get_owner_request_handler(msg_t *request)
{
    char name[]="Sri Krishna\0";
    msg_t *response = (msg_t *)resp_buffer;
    response->msg_id =MSG_ID_OWNER_NAME;
    response->flags = MSG_FLAG_RESPONSE;
    // response->protocol_version = PROTOCOL_VERSION_V1;
    response->msg_len = sizeof(name);
    mycopy(response->data,(uint8_t *)name,sizeof(name)+3);
    //uart_send_frame(DEFAULT_DEBUG_TOOL_UART,response);
    
}


void toggle_led_request_handler(msg_t *request)
{
    msg_t *response = (msg_t *)resp_buffer;
    response->msg_id =MSG_ID_TOGGLE_LED;
    response->flags = MSG_FLAG_RESPONSE;
    // response->protocol_version = PROTOCOL_VERSION_V1;
    response->msg_len = sizeof(led_info_t);
    led_info_t *info = (led_info_t *)response->data;
    info->prevState = getLedState();
    ledOperate(LED_TOGGLE);
    info->currState = getLedState();
    uart_stack_send_msg((uint8_t *)response,sizeof(led_info_t) + 3);
}

static void erase_region_for_fw_upgrade()
{
    // uint32_t address = get_oldest_bank_start_address();
    // for(int i=0;i<10;i++)
    // {
    //     uint32_t addr = address+i*PAGE_SIZE;
    //     printf("Current addr is %x\n",addr);
    //     flash_erase(addr);
    // }

}

void fw_upgrade_handler(msg_t *request)
{
    // fw_up_pkt_t *packet = (fw_up_pkt_t *)request->data;
    // uint8_t cmd = packet->cmd;
    // if(cmd == FW_UPDATER_CMD_ERASE_REGION)
    // {
        
    //     static bool erased = false;
    //     if(erased){
    //         printf("Already erased\n");
    //     }
    //     else{
    //         erase_region_for_fw_upgrade();
    //         erased=true;
    //         printf("FW Upgrade Started at %d \n",sys_tick_get_current_ticks());
    //     }
    // }

    // else if(cmd == FW_UPDATER_CMD_WRITE_CHUNK_TO_FLASH)
    // {
    //     static uint32_t address; 
    //     static int chunk = 0;
    //     if(chunk==0){
    //         address = get_oldest_bank_start_address();
    //         printf("Starting address %x\n", address);
    //     }
    // #ifdef FW_UPDATE_DEBUG
    //     printf("Chunk %d , Length = %d \n",chunk ,request->data_length);
    // #endif
    //     chunk++;
    //     flash_write(address,request->data+1,request->data_length-1);
    //     address += (request->data_length - sizeof(uint8_t));
    // }

    // else if(cmd == FW_UPDATER_CMD_RESET_YOURSELF)
    // {
    //     printf("FW Upgrade Done at %dd \n",sys_tick_get_current_ticks());
    //     delay();
    //     delay();
    //     delay();
    //     delay();
    //     NVIC_SystemReset();
    // }
    // else
    // {
    //     printf("Unsupoorted fw update command %d\n",cmd);
    // }
}




void sw_reset_handler(msg_t * request)
{
    NVIC_SystemReset();
}


void process_rx_msg()
{


    uint8_t msg_buffer[200];
    uint8_t msg_len;
    if(uart_stack_get_msg(msg_buffer,&msg_len) == 0)
    {
        // printf("Recv success len = %d\n",msg_len);
        process_protobuf_message(msg_buffer,msg_len);
    
    }
}
