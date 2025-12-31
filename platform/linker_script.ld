MEMORY {
    FLASH     : ORIGIN = 0x08008000, LENGTH = 480K
    SRAM      : ORIGIN = 0x20000000, LENGTH = 96K
}

SECTIONS {
    .metadata : {
        *(.metadata)
    } > FLASH

    .text : {
        _stext = .;
        *(.isr_vector)
        *(.text*)
        *(.rodata*)
        _etext = .;
        . = ALIGN(4);
    } > SRAM

    .data : {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > SRAM

    .bss : {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM


    .no_init : {
        *(.no_init)
    } > SRAM

}
