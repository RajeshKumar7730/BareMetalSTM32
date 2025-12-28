


MEMORY {
    FLASH : ORIGIN = 0x8000000 , LENGTH = 32K
    SRAM  : ORIGIN = 0x20000000 , LENGTH = 96K 
}


SECTIONS{
    .text : {
        *(.isr_vector)
        *(.text)
        *(.rodata)
        _lAddr = .;
    } > FLASH

    .data : {
        _sdata = .;
        *(.data)
        _edata = .;
    } > SRAM AT > FLASH

    .bss : {
        _sbss = .;
        *(.bss)
        _ebss = .;
    } > SRAM





}










