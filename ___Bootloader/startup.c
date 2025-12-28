#include <stdint.h>
#define SRAM_START (0x20000000U)
#define SRAM_SIZE (96U * 1024U)  // STM32L476 has 96 KB SRAM
#define SRAM_END (SRAM_START + SRAM_SIZE)
#define STACK_POINTER_INIT_ADDRESS (SRAM_END)
#define ISR_VECTOR_SIZE_WORDS 118  // STM32L476 has 118 interrupt vectors

void reset_handler(void);
void default_handler(void);

// Cortex-M system exceptions
void nmi_handler(void) __attribute__((weak, alias("default_handler")));
void hard_fault_handler(void) __attribute__((weak, alias("default_handler")));
void mem_manage_handler(void) __attribute__((weak, alias("default_handler")));
void bus_fault_handler(void) __attribute__((weak, alias("default_handler")));
void usage_fault_handler(void) __attribute__((weak, alias("default_handler")));
void svcall_handler(void) __attribute__((weak, alias("default_handler")));
void debug_monitor_handler(void) __attribute__((weak, alias("default_handler")));
void pendsv_handler(void) __attribute__((weak, alias("default_handler")));
void systick_handler(void) __attribute__((weak, alias("default_handler")));

// STM32L476 peripheral interrupt handlers
void wwdg_handler(void) __attribute__((weak, alias("default_handler")));
void pvd_pvm_handler(void) __attribute__((weak, alias("default_handler")));
void rtc_tamp_stamp_handler(void) __attribute__((weak, alias("default_handler")));
void rtc_wkup_handler(void) __attribute__((weak, alias("default_handler")));
void flash_handler(void) __attribute__((weak, alias("default_handler")));
void rcc_handler(void) __attribute__((weak, alias("default_handler")));
void exti0_handler(void) __attribute__((weak, alias("default_handler")));
void exti1_handler(void) __attribute__((weak, alias("default_handler")));
void exti2_handler(void) __attribute__((weak, alias("default_handler")));
void exti3_handler(void) __attribute__((weak, alias("default_handler")));
void exti4_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_channel1_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_channel2_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_channel3_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_channel4_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_channel5_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_channel6_handler(void) __attribute__((weak, alias("default_handler")));
void dma1_channel7_handler(void) __attribute__((weak, alias("default_handler")));
void adc1_2_handler(void) __attribute__((weak, alias("default_handler")));
void UART4_IRQHandler(void) __attribute__((weak, alias("default_handler"))); // Added USART4 handler
void UART5_IRQHandler(void) __attribute__((weak, alias("default_handler"))); // Added USART4 handler
void Default_Handler(void) __attribute__((weak, alias("default_handler"))); 
void SPI1_IRQHandler(void) __attribute__((weak, alias("default_handler"))); 
void TIM2_IRQ_Handler(void) __attribute__((weak, alias("default_handler"))); 



uint32_t isr_vector[ISR_VECTOR_SIZE_WORDS] __attribute__((section(".isr_vector"))) = {
    STACK_POINTER_INIT_ADDRESS,
    (uint32_t)&reset_handler,   		/* Reset Handler */
    (uint32_t)&Default_Handler,	/* NMI */
	(uint32_t)&Default_Handler,	/* Hard Fault */
	(uint32_t)&Default_Handler,  	/* MemManage */
	(uint32_t)&Default_Handler,      	/* BusFault  */
	(uint32_t)&Default_Handler,        /* UsageFault */
	(uint32_t)&Default_Handler,	/* Reserved */ 
	(uint32_t)&Default_Handler,	/* Reserved */
	(uint32_t)&Default_Handler,	/* Reserved */
	(uint32_t)&Default_Handler,	/* Reserved */
	(uint32_t)&svcall_handler,        /* SVCall */
	(uint32_t)&Default_Handler,	/* Debug */
	(uint32_t)&Default_Handler,	/* Reserved */
	(uint32_t)&pendsv_handler,        /* PendSV */
	(uint32_t)&systick_handler,      	/* SysTick */	
/* E(uint32_t)&xternal interrupt handlers follow */
	(uint32_t)&Default_Handler, 	/* 0 WWDG */
	(uint32_t)&Default_Handler, 	/* 1 PVD */
	(uint32_t)&Default_Handler, 	/* 2 TAMP_SAMP */
	(uint32_t)&Default_Handler, 	/* 3 RTC_WKUP */
	(uint32_t)&Default_Handler, 	/* 4 FLASH */
	(uint32_t)&Default_Handler, 	/* 5 RCC */
	(uint32_t)&Default_Handler, 	/* 6 EXTI0 */
	(uint32_t)&Default_Handler, 	/* 7 EXTI1 */
	(uint32_t)&Default_Handler, 	/* 8 EXTI2 and TSC */
	(uint32_t)&Default_Handler, 	/* 9 EXTI3 */
	(uint32_t)&Default_Handler, 	/* 10 EXTI4 */
	(uint32_t)&Default_Handler, 	/* 11 DMA_CH1 */
	(uint32_t)&Default_Handler, 	/* 12 DMA_CH2 */
	(uint32_t)&Default_Handler, 	/* 13 DMA_CH3 */
	(uint32_t)&Default_Handler, 	/* 14 DMA_CH4 */
	(uint32_t)&Default_Handler, 	/* 15 DMA_CH5 */
	(uint32_t)&Default_Handler, 	/* 16 DMA_CH6 */
	(uint32_t)&Default_Handler, 	/* 17 DMA_CH7 */
	(uint32_t)&adc1_2_handler, 	/* 18 ADC1_2 */
	(uint32_t)&Default_Handler, 	/* 19 CAN_TX */
	(uint32_t)&Default_Handler, 	/* 20 CAN_RX0 */
	(uint32_t)&Default_Handler, 	/* 21 CAN_RX1 */
	(uint32_t)&Default_Handler, 	/* 22 CAN_SCE */
	(uint32_t)&Default_Handler, 	/* 23 EXTI9_5 */
	(uint32_t)&Default_Handler, 	/* 24 TIM1_BRK/TIM15 */
	(uint32_t)&Default_Handler, 	/* 25 TIM1_UP/TIM16 */
	(uint32_t)&Default_Handler, 	/* 26 TIM1_TRG/TIM17 */
	(uint32_t)&Default_Handler, 	/* 27 TIM1_CC */
	(uint32_t)&TIM2_IRQ_Handler, 	/* 28 TIM2 */
	(uint32_t)&Default_Handler, 	/* 29 TIM3 */
	(uint32_t)&Default_Handler, 	/* 30 TIM4 */
	(uint32_t)&Default_Handler, 	/* 31 I2C1_EV */
	(uint32_t)&Default_Handler, 	/* 32 I2C1_ER */
	(uint32_t)&Default_Handler, 	/* 33 I2C2_EV */
	(uint32_t)&Default_Handler, 	/* 34 I2C2_ER */
	(uint32_t)&SPI1_IRQHandler, 	/* 35 SPI1 */
	(uint32_t)&Default_Handler, 	/* 36 SPI2 */
	(uint32_t)&Default_Handler, 	/* 37 USART1 */
	(uint32_t)&Default_Handler, 	/* 38 USART2 */
	(uint32_t)&Default_Handler, 	/* 39 USART3 */
	(uint32_t)&Default_Handler, 	/* 40 EXTI15_10 */
	(uint32_t)&Default_Handler, 	/* 41 RTCAlarm */
	(uint32_t)&Default_Handler, 	/* 42 DFSDM1_FLT3 */
	(uint32_t)&Default_Handler, 	/* 43 TIM8_BRK */
	(uint32_t)&Default_Handler, 	/* 44 TIM8_UP */
	(uint32_t)&Default_Handler, 	/* 45 TIM8_TRG_COM */
	(uint32_t)&Default_Handler, 	/* 46 TIM8_CC */
	(uint32_t)&Default_Handler, 	/* 47 ADC3 */
	(uint32_t)&Default_Handler, 	/* 48 FMC */
	(uint32_t)&Default_Handler, 	/* 49 SDMMC1 */
	(uint32_t)&Default_Handler, 	/* 50 TIM5 */
	(uint32_t)&Default_Handler, 	/* 51 SPI3 */
	(uint32_t)&UART4_IRQHandler, 	/* 52 UART4 */
	(uint32_t)&UART5_IRQHandler, 	/* 53 UART5 */
	(uint32_t)&Default_Handler, 	/* 54 TIM6_DACUNDER */
	(uint32_t)&Default_Handler, 	/* 55 TIM7 */
	(uint32_t)&Default_Handler, 	/* 56 DMA2_CH1 */
	(uint32_t)&Default_Handler, 	/* 57 DMA2_CH2 */
	(uint32_t)&Default_Handler, 	/* 58 DMA2_CH3 */
	(uint32_t)&Default_Handler, 	/* 59 DMA2_CH4 */
	(uint32_t)&Default_Handler, 	/* 60 DMA2_CH5 */
	(uint32_t)&Default_Handler, 	/* 61 DFSDM1_FLT0 */
	(uint32_t)&Default_Handler, 	/* 62 DFSDM1_FLT1 */
	(uint32_t)&Default_Handler, 	/* 63 DFSDM1_FLT2*/
	(uint32_t)&Default_Handler, 	/* 64 COMP */
	(uint32_t)&Default_Handler, 	/* 65 LPTIM1 */
	(uint32_t)&Default_Handler, 	/* 66 LPTIM2 */
	(uint32_t)&Default_Handler, 	/* 67 OTG_FS */
	(uint32_t)&Default_Handler, 	/* 68 DMA_CH6 */
	(uint32_t)&Default_Handler, 	/* 69 DMA_CH7 */
	(uint32_t)&Default_Handler, 	/* 70 LPUART1 */
	(uint32_t)&Default_Handler, 	/* 71 QUADSPI */
	(uint32_t)&Default_Handler, 	/* 72 I2C3_EV */
	(uint32_t)&Default_Handler, 	/* 73 I2C3_ER */
	(uint32_t)&Default_Handler, 	/* 74 SAI1 */
	(uint32_t)&Default_Handler, 	/* 75 SAI2 */
	(uint32_t)&Default_Handler, 	/* 76 SWPMI1 */
	(uint32_t)&Default_Handler, 	/* 77 TSC */
	(uint32_t)&Default_Handler, 	/* 78 LCD */
	(uint32_t)&Default_Handler, 	/* 79 AES */
	(uint32_t)&Default_Handler, 	/* 80 RNG */
	(uint32_t)&Default_Handler, 	/* 81 FPU */
};

extern int main(void);
extern int a;
extern int32_t _sdata,_edata,_lAddr,_sbss,_ebss;
void reset_handler()
{
    
	for(int i=0;i<10000;i++);
    uint8_t *flash_addr = (uint8_t *)&_lAddr;
    uint8_t *RAM_dstart = (uint8_t *)&_sdata;

    uint32_t size = (uint8_t *)&_edata- (uint8_t *)&_sdata;
    for(int i=0;i<size;i++)
    {
        RAM_dstart[i] = flash_addr[i];
    }


    uint8_t *RAM_bstart = (uint8_t *)&_sbss;
    size = (uint8_t *)&_ebss - (uint8_t *)&_sbss;
    for(int i=0;i<size;i++)
    {
        RAM_bstart[i] = 0;
    }
	
    main();

}

void default_handler(void)
{
  while(1);
}



