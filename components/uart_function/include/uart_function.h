#ifndef UART_FUNCTION_H_
#define UART_FUNCTION_H_

#define UART_NUM                UART_NUM_2
#define UART_TX_PIN             GPIO_NUM_17
#define UART_RX_PIN             GPIO_NUM_16
#define UART_BUFFER_SIZE        1024
#define RD_BUF_SIZE             UART_BUFFER_SIZE

#include "driver/uart.h"

void uart_init();

#endif /* UART_FUNCTION_H_ */
