MEMORY {
    FLASH     : ORIGIN = 0x08008000, LENGTH = 480K
    SRAM      : ORIGIN = 0x20000080, LENGTH = 96K
}

SECTIONS {
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

    .noinit : {
        *(.noinit*)
    } > SRAM
}
