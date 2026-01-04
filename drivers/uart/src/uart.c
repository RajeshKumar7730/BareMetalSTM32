#include "common.h"
#include "printf.h"
#include "sys.h"
#include "rb.h"
#include "uart.h"
// #include "msg_handler.h"

#define BUFFER_SIZE (255)
uint8_t tx_uart_buffer[BUFFER_SIZE];
rb_t tx_uart_rb;
uint8_t rx_uart_buffer[BUFFER_SIZE];
rb_t rx_uart_rb;


uint8_t tx_uart_buffer_usb[BUFFER_SIZE];
rb_t tx_uart_rb_usb;
uint8_t rx_uart_buffer_usb[BUFFER_SIZE];
rb_t rx_uart_rb_usb;



void uart_init(USART_TypeDef *uart, uint32_t baudrate , bool enable_irq)
{
    /*
        UART 5 External UART
        UART4 External UART */


    if(uart == UART5)
    {
        RCC->APB1ENR1 |= RCC_APB1ENR1_UART5EN;
        RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIODEN);
        /*
            UART5 Rx PD2 AF8
            UART5 Tx PC12 AF8
        */

        rb_init(&tx_uart_rb,tx_uart_buffer,BUFFER_SIZE);
        rb_init(&rx_uart_rb,rx_uart_buffer,BUFFER_SIZE);

        GPIOD->MODER |= GPIO_MODER_MODE2_1;
        GPIOD->MODER &= ~GPIO_MODER_MODE2_0;

        GPIOC->MODER |= GPIO_MODER_MODE12_1;
        GPIOC->MODER &= ~GPIO_MODER_MODE12_0;


        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL12_0;
        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL12_1;
        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL12_2;
        GPIOC->AFR[1] |=  GPIO_AFRH_AFSEL12_3;

        GPIOD->AFR[0] &= ~GPIO_AFRL_AFSEL2_0;
        GPIOD->AFR[0] &= ~GPIO_AFRL_AFSEL2_1;
        GPIOD->AFR[0] &= ~GPIO_AFRL_AFSEL2_2;
        GPIOD->AFR[0] |=  GPIO_AFRL_AFSEL2_3;

       
    }

    else if(uart == UART4)
    {
        /* UART 4 Tx - PC10
           UART 4 Rx - PC11*/

        RCC->APB1ENR1 |= RCC_APB1ENR1_UART4EN;
        RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);


        rb_init(&tx_uart_rb_usb,tx_uart_buffer_usb,BUFFER_SIZE);
        rb_init(&rx_uart_rb_usb,rx_uart_buffer_usb,BUFFER_SIZE);


        GPIOC->MODER |= GPIO_MODER_MODE10_1;
        GPIOC->MODER &= ~GPIO_MODER_MODE10_0;

        GPIOC->MODER |= GPIO_MODER_MODE11_1;
        GPIOC->MODER &= ~GPIO_MODER_MODE11_0;

    
        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL10_0;
        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL10_1;
        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL10_2;
        GPIOC->AFR[1] |=  GPIO_AFRH_AFSEL10_3;

        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL11_0;
        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL11_1;
        GPIOC->AFR[1] &= ~GPIO_AFRH_AFSEL11_2;
        GPIOC->AFR[1] |=  GPIO_AFRH_AFSEL11_3;


    }
    else if(uart == USART2)
    {
        /* USART 2 Tx - PA2
           USART 2 Rx - PA3*/

        RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
        RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);


        rb_init(&tx_uart_rb_usb,tx_uart_buffer_usb,BUFFER_SIZE);
        rb_init(&rx_uart_rb_usb,rx_uart_buffer_usb,BUFFER_SIZE);


        GPIOA->MODER |= GPIO_MODER_MODE2_1;
        GPIOA->MODER &= ~GPIO_MODER_MODE2_0;

        GPIOA->MODER |= GPIO_MODER_MODE3_1;
        GPIOA->MODER &= ~GPIO_MODER_MODE3_0;

    
        GPIOA->AFR[0] |=  GPIO_AFRL_AFSEL2_0;
        GPIOA->AFR[0] |=  GPIO_AFRL_AFSEL2_1;
        GPIOA->AFR[0] |=  GPIO_AFRL_AFSEL2_2;
        GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2_3;

        GPIOA->AFR[0] |=  GPIO_AFRL_AFSEL3_0;
        GPIOA->AFR[0] |=  GPIO_AFRL_AFSEL3_1;
        GPIOA->AFR[0] |=  GPIO_AFRL_AFSEL3_2;
        GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3_3;


    }


    uart->BRR = HCLK_FREQUENCY/baudrate;
    uart->CR1 |= USART_CR1_TE;
    uart->CR1 |= USART_CR1_RE;
    uart->CR1 |= USART_CR1_UE;
    // uart->CR1 |= USART_CR1_TXEIE;
    if(enable_irq)
    {
        uart->CR1 |= USART_CR1_RXNEIE;
    }


    if(uart == UART5 && enable_irq)
    {
        NVIC_EnableIRQ(UART5_IRQn);
        NVIC_SetPriority(UART5_IRQn, 2);  // Lower priority (higher number)
    }
    else if(uart == USART2 && enable_irq)
    {
        NVIC_EnableIRQ(USART2_IRQn);
        NVIC_SetPriority(USART2_IRQn, 6);  // Lower priority (higher number)
    }

    else if(uart == UART4 && enable_irq)
    {
        NVIC_EnableIRQ(UART4_IRQn);
        NVIC_SetPriority(UART4_IRQn, 2);  // Lower priority (higher number)
    }
    

}

void uart_enable_dma(USART_TypeDef *uart)
{
    uart->CR3 |= USART_CR3_DMAT;
}

void UART5_IRQHandler(void)
{

    if(UART5->ISR & USART_ISR_TXE)
    {
        uint8_t data;
        if(rb_get(&tx_uart_rb,&data))
        {
            UART5->TDR = data;
        }

        else{
            UART5->CR1 &= ~USART_CR1_TXEIE;
        }
    }


    if(UART5->ISR & USART_ISR_RXNE)
    {
        uint8_t data = UART5->RDR;
        rb_put(&rx_uart_rb,data);
        
    }

}


void UART4_IRQHandler(void)
{
    if(UART4->ISR & USART_ISR_TXE)
    {
        uint8_t data;
        if(rb_get(&tx_uart_rb_usb,&data))
        {
            UART4->TDR = data;
        }

        else{
            UART4->CR1 &= ~USART_CR1_TXEIE;
        }
    }


    if(UART4->ISR & USART_ISR_RXNE)
    {
        uint8_t data = UART4->RDR;
        // printf("D: %x\n",data);
        rb_put(&rx_uart_rb_usb,data);
        
    }

}



void USART2_IRQHandler(void)
{
    if(USART2->ISR & USART_ISR_TXE)
    {
        uint8_t data;
        if(rb_get(&tx_uart_rb_usb,&data))
        {
            USART2->TDR = data;
        }

        else{
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }


    if(USART2->ISR & USART_ISR_RXNE)
    {
        uint8_t data = USART2->RDR;
        rb_put(&rx_uart_rb_usb,data);
        
    }

}

void _putchar(char character)
{
  uart_write(DEFAULT_CLI_UART,character);
//   uart_write(UART4,character);

}

void uart_write(USART_TypeDef *uart, char ch)
{
    if(uart==UART4)
    {
        rb_put(&tx_uart_rb_usb,ch);

    }
    else if(uart==USART2)
    {
        rb_put(&tx_uart_rb_usb,ch);

    }
    else if(uart==UART5){
        rb_put(&tx_uart_rb,ch);

    }
    uart->CR1 |= USART_CR1_TXEIE;
}



void uart_send(USART_TypeDef *uart ,char *buffer)
{
    if(uart==UART5){
        for(int i=0;buffer[i]!='\0';i++)
        {
            rb_put(&tx_uart_rb,buffer[i]);
        }
    }

    else if(uart==UART4){
        for(int i=0;buffer[i]!='\0';i++)
        {
            rb_put(&tx_uart_rb_usb,buffer[i]);
        }
    }

    else if(uart==USART2){
        for(int i=0;buffer[i]!='\0';i++)
        {
            rb_put(&tx_uart_rb_usb,buffer[i]);
        }
    }
    
    
    uart->CR1 |= USART_CR1_TXEIE;
}


void uart_send_data(USART_TypeDef *uart ,uint8_t *data , uint8_t len)
{

    // uart_print_msg(data,len);
    if(uart == UART4)
    {
        for(int i=0;i<len;i++)
        {
            rb_put(&tx_uart_rb_usb,data[i]);
        }
    }

    uart->CR1 |= USART_CR1_TXEIE;

}

void uart_print_msg(uint8_t *buf,uint8_t buf_len)
{
    printf("Data    : [");
    for (uint8_t i = 0; i < buf_len; i++)
    {
        printf("0x%02X", buf[i]);
        if (i != buf_len - 1)
            printf(", ");
    }
    printf("]\n");
}

void uart_print_packet(uart_packet_t *pkt)
{
    printf("SOE        : 0x%02X\n", pkt->SOE);
    printf("Header     : 0x%02X\n", pkt->header);
    printf("CRC        : 0x%02X\n", pkt->crc);
    printf("PayloadLen : %u\n", pkt->payload_len);

    uart_print_msg(pkt->payload,pkt->payload_len);
}







