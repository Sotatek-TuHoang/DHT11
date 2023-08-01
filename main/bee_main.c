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
#include "bee_dht11.h"
#include "bee_led_rgb.h"
#include "bee_nvs.h"
#include "bee_uart.h"
#include "bee_wifi.h"
#include "bee_button.h"
#include "bee_mqtt.h"

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
bool bsend_data = true; // Flag control on/off upload data throught uart
uint8_t u8data_interval_uart = 2; // Deffault interval upload data uart
uint8_t u8data_interval_mqtt = 30; // Deffault interval upload data mqtt

/****************************************************************************/
/***        List of Tasks                                                 ***/
/****************************************************************************/
void read_dht11(void* pvParameters);
void led_control_task(void* pvParameters);
void uart_send_data_task(void* pvParameters);
void uart_cmd_task(void* pvParameters);
void send_data_button(void* arg);
void interval_button(void* arg);
void send_mqtt_data_task(void* pvParameters);
void receive_mqtt_config_task(void* pvParameters);
/****************************************************************************/

void app_main()
{
    nvs_flash_function_init();

    load_data_from_nvs(&bsend_data, &u8data_interval_uart, &u8data_interval_mqtt); // Read status from NVS

    DHT11_init(DHT11_PIN); //Initialize DHT11

    led_rgb_init(LED_TEMP_RED_PIN, LED_TEMP_GREEN_PIN, LED_TEMP_BLUE_PIN);
    led_rgb_init(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN);

    uart_init(); //Initialize uart
    
    button_init();

    xTaskCreate(&read_dht11, "read_dht11", 2048, NULL, 31, NULL);
    xTaskCreate(&uart_send_data_task, "uart_send_data_task", 2048, NULL, 8, NULL);
    xTaskCreate(&led_control_task, "led_control_task", 1024, NULL, 6, NULL);
    xTaskCreate(&uart_cmd_task, "uart_cmd_task", 4096, NULL, 4, NULL);
    xTaskCreate(interval_button, "interval_button", 2048, NULL, 3, NULL);
    xTaskCreate(send_data_button, "send_data_button", 2048, NULL, 2, NULL);
    
    wifi_init_func();

    mqtt_app_start();

    xTaskCreate(send_mqtt_data_task, "send_mqtt_data_task", 2048, NULL, 7, NULL);
    xTaskCreate(receive_mqtt_config_task, "receive_mqtt_config_task", 2048, NULL, 5, NULL);

}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/