




#ifndef UART_H
#define UART_H

#include "common.h"

#define DEFAULT_CLI_UART (UART5)
#define DEFAULT_DEBUG_TOOL_UART (UART4)
typedef struct{
   uint8_t SOE;
   uint8_t header;
   uint8_t crc;
   uint8_t payload_len;
   uint8_t payload[];
}uart_packet_t;
#define NUM_SOE_BYTES       (1)
#define NUM_HEADER_BYTES    (1)
#define NUM_CRC_BYTES       (1)
#define NUM_DATA_LEN_BYTES  (1)
#define MAX_PAYLOAD_SIZE    (17)
#define MAX_UART_PKT_SIZE   (NUM_SOE_BYTES + NUM_HEADER_BYTES + NUM_CRC_BYTES + NUM_DATA_LEN_BYTES + NUM_HEADER_BYTES + MAX_PAYLOAD_SIZE)
void uart_send(USART_TypeDef *uart ,char *buffer);
void uart_init(USART_TypeDef *uart, uint32_t baudrate);
// void //uart_send_frame(USART_TypeDef *uart ,msg_t *msg);
void uart_send_data(USART_TypeDef *uart ,uint8_t *data , uint8_t len);
void uart_print_packet(uart_packet_t *pkt);
void uart_print_msg(uint8_t *buf,uint8_t buf_len);
// bool uart_recieve(uint8_t *byte);
void uart_write(USART_TypeDef *uart ,char ch);

#endif