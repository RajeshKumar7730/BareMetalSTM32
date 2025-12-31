

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
#include "FreeRTOS.h"
#include "projdefs.h"
#include "task.h"
#define MAX_MSGS_IN_UART_PKTS_QUEUE (8)

typedef void(*task_fn)();
typedef struct{
    char *name;
    task_fn execute_fn;
    bool ready_to_run;
    uint16_t timer;
    uint16_t periodicity;
}task_t;


task_t periodic_tasks[]={
    {.name= "Blinky Task" ,          .execute_fn = led_task , .ready_to_run = false ,.timer = 0 , .periodicity = 1000 },
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

void Task1(void *pvParameters);
void Task2(void *pvParameters);
void Task3(void *pvParameters);


volatile fault_info_t info __attribute__((section(".no_init")));


void print_fault_info(void)
{
    printf("Address of info is %x\n",&info);
    if (info.magic != FAULT_MAGIC)
    {
        printf("No magic %x %x\n",&info.magic , info.magic);
        return;
    }

    printf("\n---  FAULT DETECTED ---\n");
    printf("PC   : 0x%08lX\n", info.pc);
    printf("LR   : 0x%08lX\n", info.lr);
    printf("PSR  : 0x%08lX\n", info.psr);
    printf("IPSR : %lu\n",     info.ipsr);
    printf("CFSR : 0x%08lX\n", info.cfsr);
    printf("HFSR : 0x%08lX\n", info.hfsr);
    printf("IRQn : 0x%08lX\n", info.irqn);
    /* Clear after printing */
    info.magic = 0;
}

int main()
{
    // sys_clock_init();
    uart_init(UART5, 115200);
    led_init();
    printf("RTOS Image booted\n");
    print_fault_info();

/*
    sys_tick_init(1000,&update_periodic_tasks);

    printf("1.1.1 Img booted\n");
    
    queue_t uart_rx_packets_q;
    uint8_t uart_rx_packets_buf[MAX_MSGS_IN_UART_PKTS_QUEUE*MAX_UART_PKT_SIZE];
    queue_init(&uart_rx_packets_q, uart_rx_packets_buf , 4 , MAX_UART_PKT_SIZE);
    fsm_init(&uart_rx_packets_q);

    uart_stack_config_t config = {.uart = DEFAULT_DEBUG_TOOL_UART , .baudrate =115200};
    uart_stack_init(&config,&uart_rx_packets_q);


    // fw_meta_t *ptr = (fw_meta_t *)IMAGE1_START_ADDRESS;
    // uint8_t crc = 0;
    // crc = calculate_crc8((uint8_t *)(IMAGE1_START_ADDRESS+METADATA_SIZE),ptr->size);
    // printf("Bank 1 Computed  CRC is %x\n", crc);
    // ptr = (fw_meta_t *)IMAGE2_START_ADDRESS;
    // crc = 0;
    // crc = calculate_crc8((uint8_t *)(IMAGE2_START_ADDRESS+METADATA_SIZE),ptr->size);
    // printf("Bank 2 Computed  CRC is %x\n", crc);



    
*/


    xTaskCreate(Task1,"Task 1",100,NULL,1,NULL);
    xTaskCreate(Task2,"Task 2",100,NULL,1,NULL);
    // xTaskCreate(Task3,"Task 3",100,NULL,3,NULL);

    vTaskStartScheduler();
    while(1)
    {   
        // // // execute_tasks();
        // ledOperate(LED_TOGGLE);
        // printf("Hi Indu\n");
        // delay();
        
    }
    
}


void Task1(void *pvParameters)
{
    while(1)
    {
        printf("\nHi ");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}


void Task2(void *pvParameters)
{
    while(1)
    {
        printf("Raj ");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void Task3(void *pvParameters)
{
    while(1)
    {
        printf("No time\n");
        delay();
    }
}




