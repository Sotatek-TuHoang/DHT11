/*****************************************************************************
 *
 * @file 	bee_uart.h
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project control uart
 *
 ***************************************************************************/
/****************************************************************************/
#ifndef BEE_UART_H_
#define BEE_UART_H_

#define UART_NUM                UART_NUM_2
#define UART_TX_PIN             GPIO_NUM_17
#define UART_RX_PIN             GPIO_NUM_16
#define UART_BUFFER_SIZE        2048
#define RD_BUF_SIZE             UART_BUFFER_SIZE

//define data temp
#define HeaderHiByte_Temp   0x55
#define HeaderLoByte_Temp   0xAA
#define Ver_Temp            0x01
#define Id_Temp             0x01
#define LeHiByteData_Temp   0x00
#define LeLoByteData_Temp   0x01
#define HiByteData_Temp     0x00

//define data humi
#define HeaderHiByte_Humi    0x55
#define HeaderLoByte_Humi    0xAA
#define Ver_Humi             0x01
#define Id_Humi              0x02
#define LeHiByteData_Humi    0x00
#define LeLoByteData_Humi    0x01
#define HiByteData_Humi      0x00

//define cmd uart
#define HeaderHiByte_cmd         0x55
#define HeaderLoByte_cmd         0xAA
#define Ver_cmd                  0x02
#define Id_SendData              0x03
#define LoByteData_SendData      0x01
#define Id_interval              0x04
#define LoByteData_interval_2s   0x02
#define LoByteData_interval_5s   0x05
#define LoByteData_interval_10s  0x10

//define cmd wifi reprov
#define Id_wifi_reprov          0x05
#define LoByteData_wifi_reprov  0x11

#include "driver/uart.h"

void uart_init();
char calculate_checksum(const char *data, size_t length);
void uart_send_data_task(void* pvParameters);
void uart_cmd_task(void* pvParameters);
#endif /* BEE_UART_H_ */
