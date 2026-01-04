#ifndef DMA_H
#define DMA_H

#include "common.h"



typedef enum{
    MEM_TO_MEM ,
    PERIPH_TO_MEM ,
    MEM_TO_PERIPH 
}dma_type_e;

typedef enum{
    DMA_PRIORITY_LOW,
    DMA_PRIORITY_MEDIUM,
    DMA_PRIORITY_HIGH,
    DMA_PRIORITY_VERY_HIGH
}dma_priority_e;

typedef struct{
    uint8_t channel_number;
    uint8_t request_num;
}ch_sel_conf_t;

typedef struct{
    DMA_TypeDef *dma;
    DMA_Channel_TypeDef *dma_ch;
    dma_type_e transfer_type;
    uint32_t src_address;
    uint32_t dest_address;
    uint16_t transfer_size; // Max 4 bytes
    uint16_t total_bytes;
    uint8_t priority;
    uint8_t irq;
    uint8_t irq_priority;
    ch_sel_conf_t *ch_conf; 
}dma_config_t;


void dma_init(dma_config_t *config);
void dma_start(dma_config_t *config);



#endif /* DMA_H */