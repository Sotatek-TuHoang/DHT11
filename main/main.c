/*****************************************************************************
 *
 * @file 	main.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project read signal DHT11, upload data to host main,
 *          receive command from host main and button.
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <inttypes.h>
#include "esp_system.h"
#include "dht11.h"
#include "led_rgb.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_wifi.h"
#include <string.h>
#include "nvs_function.h"
#include "uart_function.h"
#include "wifi_function.h"
#include "button.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DHT11_PIN               GPIO_NUM_4

#define LED_TEMPE_RED_PIN       GPIO_NUM_12
#define LED_TEMPE_GREEN_PIN     GPIO_NUM_13
#define LED_TEMPE_BLUE_PIN      GPIO_NUM_14

#define LED_HUMI_RED_PIN        GPIO_NUM_25
#define LED_HUMI_GREEN_PIN      GPIO_NUM_26
#define LED_HUMI_BLUE_PIN       GPIO_NUM_27

#define H_tempe_threshold       30
#define L_tempe_threshold       25
#define H_humi_threshold        80
#define L_humi_threshold        60

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
bool        bsend_data = true; // Flag control on/off upload data throught uart
uint32_t    u32data_interval = 2000; // Deffault interval upload data

/****************************************************************************/
/***        Handle                                                        ***/
/****************************************************************************/
QueueHandle_t   send_data_evt_queue = NULL;
QueueHandle_t   interval_evt_queue = NULL;
QueueHandle_t   uart_queue;
TaskHandle_t    uart_send_data_task_handle;

/****************************************************************************/
/***        List of Function                                              ***/
/****************************************************************************/

void led_control_task(void* parameter);
void uart_send_data_task(void* parameter);
void uart_cmd_task(void* parameter);
static void send_data_button(void* arg);
static void interval_button(void* arg);
/****************************************************************************/

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

void app_main()
{
    nvs_flash_init(); // Initialize NVS

    load_data_from_nvs(&bsend_data, &u32data_interval); // Read status from NVS

    wifi_init_sta();

    DHT11_init(DHT11_PIN); //Initialize DHT11

    led_rgb_init(LED_TEMPE_RED_PIN, LED_TEMPE_GREEN_PIN, LED_TEMPE_BLUE_PIN);
    led_rgb_init(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN);

    uart_init(); //Initialize uart
    
    button_init();

    send_data_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    interval_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(&led_control_task, "led_control_task", 2048, NULL, 3, NULL);
    xTaskCreate(&uart_send_data_task, "uart_send_data_task", 2048, NULL, 2, &uart_send_data_task_handle);
    xTaskCreate(&uart_cmd_task, "uart_cmd_task", 2048, NULL, 1, NULL);
    xTaskCreate(send_data_button, "send_data_button", 2048, NULL, 9, NULL);
    xTaskCreate(interval_button, "interval_button", 2048, NULL, 10, NULL);
    for (;;)
    {
        struct dht11_reading reading = DHT11_read(); //Read status from DHT11 and print to serial monitor
        if (reading.status == DHT11_OK)
        {
            printf("Temperature: %u C\nHumidity: %u%%\n", reading.temperature, reading.humidity);
            printf("Data send: %s\n", bsend_data ? "Enabled" : "Disabled");
            printf("Interval: %lu\n", u32data_interval);
        }
        else
        {
            printf("Error reading from DHT11. Status: %u\n", reading.status);
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

/****************************************************************************/
/***       Function                                                       ***/
/****************************************************************************/

/** @brief Button control upload data to Host
 */
static void send_data_button(void* arg)
{
    uint32_t gpio_num;
    for (;;) {
        if (xQueueReceive(send_data_evt_queue, &gpio_num, portMAX_DELAY))
        {
            bsend_data = !bsend_data;
            save_data_to_nvs(bsend_data, u32data_interval);
        }
    }
}

/** @brief Button control upload interval to Host
 */
static void interval_button(void* arg)
{
    uint32_t gpio_num;
    for (;;) {
        if (xQueueReceive(interval_evt_queue, &gpio_num, portMAX_DELAY))
        {
            if (u32data_interval == 2000)
            {
                u32data_interval = 5000;  // Change to every 5 secends
            }
            else if (u32data_interval == 5000)
            {
                u32data_interval = 10000;  // Change to every 5 secends
            }
            else if (u32data_interval == 10000)
            {
                u32data_interval = 2000;  // Change to every 10 secends
            }
            else {}
            save_data_to_nvs(bsend_data, u32data_interval);
        }
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
            }  
        }
    }      
    free(uart_data_recei);
    vTaskDelete(NULL); 
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
        struct dht11_reading reading = DHT11_read();
        if (reading.status == DHT11_OK && bsend_data)
        { 
            snprintf(tempe_hex, sizeof(tempe_hex), "%02X", reading.temperature);
            uart_data_tempe[7] = LoByteData_Tempe = strtol(tempe_hex, NULL, 16);
            uart_data_tempe[8] = Chs_Tempe = calculate_checksum((char*)uart_data_tempe, 8);
            uart_write_bytes(UART_NUM, uart_data_tempe, sizeof(uart_data_tempe));

            snprintf(humi_hex, sizeof(humi_hex), "%02X", reading.humidity);
            uart_data_humi[7] = LoByteData_Humi = strtol(humi_hex, NULL, 16);
            uart_data_humi[8] = Chs_Humi = calculate_checksum((char*)uart_data_humi, 8);
            uart_write_bytes(UART_NUM, uart_data_humi, sizeof(uart_data_humi));
        }
        vTaskDelay(u32data_interval / portTICK_PERIOD_MS);
    }
}

/** @brief warning by led rgb
 */
void led_control_task(void* parameter)
{
    for (;;)
    {
        struct dht11_reading reading = DHT11_read();
        if (reading.status == DHT11_OK)
        {
            // Warning temperature
            if (reading.temperature > H_tempe_threshold) // LED Temp turning red if high temperature
            {
                led_rgb_set_color(LED_TEMPE_RED_PIN, LED_TEMPE_GREEN_PIN, LED_TEMPE_BLUE_PIN, 1, 0, 0);
            }
            else if (reading.temperature < L_tempe_threshold) // LED Temp turning white if high temperature
            {
                led_rgb_set_color(LED_TEMPE_RED_PIN, LED_TEMPE_GREEN_PIN, LED_TEMPE_BLUE_PIN, 1, 1, 1);
            }
            else //// LED Temp turning green if good temperature
            {
                led_rgb_set_color(LED_TEMPE_RED_PIN, LED_TEMPE_GREEN_PIN, LED_TEMPE_BLUE_PIN, 0, 1, 0);
            }
            // Warning humidity
            if (reading.humidity > H_humi_threshold) // LED HUmi turning blue when high humidity
            {
                led_rgb_set_color(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN, 0, 0, 1);
            }
            else if (reading.humidity < L_humi_threshold) // LED HUmi turning yellow when high humidity
            {
                led_rgb_set_color(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN, 1, 1, 0);
            }
            else // LED Humi turning green when good humidity
            {
                led_rgb_set_color(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN, 0, 1, 0);
            }
        }
        else // both led turning off if cant read status form dht11
        {
            led_rgb_set_color(LED_TEMPE_RED_PIN, LED_TEMPE_GREEN_PIN, LED_TEMPE_BLUE_PIN, 0, 0, 0);
            led_rgb_set_color(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN, 0, 0, 0);
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/