#include "flash.h"
#include "printf.h"

static void unlock_flash()
{
    /*Unlock flash*/
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
}

static void lock_flash()
{
    FLASH->CR = FLASH_CR_LOCK;
}

static void clear_errors()
{
    FLASH->SR &= ~(FLASH_SR_OPERR | FLASH_SR_PROGERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZERR | FLASH_SR_PGSERR | FLASH_SR_MISERR | FLASH_SR_FASTERR | FLASH_SR_RDERR | FLASH_SR_OPTVERR );
}
static void WAIT_TILL_BSY_CLEAR()
{
    while(FLASH->SR & FLASH_SR_BSY);

}

static int check_bounds(uint32_t address)
{
    /*Aligning by page size to get the start of page if given address is somewhere between */
    
    if( (address < BANK1_START_ADDRESS) || (address >= BANK2_END_ADDRESS) )
    {
        return 1;
    }

    return 0;
}

uint32_t get_oldest_bank_start_address()
{
    fw_meta_t *image1 = (fw_meta_t *)((uint32_t*)IMAGE1_START_ADDRESS);
    fw_meta_t *image2 = (fw_meta_t *)((uint32_t*)IMAGE2_START_ADDRESS);
    if(image1->magic_number == MAGIC_NUMBER && image2->magic_number == MAGIC_NUMBER)
    {
        
        if(image1->build_time < image2->build_time)
        {
            return IMAGE1_START_ADDRESS;
        }
        else{
             return IMAGE2_START_ADDRESS;
        }
        
    }
    else if(image1->magic_number == MAGIC_NUMBER)
    {
        return IMAGE2_START_ADDRESS;
    }
    else if(image2->magic_number == MAGIC_NUMBER)
    {
        return IMAGE1_START_ADDRESS;
    }
    else{
        return IMAGE2_START_ADDRESS;
    }

    
}
void flash_erase(uint32_t address){
    
    address = address - address % PAGE_SIZE;

    if(check_bounds(address) != 0)
    {
        printf("Invalid flash erase arg\n");
        return;
    }
    
    bool is_bank2 = false;
    if(address >= BANK2_START_ADDRESS)
    {
        is_bank2 = true;
    }
    else{
        is_bank2 = false;
    }
    uint8_t pg_num;
    if(is_bank2)
    {
        pg_num = (address- BANK2_START_ADDRESS) / PAGE_SIZE;
    }
    else{
        pg_num = (address- BANK1_START_ADDRESS) / PAGE_SIZE;
    }
#ifdef FLASH_DEBUG
    printf("Erasing add %x and pg %d\n",address,pg_num);
#endif
    unlock_flash();
   
    WAIT_TILL_BSY_CLEAR();
    clear_errors();

    FLASH->CR |= FLASH_CR_PER;
    if(is_bank2)
    {
        FLASH->CR |= FLASH_CR_BKER;/*Bank 2 erase*/
    }
    else{
        FLASH->CR &= ~FLASH_CR_BKER;/*Bank 1 erase*/
    }
    FLASH->CR &= ~(FLASH_CR_PNB);/*Clearing*/
    FLASH->CR |= (pg_num << FLASH_CR_PNB_Pos);

    FLASH->CR |= FLASH_CR_STRT;
    
    WAIT_TILL_BSY_CLEAR();
    lock_flash();
#ifdef FLASH_DEBUG
    printf("Flash erase done\n");
#endif
}
void flash_write(uint32_t dest_addr,uint8_t *data,uint32_t len){


    if(check_bounds(dest_addr) != 0 || (len % 8 != 0) )
    {
        printf("Flash write failed");
        return;
    }
    unlock_flash();

    WAIT_TILL_BSY_CLEAR();
    clear_errors();

    FLASH->CR |= FLASH_CR_PG;

    uint32_t *dest_ptr = (uint32_t *)dest_addr;


    uint32_t *src_ptr =(uint32_t *)data;
    while(len!=0)
    {
        dest_ptr[0] = src_ptr[0];
        dest_ptr[1] = src_ptr[1];
        dest_ptr +=2;
        src_ptr +=2;
        len = len - 8;
        WAIT_TILL_BSY_CLEAR();
        FLASH->SR &= ~FLASH_SR_EOP;

    }

    FLASH->CR &= ~FLASH_CR_PG;

    lock_flash();
#ifdef FLASH_DEBUG
    printf("Flash write success\n");
#endif

}