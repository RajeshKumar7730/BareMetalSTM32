#include "dma.h"
// #include "uart.h"
extern uint8_t memoryA[10];
extern uint8_t memoryB[10];

/*Example config for DMA

uint8_t memoryA[10] = {
    'a','b','c','d','e','f','g','h','i','j'
};
char ALPHABET[131] = {
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z',
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z',
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z',
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z',
    'A','B','C','D','E','F','G','H','I','J',
    'K','L','M','N','O','P','Q','R','S','T',
    'U','V','W','X','Y','Z','\n'
};
uint8_t memoryB[10];

dma_config_t uart_tx_config = {
        .dma = DMA2,
        .dma_ch = DMA2_Channel3,
        .transfer_type = MEM_TO_PERIPH,
        .src_address =  (uint32_t)ALPHABET,
        .dest_address = (uint32_t)&(UART4->TDR),
        .transfer_size = 1,
        .total_bytes = sizeof(ALPHABET),
        .priority = DMA_PRIORITY_LOW,
        .irq = DMA2_Channel3_IRQn,
        .irq_priority = 5,
        .ch_conf = &(ch_sel_conf_t){ .channel_number = 3, .request_num = 0b0010 }
    };
    // dma_init(&uart_tx_config);

    dma_config_t mem2mem_config = {
        .dma = DMA1,
        .dma_ch = DMA1_Channel1,
        .transfer_type = MEM_TO_MEM,
        .src_address =  (uint32_t)memoryA,
        .dest_address = (uint32_t)memoryB,
        .transfer_size = 1,
        .total_bytes = sizeof(memoryA),
        .priority = DMA_PRIORITY_LOW,
        .irq = DMA1_Channel1_IRQn,
        .irq_priority = 5,
        .ch_conf = NULL
    };
*/


static void enable_dma_channel(DMA_Channel_TypeDef * dma_channel)
{
    dma_channel->CCR |=(DMA_CCR_EN);
}

static void disable_dma_channel(DMA_Channel_TypeDef * dma_channel)
{
    dma_channel->CCR |=(DMA_CCR_EN);
}

void dma_init(dma_config_t *config)
{
    // RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    DMA_Channel_TypeDef *dma_ch = config->dma_ch;
    if(config->dma == DMA1)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    }
    else
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    }
    disable_dma_channel(dma_ch);
    dma_ch->CCR = 0;
    dma_ch->CCR = (DMA_CCR_TCIE | DMA_CCR_TEIE);
    dma_ch->CCR |= ((uint16_t)config->priority << DMA_CCR_PL_Pos);
    dma_ch->CNDTR = config->total_bytes / config->transfer_size;
    dma_ch->CCR |= (((uint16_t)config->transfer_size - 1) << DMA_CCR_PSIZE_Pos);
    dma_ch->CCR |= (((uint16_t)config->transfer_size - 1) << DMA_CCR_MSIZE_Pos);

    if(config->dma == DMA1 && (config->ch_conf != NULL)){
        DMA1_CSELR->CSELR &= ~(0xF << ((config->ch_conf->channel_number -1)*4));
        DMA1_CSELR->CSELR |= (config->ch_conf->request_num) << ((config->ch_conf->channel_number   -1)*4);
    }
    else if(config->dma == DMA1 && (config->ch_conf != NULL)){
        DMA2_CSELR->CSELR &= ~(0xF << ((config->ch_conf->channel_number -1)*4));
        DMA2_CSELR->CSELR |= (config->ch_conf->request_num) << ((config->ch_conf->channel_number -1)*4);
    }
    
    DMA2_CSELR->CSELR &= ~(DMA_CSELR_C3S_Msk);
    DMA2_CSELR->CSELR |= (0b0010 << DMA_CSELR_C3S_Pos);    
    if(config->transfer_type == MEM_TO_MEM)
    {
        dma_ch->CCR |= (DMA_CCR_PINC | DMA_CCR_MINC);
        dma_ch->CCR &= ~(DMA_CCR_DIR);
        dma_ch->CCR |= DMA_CCR_MEM2MEM;
        dma_ch->CPAR = config->src_address;
        dma_ch->CMAR = config->dest_address;
    } 
    else if(config->transfer_type == PERIPH_TO_MEM)
    {   
        dma_ch->CCR &= ~(DMA_CCR_PINC);
        dma_ch->CCR |=  DMA_CCR_MINC;
        dma_ch->CCR &= ~(DMA_CCR_DIR);
        dma_ch->CCR &= ~DMA_CCR_MEM2MEM;
        dma_ch->CPAR = config->src_address;
        dma_ch->CMAR = config->dest_address;
    }
    else if(config->transfer_type == MEM_TO_PERIPH){

        dma_ch->CCR &= ~(DMA_CCR_PINC);
        dma_ch->CCR |=  DMA_CCR_MINC;
        dma_ch->CCR |= (DMA_CCR_DIR);
        dma_ch->CCR &= ~DMA_CCR_MEM2MEM;
        dma_ch->CPAR = config->dest_address;
        dma_ch->CMAR = config->src_address;
    }
    else{

    }
    NVIC_EnableIRQ(config->irq);
    NVIC_SetPriority(config->irq,config->irq_priority);
}


void dma_start(dma_config_t *config)
{
    printf("DMA > Started\n");
    dma_init(config);
    enable_dma_channel(config->dma_ch);
}


void dma1_channel1_handler( )
{
    if(DMA1->ISR & DMA_ISR_TCIF1){
        printf("DMA1> TCI\n");
    }
    if(DMA1->ISR & DMA_ISR_HTIF1){
        printf("DMA1> HTI\n");
    }
    if(DMA1->ISR & DMA_ISR_TEIF1){
        printf("DMA1> TEI\n");
    }
    
    DMA1->IFCR = DMA_IFCR_CGIF1;
    printf("DMA1> Cleared all INTs\n");

}


void dma2_channel3_handler( )
{
    if(DMA2->ISR & DMA_ISR_TCIF3){
        UART4->CR3 &= ~(USART_CR3_DMAT);
        UART4->ICR |= USART_ICR_TCCF;
        printf("DMA2> TCI\n");
    }
    if(DMA2->ISR & DMA_ISR_HTIF3){
        printf("DMA2> HTI\n");
    }
    if(DMA2->ISR & DMA_ISR_TEIF3){
        printf("DMA2> TEI\n");
    }

    DMA2->IFCR = DMA_IFCR_CGIF3;
    printf("DMA2> Cleared all INTs\n");

}


/*void mem2mem_dma_init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    DMA1_Channel1->CPAR = (uint32_t)memoryA;
    DMA1_Channel1->CMAR = (uint32_t)memoryB;
    DMA1_Channel1->CNDTR = sizeof(memoryA);               // number of bytes
    DMA1_Channel1->CCR |= (DMA_CCR_MEM2MEM);
    DMA1_Channel1->CCR |= (DMA_CCR_PL);
    DMA1_Channel1->CCR &= ~(DMA_CCR_MSIZE);
    DMA1_Channel1->CCR &= ~(DMA_CCR_PSIZE);
    DMA1_Channel1->CCR |= DMA_CCR_MINC;
    DMA1_Channel1->CCR |= DMA_CCR_PINC;
    DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
    DMA1_Channel1->CCR |= (DMA_CCR_TCIE  | DMA_CCR_TEIE);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_SetPriority(DMA1_Channel1_IRQn,3);
}

void m2p_dma_init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    DMA2_Channel3->CMAR = (uint32_t)ALPHABET;
    DMA2_Channel3->CPAR = (uint32_t)(&UART4->TDR);
    DMA2_Channel3->CNDTR = sizeof(ALPHABET);
    DMA2_Channel3->CCR |= (DMA_CCR_TCIE | DMA_CCR_TEIE);
    DMA2_Channel3->CCR |= DMA_CCR_DIR; // M2P
    DMA2_Channel3->CCR &= ~(DMA_CCR_PINC);
    DMA2_Channel3->CCR |= (DMA_CCR_MINC);
    DMA2_Channel3->CCR &= ~(DMA_CCR_MSIZE);
    DMA2_Channel3->CCR &= ~(DMA_CCR_PSIZE);
    DMA2_Channel3->CCR &= ~(DMA_CCR_PL);
    DMA2_Channel3->CCR &= ~(DMA_CCR_MEM2MEM);
    DMA2_CSELR->CSELR &= ~(DMA_CSELR_C3S_Msk);
    DMA2_CSELR->CSELR |= (0b0010 << DMA_CSELR_C3S_Pos);
    NVIC_EnableIRQ(DMA2_Channel3_IRQn);
    NVIC_SetPriority(DMA2_Channel3_IRQn,3);

}

void mem2mem_dma_start(bool first_time)
{
    if(first_time)
    {
        printf("DMA> Starting Transfer\n");
        DMA1_Channel1->CCR |= (DMA_CCR_EN);
    }
    else{
        printf("DMA>Restarting\n");
        DMA1_Channel1->CCR &= ~(DMA_CCR_EN);
        DMA1_Channel1->CNDTR = sizeof(memoryA);  
        DMA1_Channel1->CCR |= (DMA_CCR_EN);
        
    }
}


void m2p_dma_start(bool first_time)
{
    if(first_time)
    {
        printf("DMA2> Starting Transfer\n");
        DMA2_Channel3->CCR |= (DMA_CCR_EN);
        UART4->CR3 |= USART_CR3_DMAT;
    }
    else{
        printf("DMA2>Restarting\n");
        DMA2_Channel3->CCR &= ~(DMA_CCR_EN);
        DMA2_Channel3->CNDTR = sizeof(ALPHABET);  
        DMA2_Channel3->CCR |= (DMA_CCR_EN);
        UART4->CR3 |= USART_CR3_DMAT;

        
    }
}*/



