/*****************************************************************************
 *
 * @file 	uart_function.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project control uart
 *
 ***************************************************************************/
/****************************************************************************/
#ifndef UART_FUNCTION_H_
#define UART_FUNCTION_H_

#define UART_NUM                UART_NUM_2
#define UART_TX_PIN             GPIO_NUM_17
#define UART_RX_PIN             GPIO_NUM_16
#define UART_BUFFER_SIZE        1024
#define RD_BUF_SIZE             UART_BUFFER_SIZE

#include "driver/uart.h"

void uart_init();
char calculate_checksum(const char *data, size_t length);
void uart_send_data_task(void* parameter);
void uart_cmd_task(void* parameter);

#endif /* UART_FUNCTION_H_ */
