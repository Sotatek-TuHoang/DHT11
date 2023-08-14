/*****************************************************************************
 *
 * @file     main.c
 * @author   tuha
 * @date     3 July 2023
 * @brief    Module for project reading data from SHT3x sensor and performing various tasks.
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
/***        Exported variables                                            ***/
/****************************************************************************/
bool bsend_data = true; // Flag control on/off upload data through UART
uint8_t u8data_interval_uart = 2; // Default interval to upload data via UART
uint8_t u8data_interval_mqtt = 30; // Default interval to upload data via MQTT

/****************************************************************************/

void app_main()
{
    nvs_flash_func_init();

    load_data_from_nvs(&bsend_data, &u8data_interval_uart, &u8data_interval_mqtt);

    DHT11_init(DHT11_PIN);

    led_rgb_init(LED_TEMP_RED_PIN, LED_TEMP_GREEN_PIN, LED_TEMP_BLUE_PIN); //init temperatute led
    led_rgb_init(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN); //init humidity led

    uart_init();
    
    button_init();

    xTaskCreate(read_dht11_task, "read_dht11", 2048, NULL, 31, NULL);
    xTaskCreate(uart_send_data_task, "uart_send_data_task", 4096, NULL, 8, NULL);
    xTaskCreate(led_control_task, "led_control_task", 1024, NULL, 6, NULL);
    xTaskCreate(uart_cmd_task, "uart_cmd_task", 4096, NULL, 4, NULL);
    xTaskCreate(interval_button_isr, "interval_button", 2048, NULL, 3, NULL);
    xTaskCreate(send_data_button_isr, "send_data_button", 2048, NULL, 2, NULL);

    wifi_func_init();

    mqtt_func_init();

    xTaskCreate(send_mqtt_data_task, "send_mqtt_data_task", 4096, NULL, 7, NULL);
    xTaskCreate(receive_mqtt_config_task, "receive_mqtt_config_task", 4096, NULL, 5, NULL);

}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
