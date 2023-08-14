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

#include "driver/uart.h"

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

//define cmd wifi prov
#define Id_wifi_prov          0x05
#define LoByteData_wifi_prov  0x11

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/**
 * @brief Initialize the UART communication.
 *
 * This function initializes the UART communication with the specified configuration parameters.
 * It sets up the UART communication settings such as baud rate, data bits, parity, stop bits, and flow control.
 *
 * @note Make sure to configure the UART pins (TX and RX) before calling this function.
 */
void uart_init(void);

/**
 * @brief Calculate a checksum for the given data.
 *
 * This function calculates a simple checksum for the provided data by summing up all the bytes and taking the modulo
 * 256 of the sum. The resulting checksum is returned.
 *
 * @param data - Pointer to the data for which the checksum needs to be calculated.
 * @param length - Length of the data in bytes.
 * @return The calculated checksum value.
 */
char calculate_checksum(const char *data, size_t length);

/**
 * @brief Task to send DHT11 temperature and humidity data over UART at specified intervals.
 *
 * This task reads the DHT11 temperature and humidity data and sends it over UART at regular intervals.
 * It first converts the temperature and humidity data to hexadecimal format and prepares the UART data packets.
 * The checksum for each packet is calculated using the provided function `calculate_checksum`.
 * The packets are then sent over UART using the `uart_write_bytes` function.
 * The task runs periodically based on the interval specified by `u8data_interval_uart`.
 */
void uart_send_data_task(void* pvParameters);

/**
 * @brief Task to process UART command events and execute corresponding actions.
 *
 * This task receives UART command events from the queue `uart_cmd_queue` and processes the received commands.
 * It reads the UART data, calculates the checksum, and checks the header, version, and checksum to validate the data.
 * Depending on the received command, it performs specific actions such as toggling data sending, changing data interval,
 * starting Wi-Fi provisioning, and publishing data to a host.
 */
void uart_cmd_task(void* pvParameters);

#endif /* BEE_UART_H_ */
