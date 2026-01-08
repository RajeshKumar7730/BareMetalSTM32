

#include "protobuf_handler.h"
#include "uart_stack.h"
#include "pb_decode.h"
#include "pb_common.h"
#include "pb_encode.h"
#include "common.h"
#include "flash.h"
#include "uart.h"
#include "led.h"
#include "sys.h"
#include "fw_update.h"

void process_protobuf_message(uint8_t *rx_buf , uint8_t len)
{


    proto_msg msg = proto_msg_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(rx_buf, len);
     if (!pb_decode(&stream, proto_msg_fields, &msg)) {
        printf("PB decode failed: %s\n", PB_GET_ERROR(&stream));
        return;
    }


    if(msg.which_payload == proto_msg_add_req_tag)
    {
        proto_msg response = proto_msg_init_zero;
        response.which_payload = proto_msg_add_resp_tag;
        response.payload.add_resp.result = msg.payload.add_req.a + msg.payload.add_req.b;

        printf("Sum is %d \n",response.payload.add_resp.result);
        uint8_t buffer[30];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer,30);
        if(!pb_encode(&stream, proto_msg_fields , &response))
        {
            printf("Encoding failed");
        }

        uart_stack_send_msg((uint8_t *)buffer,stream.bytes_written);
    }
    else if(msg.which_payload == proto_msg_sw_info_req_tag)
    {
        printf("Got swinfo req");

        proto_msg resp = proto_msg_init_zero;
        resp.which_payload = proto_msg_sw_info_resp_tag;

        /* Bank 1 */
        uint32_t version = get_fw_version();
        printf("Version is %d\n",version);
        resp.payload.sw_info_resp.has_bank1 = true;
        resp.payload.sw_info_resp.bank1.active  = 1;
        resp.payload.sw_info_resp.bank1.major   = GET_MAJOR_VERSION(version);
        resp.payload.sw_info_resp.bank1.minor   = GET_MINOR_VERSION(version);
        resp.payload.sw_info_resp.bank1.patch   = GET_PATCH_VERSION(version);
        resp.payload.sw_info_resp.bank1.crc    = get_fw_crc();

        /* Bank 2 */
        // resp.payload.sw_info_resp.has_bank2 = false;
        // resp.payload.sw_info_resp.bank2.active  = 2;
        // resp.payload.sw_info_resp.bank2.major   = 0x12;
        // resp.payload.sw_info_resp.bank2.minor   = 0x22;
        // resp.payload.sw_info_resp.bank2.patch = 0x102;
        // resp.payload.sw_info_resp.bank2.crc    = 0xAABB0002;

        uint8_t buffer[100];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer,100);
        if(!pb_encode(&stream, proto_msg_fields , &resp))
        {
            printf("Encoding failed");
        }

        uart_stack_send_msg((uint8_t *)buffer,stream.bytes_written);

    }


    else if(msg.which_payload == proto_msg_reset_req_tag)
    {
        sys_reset();
    }


    else if(msg.which_payload == proto_msg_upd_req_tag)
    {
        handle_fw_update_msg(&msg.payload.upd_req);  
         // handle_fw_update(&msg.payload.upd_req);
        // uart_print_msg(msg.payload.upd_req.data , msg.payload.upd_req.data_count*4);
    }

    else if(msg.which_payload == proto_msg_led_req_tag )
    {
        ledOperate(msg.payload.led_req.operation);
    }
    else
    {
        printf("Payload is %d\n",msg.which_payload);
    }
}







/*
void handle_fw_update(fw_upgrade *req)
{
    // fw_up_pkt_t *packet = (fw_up_pkt_t *)request->data;
    // uint8_t cmd = packet->cmd;
    if(req->cmd == UPD_CMD_CMD_FLASH_ERASE)
    {
        
        static bool erased = false;
        if(erased){
            printf("Already erased\n");
        }
        else{
            erase_region_for_fw_upgrade();
            erased=true;
            printf("FW Upgrade Started at %d \n",sys_tick_get_current_ticks());
        }
    }

    else if(req->cmd == UPD_CMD_CMD_FLASH_WRITE)
    {
        static uint32_t address; 
        static int chunk = 0;
        if(chunk==0){
            address = get_oldest_bank_start_address();
            printf("Starting address %x\n", address);
        }
    #ifdef FW_UPDATE_DEBUG
        printf("Chnk %d , Len = %d \n",chunk ,req->data_count*4);
    #endif
        chunk++;
        flash_write(address,(uint8_t *)req->data,req->data_count*4);
        address += (4*req->data_count);
    }

    else if(req->cmd == UPD_CMD_CMD_FW_UPGRADE_DONE)
    {
        printf("FW Upd Done at %dd \n",sys_tick_get_current_ticks());
        delay();
        NVIC_SystemReset();
    }
    else
    {
        printf("Unsupoorted fw update command %d\n",req->cmd);
    }
}

*/


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

