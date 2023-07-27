/*****************************************************************************
 *
 * @file 	bee_uart.c
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
#include "esp_log.h"

#include "bee_uart.h"
#include "bee_nvs.h"
#include "bee_dht11.h"
#include "bee_mqtt.h"
#include "bee_wifi.h"

static const char *TAG = "UART";

QueueHandle_t  uart_cmd_queue;
extern uint8_t u8status;
extern uint8_t u8temp;
extern uint8_t u8humi;
extern bool    bsend_data;
extern uint8_t u8data_interval_uart;

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
    uart_driver_install(UART_NUM, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 6, &uart_cmd_queue, 0);
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
void uart_send_data_task(void* pvParameters)
{
    TickType_t last_time_send = xTaskGetTickCount();
    TickType_t interval_send_uart = pdMS_TO_TICKS(u8data_interval_uart * 1000);

    char temp_hex[3];
    uint8_t LoByteData_Temp    = 0x00;
    uint8_t Chs_Temp           = 0x00;

    char humi_hex[3];
    uint8_t LoByteData_Humi     = 0x00;
    uint8_t Chs_Humi            = 0x00;

    char uart_data_temp[9] = {HeaderHiByte_Temp, HeaderLoByte_Temp, Ver_Temp, Id_Temp,
     LeHiByteData_Temp, LeLoByteData_Temp, HiByteData_Temp, LoByteData_Temp, Chs_Temp};
    char uart_data_humi[9] = {HeaderHiByte_Humi, HeaderLoByte_Humi, Ver_Humi, Id_Humi,
     LeHiByteData_Humi, LeLoByteData_Humi, HiByteData_Humi, LoByteData_Humi, Chs_Humi};
    for (;;)
    {
        interval_send_uart = pdMS_TO_TICKS(u8data_interval_uart * 1000);
        if ((xTaskGetTickCount() - last_time_send) >= interval_send_uart)
        {
            last_time_send = xTaskGetTickCount();
            if (u8status == DHT11_OK && bsend_data)
            { 
                snprintf(temp_hex, sizeof(temp_hex), "%02X", u8temp);
                uart_data_temp[7] = LoByteData_Temp = strtol(temp_hex, NULL, 16);
                uart_data_temp[8] = Chs_Temp = calculate_checksum((char*)uart_data_temp, 8);
                uart_write_bytes(UART_NUM, uart_data_temp, sizeof(uart_data_temp));

                snprintf(humi_hex, sizeof(humi_hex), "%02X", u8humi);
                uart_data_humi[7] = LoByteData_Humi = strtol(humi_hex, NULL, 16);
                uart_data_humi[8] = Chs_Humi = calculate_checksum((char*)uart_data_humi, 8);
                uart_write_bytes(UART_NUM, uart_data_humi, sizeof(uart_data_humi));
            }
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

/** @brief Control module from Host main througt UART
 */
void uart_cmd_task(void* pvParameters)
{   
    uart_event_t event;
    char* uart_data_recei = (char*)malloc(9 * RD_BUF_SIZE);
    for (;;)
    {
        if (xQueueReceive(uart_cmd_queue, (void*)&event, (TickType_t)portMAX_DELAY))
        {
            bzero(uart_data_recei, RD_BUF_SIZE);
            uart_read_bytes(UART_NUM_2, uart_data_recei, event.size,portMAX_DELAY);
            uint8_t tem_checksum = calculate_checksum(uart_data_recei, 8);
            if ((uart_data_recei[0] == HeaderHiByte_cmd)
                && (uart_data_recei[1] == HeaderLoByte_cmd)
                && (uart_data_recei [2] == Ver_cmd)
                && (tem_checksum == uart_data_recei[8]))
            {     
                if ((uart_data_recei[3] == Id_SendData) && (uart_data_recei[7] == LoByteData_SendData))
                {
                    bsend_data = !bsend_data;
                    ESP_LOGI (TAG, "Send data to host main: %d", bsend_data);
                    save_uart_data_to_nvs(bsend_data, u8data_interval_uart);
                    send_data_mqtt("Id_SendData", bsend_data);
                }
                else if ((uart_data_recei[3] == Id_interval) && (uart_data_recei[7] == LoByteData_interval_2s))
                {
                    u8data_interval_uart = 2;
                    ESP_LOGI (TAG, "Interval send data to host main: %d", u8data_interval_uart);
                    send_data_mqtt("Id_interval", LoByteData_interval_2s);
                    save_uart_data_to_nvs(bsend_data, u8data_interval_uart);
                }
                else if ((uart_data_recei[3] == Id_interval) && (uart_data_recei[7] == LoByteData_interval_5s))
                {
                    u8data_interval_uart = 5;
                    ESP_LOGI (TAG, "Interval send data to host main: %d", u8data_interval_uart);
                    send_data_mqtt("Id_interval", LoByteData_interval_5s);
                    save_uart_data_to_nvs(bsend_data, u8data_interval_uart);
                }
                else if ((uart_data_recei[3] == Id_interval) && (uart_data_recei[7] == LoByteData_interval_10s))
                {
                    u8data_interval_uart = 10;
                    ESP_LOGI (TAG, "Interval send data to host main: %d", u8data_interval_uart);
                    send_data_mqtt("Id_interval", LoByteData_interval_10s);
                    save_uart_data_to_nvs(bsend_data, u8data_interval_uart);
                }
                else if ((uart_data_recei[3] == Id_wifi_reprov) && (uart_data_recei[7] == LoByteData_wifi_reprov))
                {
                    ESP_LOGI (TAG, "Start wifi reprov: %d", LoByteData_wifi_reprov);
                    send_data_mqtt("Id_wifi_reprov", LoByteData_wifi_reprov);
                    wifi_reprov();
                }
            }  
        }
    }      
    free(uart_data_recei);
    vTaskDelete(NULL); 
}


