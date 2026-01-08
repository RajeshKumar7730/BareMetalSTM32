


#include "fw_update.h"
#include "protobuf_handler.h"
#include "flash.h"
#include "uart_stack.h"
#include "pb_encode.h"
#include "crc.h"
static fw_upd_status_t upd_mgr;





static void send_status(UPD_CMD cmd,uint8_t status)
{
    proto_msg response ;
    response.which_payload = proto_msg_upd_resp_tag;
    response.payload.upd_resp.last_cmd = cmd;
    response.payload.upd_resp.status = status;

    uint8_t buffer[50];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer,50);
    if(!pb_encode(&stream, proto_msg_fields , &response))
    {
        printf("Encoding failed");
    }
    printf("Sent status %d\n",status);
    uart_stack_send_msg((uint8_t *)buffer,stream.bytes_written);
    
}


static uint32_t prepare_region_for_fw_upgrade(uint32_t size_in_bytes)
{
    uint32_t start_address = get_oldest_bank_start_address();
    printf("Starting add %x\n",start_address);
    uint8_t num_pages_to_erase = (size_in_bytes + PAGE_SIZE - 1) /PAGE_SIZE;
    for(int i=0;i< num_pages_to_erase;i++)
    {
        uint32_t addr = start_address+i*PAGE_SIZE;
        //printf("Current addr is %x\n",addr);
        bool rc = flash_erase(addr);
        if(rc == false) return INVALID_ADDRESS;
    }
    return start_address;
}


void fw_update_init()
{   
    upd_mgr.state = FW_UPD_STATE_IDLE;
    upd_mgr.num_chunks = 0;
    upd_mgr.start_address = INVALID_ADDRESS;
    upd_mgr.current_address = INVALID_ADDRESS;
}  

void handle_fw_update_msg(fw_upgrade_req *req)
{

    switch(upd_mgr.state)
    {
        case FW_UPD_STATE_IDLE : {
            uint8_t status = STATUS_UNEXPECTED_COMMAND;
            if(req->cmd == UPD_CMD_FLASH_ERASE)
            {
                printf("Erase cmd recieved\n");
                uint32_t start_address = prepare_region_for_fw_upgrade(req->fw_size);
                if(start_address != INVALID_ADDRESS)
                {
                    status = STATUS_FLASH_ERASE_SUCCESS;
                    upd_mgr.state = FW_UPD_STATE_GET_CHUNKS;
                    upd_mgr.start_address = start_address;
                    upd_mgr.current_address = start_address;
                }
                else{
                    status = STATUS_FLASH_ERASE_FAILED;
                    upd_mgr.state = FW_UPD_STATE_IDLE;
                }
            }
            send_status(req->cmd,status);
            break;
        }

        case FW_UPD_STATE_GET_CHUNKS : {
            if(req->cmd == UPD_CMD_DATA_CHUNK)
            {
                upd_mgr.num_chunks++;
                if(req->data_count % 2 == 0)
                {
                    flash_write(upd_mgr.current_address , (uint8_t *)req->data ,req->data_count*4);
                    upd_mgr.current_address += (req->data_count*4);
                }
                else
                {
                    printf("Invalid data size");
                }
                // printf("Chunks recieved %d  Len = %d \n",upd_mgr.num_chunks , req->data_count);
                if(upd_mgr.num_chunks % ACK_FOR_NUM_CHUNKS == 0)
                {
                    send_status(UPD_CMD_DATA_CHUNK,STATUS_NO_ERROR);
                    // upd_mgr.state = FW_UPD_STATE_SEND_ACK;
                }
            }
            else if(req->cmd == UPD_CMD_FW_UPGRADE_DONE)
            {
                printf("Full image recieved\n");
                uint8_t crc_status =STATUS_FW_CRC_FAILURE;
                fw_meta_t *metadata = (fw_meta_t *)upd_mgr.start_address;
                uint32_t actual_crc = metadata->crc;
                uint32_t computed_crc = calculate_crc8((uint8_t *)(upd_mgr.start_address+METADATA_SIZE),metadata->size);
                if(actual_crc == computed_crc)
                {
                    crc_status = STATUS_FW_CRC_SUCCESS;
                }
                send_status(UPD_CMD_FW_UPGRADE_DONE,crc_status);
                upd_mgr.state = FW_UPD_STATE_WAIT_FOR_REBOOT;
            }
            else
            {
                send_status(req->cmd,STATUS_UNEXPECTED_COMMAND);
            }
            break;
        }

        // case FW_UPD_STATE_SEND_ACK : {
        //     send_status(UPD_CMD_DATA_CHUNK,STATUS_NO_ERROR);
        //     upd_mgr.state = FW_UPD_STATE_GET_CHUNKS;
        //     break;
        // }
            
        // case FW_UPD_STATE_VERIFY_FW_INTEGRITY : {

        //     uint8_t crc_status =STATUS_FW_CRC_SUCCESS;
        //     send_status(UPD_CMD_FW_UPGRADE_DONE,crc_status);
        //     upd_mgr.state = FW_UPD_STATE_WAIT_FOR_REBOOT;
        //     break;
        // }

        case FW_UPD_STATE_WAIT_FOR_REBOOT : {
            
            if(req->cmd == UPD_CMD_RESET_DEVICE)
            {
                NVIC_SystemReset();
            }
            else
            {
                send_status(UPD_CMD_FW_UPGRADE_DONE,STATUS_UNEXPECTED_COMMAND);
            }
            break;
        }
        default : {
            printf("Invalid state \n");
            upd_mgr.state = FW_UPD_STATE_IDLE;
            break;
        }
    }
}


