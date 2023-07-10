/*****************************************************************************
 *
 * @file 	uart_function.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project control uart
 *
 ***************************************************************************/
/****************************************************************************/
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <string.h>

#include "uart_function.h"
#include "bee_nvs.h"
#include "dht11.h"

extern QueueHandle_t uart_queue;
extern uint8_t     u8status;
extern uint8_t     u8temp;
extern uint8_t     u8humi;
extern bool        bsend_data;
extern uint32_t    u32data_interval;

void uart_init()
{
    uart_config_t uart_config =
    {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 2, &uart_queue, 0);
}

/** @brief Caculate Checksum
 */
char calculate_checksum(const char *data, size_t length) {
    uint8_t u8checksum = 0;
    for (size_t i = 0; i < length; i++)
    {
        u8checksum += data[i];
    }
    u8checksum %= 0x100;
    return u8checksum;
}

/** @brief Send data to Host main throught UART
 */
void uart_send_data_task(void* parameter)
{
    char tempe_hex[3];
    uint8_t HeaderHiByte_Tempe  = 0x55;
    uint8_t HeaderLoByte_Tempe  = 0xAA;
    uint8_t Ver_Tempe           = 0x01;
    uint8_t Id_Tempe            = 0x01;
    uint8_t LeHiByteData_Tempe  = 0x00;
    uint8_t LeLoByteData_Tempe  = 0x01;
    uint8_t HiByteData_Tempe    = 0x00;
    uint8_t LoByteData_Tempe    = 0x00;
    uint8_t Chs_Tempe           = 0x00;

    char humi_hex[3];
    uint8_t HeaderHiByte_Humi   = 0x55;
    uint8_t HeaderLoByte_Humi   = 0xAA;
    uint8_t Ver_Humi            = 0x01;
    uint8_t Id_Humi             = 0x02;
    uint8_t LeHiByteData_Humi   = 0x00;
    uint8_t LeLoByteData_Humi   = 0x01;
    uint8_t HiByteData_Humi     = 0x00;
    uint8_t LoByteData_Humi     = 0x00;
    uint8_t Chs_Humi            = 0x00;

    char uart_data_tempe[9] = {HeaderHiByte_Tempe, HeaderLoByte_Tempe, Ver_Tempe, Id_Tempe,
     LeHiByteData_Tempe, LeLoByteData_Tempe, HiByteData_Tempe, LoByteData_Tempe, Chs_Tempe};
    char uart_data_humi[9] = {HeaderHiByte_Humi, HeaderLoByte_Humi, Ver_Humi, Id_Humi,
     LeHiByteData_Humi, LeLoByteData_Humi, HiByteData_Humi, LoByteData_Humi, Chs_Humi};
    for (;;)
    {
        if (u8status == DHT11_OK && bsend_data)
        { 
            snprintf(tempe_hex, sizeof(tempe_hex), "%02X", u8temp);
            uart_data_tempe[7] = LoByteData_Tempe = strtol(tempe_hex, NULL, 16);
            uart_data_tempe[8] = Chs_Tempe = calculate_checksum((char*)uart_data_tempe, 8);
            uart_write_bytes(UART_NUM, uart_data_tempe, sizeof(uart_data_tempe));

            snprintf(humi_hex, sizeof(humi_hex), "%02X", u8humi);
            uart_data_humi[7] = LoByteData_Humi = strtol(humi_hex, NULL, 16);
            uart_data_humi[8] = Chs_Humi = calculate_checksum((char*)uart_data_humi, 8);
            uart_write_bytes(UART_NUM, uart_data_humi, sizeof(uart_data_humi));
        }
        vTaskDelay(u32data_interval / portTICK_PERIOD_MS);
    }
}

/** @brief Control module from Host main througt UART
 */
void uart_cmd_task(void* parameter)
{   
    uart_event_t event;
    char* uart_data_recei = (char*)malloc(9 * RD_BUF_SIZE);
    uint8_t HeaderHiByte_cmd        = 0x55;
    uint8_t HeaderLoByte_cmd        = 0xAA;
    uint8_t Ver_cmd                 = 0x02;
    uint8_t Id_SendData             = 0x03;
    uint8_t LoByteData_SendData     = 0x01;
    uint8_t Id_interval             = 0x04;
    uint8_t LoByteData_interval_2s  = 0x02;
    uint8_t LoByteData_interval_5s  = 0x05;
    uint8_t LoByteData_interval_10s = 0x10; 
    for (;;)
    {
        if (xQueueReceive(uart_queue, (void*)&event, (TickType_t)portMAX_DELAY))
        {
            bzero(uart_data_recei, RD_BUF_SIZE);
            uart_read_bytes(UART_NUM_2, uart_data_recei, event.size,portMAX_DELAY);
            if ((uart_data_recei[0] == HeaderHiByte_cmd)
                && (uart_data_recei[1] == HeaderLoByte_cmd)
                && (uart_data_recei [2] == Ver_cmd))
            {
                uint32_t tem_checksum = calculate_checksum(uart_data_recei, 8);
                if (tem_checksum == uart_data_recei[8])
                {     
                    if ((uart_data_recei[3] == Id_SendData) && (uart_data_recei[7] == LoByteData_SendData))
                    {
                        bsend_data = !bsend_data;
                        save_data_to_nvs(bsend_data, u32data_interval);
                    }
                    else if ((uart_data_recei[3] == Id_interval) && (uart_data_recei[7] == LoByteData_interval_2s))
                    {
                        u32data_interval = 2000;
                        save_data_to_nvs(bsend_data, u32data_interval);
                    }
                    else if ((uart_data_recei[3] == Id_interval) && (uart_data_recei[7] == LoByteData_interval_5s))
                    {
                        u32data_interval = 5000;
                        save_data_to_nvs(bsend_data, u32data_interval);
                    }
                    else if ((uart_data_recei[3] == Id_interval) && (uart_data_recei[7] == LoByteData_interval_10s))
                    {
                        u32data_interval = 10000;
                        save_data_to_nvs(bsend_data, u32data_interval);
                    }
                }
                else 
                {
                    printf("wrong checksum\n");
                }
            }  
        }
    }      
    free(uart_data_recei);
    vTaskDelete(NULL); 
}
