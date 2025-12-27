

#include "led.h"
#include "common.h"
#include "uart.h"
#include "sys.h"
#include "rb.h"
#include "string.h"
#include "uart_fsm.h"
#include "printf.h"
#include "flash.h"
#include "uart_stack.h"
#include "crc.h"
#include "queue.h"
#include "protobuf_handler.h"
#define MAX_MSGS_IN_UART_PKTS_QUEUE (8)

typedef void(*task_fn)();
typedef struct{
    char *name;
    task_fn execute_fn;
    bool ready_to_run;
    uint16_t timer;
    uint16_t periodicity;
}task_t;

void print(){
    // uint8_t data1[]={0x11,0x12,0x13,0x14,0x15,0x16,0x17};

    // // isotp_frame_t frame;
    // // frame.frame_info = (ISOTP_FRAME_TYPE_FF << 4) | sizeof(data);
    // // frame.payload = data;
    // // send_isotp_frame(&frame);
    // //uart_stack_send_msg(data1,sizeof(data1));
    // uint8_t data2[] = {
    // 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    // 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    // 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    // 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    // 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    // 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    // 0x80};
    //uart_stack_send_msg(data2,sizeof(data2));
}
task_t periodic_tasks[]={
    {.name= "Blinky Task" ,          .execute_fn = led_task , .ready_to_run = false ,.timer = 0 , .periodicity = 1000 },
    {.name= "Print Task" ,           .execute_fn = print ,    .ready_to_run = false ,.timer = 0 , .periodicity = 2500 },
    {.name= "Msg processing Task" ,  .execute_fn = process_rx_msg ,    .ready_to_run = false ,.timer = 0 , .periodicity = 5 },

};

task_t immediate_tasks[]={
    {.name= "Rx FSM Task" , .execute_fn = uart_fsm_task },
    {.name= "UART Task" , .execute_fn = uart_stack_task },
};


void update_periodic_tasks()
{
    for(int i=0;i < sizeof(periodic_tasks)/sizeof(periodic_tasks[0]) ; i++)
    {
        task_t *curr_task = &periodic_tasks[i];
        if(curr_task->timer >= curr_task->periodicity)
        {
            curr_task->ready_to_run = true;
        }
        curr_task->timer++;
    }
}

/*Executes both periodic and immediate tasks*/
void execute_tasks()
{
    for(int i=0;i < sizeof(periodic_tasks)/sizeof(periodic_tasks[0]) ; i++)
    {
        task_t *curr_task = &periodic_tasks[i];
        if(curr_task->ready_to_run && (curr_task->execute_fn != NULL) )
        {
            curr_task->execute_fn();
            // printf("Task : %s",curr_task->name);
            curr_task->timer = 0;
            curr_task->ready_to_run = false;
        }
    }

    for(int i=0;i < sizeof(immediate_tasks)/sizeof(immediate_tasks[0]) ; i++)
    {
        task_t *curr_task = &immediate_tasks[i];
        if((curr_task->execute_fn != NULL) )
        {
            curr_task->execute_fn();
        }
    }


}

int main()
{
    sys_clock_init();
    uart_init(UART5, 115200);

    led_init();
    sys_tick_init(1000,&update_periodic_tasks);

    printf("\nApp booted\n");
    
    queue_t uart_rx_packets_q;
    uint8_t uart_rx_packets_buf[MAX_MSGS_IN_UART_PKTS_QUEUE*MAX_UART_PKT_SIZE];
    queue_init(&uart_rx_packets_q, uart_rx_packets_buf , 4 , MAX_UART_PKT_SIZE);
    fsm_init(&uart_rx_packets_q);

    uart_stack_config_t config = {.uart = DEFAULT_DEBUG_TOOL_UART , .baudrate =115200};
    uart_stack_init(&config,&uart_rx_packets_q);


    fw_meta_t *ptr = (fw_meta_t *)IMAGE1_START_ADDRESS;
    uint8_t crc = 0;
    crc = calculate_crc8((uint8_t *)(IMAGE1_START_ADDRESS+METADATA_SIZE),ptr->size);
    printf("Bank 1 Computed  CRC is %x\n", crc);
    ptr = (fw_meta_t *)IMAGE2_START_ADDRESS;
    crc = 0;
    crc = calculate_crc8((uint8_t *)(IMAGE2_START_ADDRESS+METADATA_SIZE),ptr->size);
    printf("Bank 2 Computed  CRC is %x\n", crc);


    while(1)
    {   
        execute_tasks();
        
        
    }
    
}





