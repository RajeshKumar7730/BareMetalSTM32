
#include "uart_fsm.h"
#include "common.h"
#include "printf.h"
#include "flash.h"
#include "uart.h"
uint8_t data_buff_idx=0;
uint8_t temp_byte;
uint8_t msg_buff[20];
uint32_t header;
uint8_t data_bytes_cnt = 0;
static uint8_t request_buffer[sizeof(uart_packet_t) + 17];
static uart_packet_t *incoming_msg = (uart_packet_t *)request_buffer;
static void  clean_current_context()
{
    data_bytes_cnt = 0;
    for(int i=0;i<sizeof(request_buffer);i++)
    {
        request_buffer[i]=0;
    }
}






static fsm_t rx_fsm;
extern rb_t rx_uart_rb_usb;


void fsm_init(queue_t *queue){
    rx_fsm.queue = queue;
    rx_fsm.rb = &rx_uart_rb_usb;
    rx_fsm.state = COMMS_STATE_IDLE;
    
}


void uart_fsm_task(){


    switch(rx_fsm.state)
    {
        case COMMS_STATE_IDLE:
            if(rb_get(rx_fsm.rb,&temp_byte)==0) return;
            rx_fsm.state = (temp_byte == SOF_BYTE ) ? (COMMS_STATE_GET_PKT_HEADER) : (COMMS_STATE_IDLE);
            incoming_msg->SOE = temp_byte;
            break;

        case COMMS_STATE_GET_PKT_HEADER:{
            if(rb_get(rx_fsm.rb,&temp_byte)==0) return;
            incoming_msg->header = temp_byte;
            rx_fsm.state = COMMS_STATE_GET_CRC;
            break;
        }    
        case COMMS_STATE_GET_CRC:{
            if(rb_get(rx_fsm.rb,&temp_byte)==0) return;
            incoming_msg->crc = temp_byte;
            rx_fsm.state = COMMS_STATE_GET_PAYLOAD_LEN;
            break;
        }   
        case COMMS_STATE_GET_PAYLOAD_LEN:{
            if(rb_get(rx_fsm.rb,&temp_byte)==0) return;
            incoming_msg->payload_len = temp_byte;
            rx_fsm.state = COMMS_STATE_GET_PAYLOAD;
            break;
        } 
        case COMMS_STATE_GET_PAYLOAD:{
            if(data_bytes_cnt == incoming_msg->payload_len)
            {
                rx_fsm.state = COMMS_STATE_VERIFY_PKT;
                data_bytes_cnt = 0;
                // printf("Moving to check CRC\n");
            }
            else{
                if(rb_get(rx_fsm.rb,&temp_byte)==0) return;
                incoming_msg->payload[data_bytes_cnt] = temp_byte;
                data_bytes_cnt++;
            }
            break;
        }
        case COMMS_STATE_VERIFY_PKT:{
            uint8_t computed_crc = calculate_crc8((uint8_t *)&incoming_msg->payload , incoming_msg->payload_len);
            if(computed_crc != incoming_msg->crc)
            {
                printf("Invalid CRC\n");
                rx_fsm.state = COMMS_STATE_IDLE;
            }
            else
            {
                if(queue_enqueue(rx_fsm.queue,(uint8_t *)incoming_msg,rx_fsm.queue->elem_size) !=0 )
                {
                    printf("Failed to add pkt \n");
                }
                 else{
                // uart_print_packet(incoming_msg);
                // printf("Pkt Added \n");
                }
            }
            
           
            rx_fsm.state = COMMS_STATE_IDLE;
            clean_current_context();
            break;
        }

        default:
            rx_fsm.state = COMMS_STATE_IDLE;
            clean_current_context();

        
                
    }
}