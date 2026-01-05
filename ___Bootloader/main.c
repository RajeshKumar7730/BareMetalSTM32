
// User LED is PA5
// Connected to AHB1 bus
#include "common.h"
#include "sys.h"
#include "rb.h"
#include "string.h"
#include "printf.h"
#include "flash.h"
#include "crc.h"
#include "uart.h"
// #include "task.h"
extern rb_t rx_uart_rb;

char last_msg_recieved = 'N';



#define SRAM_START (0x20000000U)
#define SRAM_SIZE (96U * 1024U)  // STM32L476 has 96 KB SRAM
#define SRAM_END (SRAM_START + SRAM_SIZE)

#define FLASH_START_ADDRESS     (0x8000000)
#define BOOTLOADER_IMAGE_SIZE   (0x8000)
#define METADATA_SIZE           (0x80)
#define IMAGE1_START_ADDRESS    (FLASH_START_ADDRESS + BOOTLOADER_IMAGE_SIZE)
#define IMAGE2_START_ADDRESS    (BANK2_START_ADDRESS)
#define MAGIC_NUMBER            (0xDEADBEEF)
#define SRAM_LOAD_ADDRESS       (0x20000000)
#define IMAGE_ID_1              (1)
#define IMAGE_ID_2              (2)

typedef void (*func_ptr)(void);

// typedef struct {
//     uint32_t magic_number;
//     uint32_t version;
//     uint32_t size;
//     uint32_t crc;
//     uint32_t build_time;  // epoch
// } fw_meta_t;

static void jump_to_app(uint32_t img_address)
{
    func_ptr app_reset_handler;
    app_reset_handler = (func_ptr)(*(uint32_t *)(img_address + 4));
    SCB->VTOR = img_address;
    app_reset_handler();
}

static void copy_image_to_SRAM(uint8_t image_id, uint32_t load_address)
{
    printf("\nBank %d\n",image_id);
    for(int i=0;i<5;i++)
    {
        printf(".");
        delay();
    }
    uint32_t *src_ptr;
    fw_meta_t *image;
    if(image_id == IMAGE_ID_1){
        image = (fw_meta_t *)((uint32_t*)IMAGE1_START_ADDRESS);
        src_ptr = (uint32_t *) (IMAGE1_START_ADDRESS); // Copy along with metadata so that its easier to find which is active bank

    }
    else if(image_id == IMAGE_ID_2){
        image = (fw_meta_t *)((uint32_t*)IMAGE2_START_ADDRESS);
        src_ptr =(uint32_t *) (IMAGE2_START_ADDRESS );
    }
    // printf("Seltd image  Mgc Number : %x, Version : %d Bld Time : %d",image->magic_number ,
    //                                     image->version , image->build_time);

    uint32_t words_to_copy = image->size/4;
    uint32_t *dst_ptr = (uint32_t *)load_address;


    while(words_to_copy != 0)
    {
        *dst_ptr = *src_ptr;
        dst_ptr++;
        src_ptr++;
        words_to_copy--;
    }
    
    jump_to_app(load_address+METADATA_SIZE);

}


static bool validate_bank(uint32_t *bank_address)
{
    fw_meta_t *fw_meta = (fw_meta_t *)bank_address;
    if(fw_meta->magic_number != MAGIC_NUMBER)   return false;

    uint8_t computed_crc = 0;
    computed_crc = calculate_crc8(((uint8_t *)bank_address)+METADATA_SIZE,fw_meta->size);
    // printf("Bank 1 Actual %x Computed  CRC is %x\n",fw_meta->crc, computed_crc );

    if(computed_crc != fw_meta->crc)    return false;
    
    return true;
}

static uint8_t find_latest_image()
{
    fw_meta_t *fw_meta1 = (fw_meta_t *)IMAGE1_START_ADDRESS;
    fw_meta_t *fw_meta2 = (fw_meta_t *)IMAGE2_START_ADDRESS;
    if(fw_meta1->build_time >= fw_meta2->build_time)    return IMAGE_ID_1;
    else    return IMAGE_ID_2;
}

int main()
{
    sys_clock_init();
    uart_init(DEFAULT_CLI_UART,115200);
    printf("\nBootloader started");
    for(int i=0;i<3;i++)
    {
        printf(".");
        delay();
    }
    // led_init();
    // ledOperate(LED_OFF);

    bool is_bank1_valid = validate_bank((uint32_t *)IMAGE1_START_ADDRESS);
    bool is_bank2_valid = validate_bank((uint32_t *)IMAGE2_START_ADDRESS);

    if(!is_bank1_valid && !is_bank2_valid)
    {
        while(1)
        {
            printf("No valid image to boot\n");
            delay();
        }
    }
    else if(is_bank1_valid && !is_bank2_valid)
    {
        copy_image_to_SRAM(IMAGE_ID_1,SRAM_LOAD_ADDRESS);
    }
    else if(is_bank2_valid && !is_bank1_valid)
    {
        copy_image_to_SRAM(IMAGE_ID_2,SRAM_LOAD_ADDRESS);

    }
    else
    {
        uint8_t latest_image_id = find_latest_image();
        copy_image_to_SRAM(latest_image_id , SRAM_LOAD_ADDRESS);
    }

}




