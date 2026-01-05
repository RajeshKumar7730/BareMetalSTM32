

#ifndef FLASH_H
#define FLASH_H
#include "common.h"


#define PAGE_SIZE (0x800)
#define NUM_PPAGES_IN_BANK (256)
#define BANK_SIZE (NUM_PPAGES_IN_BANK * PAGE_SIZE)
#define BANK1_START_ADDRESS (0x08000000)
#define BANK1_END_ADDRESS   (BANK1_START_ADDRESS + BANK_SIZE)
#define BANK2_START_ADDRESS (0x08080000)
#define BANK2_END_ADDRESS (BANK2_START_ADDRESS + BANK_SIZE)


#define FLASH_START_ADDRESS     (0x8000000)
#define BOOTLOADER_IMAGE_SIZE   (0x8000)
#define METADATA_SIZE           (0x80)
#define IMAGE1_START_ADDRESS    (FLASH_START_ADDRESS + BOOTLOADER_IMAGE_SIZE)
#define IMAGE2_START_ADDRESS    (BANK2_START_ADDRESS)
#define MAGIC_NUMBER            (0xDEADBEEF)
#define SRAM_LOAD_ADDRESS       (0x20000000)
#define IMAGE_ID_1              (1)
#define IMAGE_ID_2              (2)

void flash_erase(uint32_t address);
void flash_write(uint32_t dest_addr,uint8_t *data,uint32_t len);
uint32_t get_oldest_bank_start_address();
#endif /* FLASH_H */