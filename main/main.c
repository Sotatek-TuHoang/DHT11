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
#include <string.h>
#include "esp_system.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_wifi.h"

#include "dht11.h"
#include "bee_led_rgb.h"
#include "bee_nvs.h"
#include "bee_uart.h"
#include "bee_wifi.h"
#include "bee_button.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
#define DHT11_PIN               GPIO_NUM_4

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
bool        bsend_data = true; // Flag control on/off upload data throught uart
uint32_t    u32data_interval = 2000; // Deffault interval upload data
struct dht11_reading reading;
uint8_t     u8status;
uint8_t     u8temp;
uint8_t     u8humi;

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
static void read_dht11(void* parameter);
void led_control_task(void* parameter);
void uart_send_data_task(void* parameter);
void uart_cmd_task(void* parameter);
static void send_data_button(void* arg);
static void interval_button(void* arg);
/****************************************************************************/

void app_main()
{
    nvs_flash_function_init();

    load_data_from_nvs(&bsend_data, &u32data_interval); // Read status from NVS

    wifi_init_sta();

    DHT11_init(DHT11_PIN); //Initialize DHT11

    led_rgb_init(LED_TEMPE_RED_PIN, LED_TEMPE_GREEN_PIN, LED_TEMPE_BLUE_PIN);
    led_rgb_init(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN);

    uart_init(); //Initialize uart
    
    button_init();

    send_data_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    interval_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(&read_dht11, "read_dht11", 2048, NULL, 1, NULL);
    xTaskCreate(&led_control_task, "led_control_task", 2048, NULL, 2, NULL);
    xTaskCreate(&uart_send_data_task, "uart_send_data_task", 2048, NULL, 3, &uart_send_data_task_handle);
    xTaskCreate(&uart_cmd_task, "uart_cmd_task", 2048, NULL, 4, NULL);
    xTaskCreate(send_data_button, "send_data_button", 2048, NULL, 5, NULL);
    xTaskCreate(interval_button, "interval_button", 2048, NULL, 6, NULL);
}

/****************************************************************************/
/***       Function                                                       ***/
/****************************************************************************/

static void read_dht11(void* parameter)
{
    TickType_t last_time_read = xTaskGetTickCount();
    TickType_t interval_read = pdMS_TO_TICKS(2000);  // 2 giây

    for (;;) {
        if ((xTaskGetTickCount() - last_time_read) >= interval_read)
        {
            last_time_read = xTaskGetTickCount();

            reading = DHT11_read();
            u8temp = reading.temperature;
            u8humi = reading.humidity;
            u8status = reading.status;

            if (u8status == DHT11_OK)
            {
                printf("Temperature: %d C\nHumidity: %d%%\n", u8temp, u8humi);
                printf("Data send: %s\n", bsend_data ? "Enabled" : "Disabled");
                printf("Interval: %lu\n", u32data_interval);
            }
            else
            {
                printf("Error reading from DHT11: %d\n", u8status);
            }
        }   
        vTaskDelay(150 / portTICK_PERIOD_MS); 
    }
}

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

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/