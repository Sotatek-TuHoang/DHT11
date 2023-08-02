/*****************************************************************************
 *
 * @file 	dht11.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project control led rgb
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "bee_led_rgb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bee_dht11.h"

/****************************************************************************/
/***        Extern variables                                              ***/
/****************************************************************************/
extern uint8_t     u8status;
extern uint8_t     u8temp;
extern uint8_t     u8humi;

/****************************************************************************/
/***        Init Functions in App main                                    ***/
/****************************************************************************/
void led_rgb_init(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin) {
    esp_rom_gpio_pad_select_gpio(red_pin);
    gpio_set_direction(red_pin, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(green_pin);
    gpio_set_direction(green_pin, GPIO_MODE_OUTPUT);
    esp_rom_gpio_pad_select_gpio(blue_pin);
    gpio_set_direction(blue_pin, GPIO_MODE_OUTPUT);
}

void led_rgb_set_color(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin, bool red, bool green, bool blue) {
    gpio_set_level(red_pin, red);
    gpio_set_level(green_pin, green);
    gpio_set_level(blue_pin, blue);
}

/****************************************************************************/
/***        Led control task                                              ***/
/****************************************************************************/
void led_control_task(void* pvParameters)
{
    for (;;)
    {
        if (u8status == DHT11_OK)
        {
            // Warning temperature
            if (u8temp > H_temp_threshold) // LED Temp turning red if high temperature
            {
                led_rgb_set_color(LED_TEMP_RED_PIN, LED_TEMP_GREEN_PIN, LED_TEMP_BLUE_PIN, 1, 0, 0);
            }
            else if (u8temp < L_temp_threshold) // LED Temp turning white if high temperature
            {
                led_rgb_set_color(LED_TEMP_RED_PIN, LED_TEMP_GREEN_PIN, LED_TEMP_BLUE_PIN, 1, 1, 1);
            }
            else //// LED Temp turning green if good temperature
            {
                led_rgb_set_color(LED_TEMP_RED_PIN, LED_TEMP_GREEN_PIN, LED_TEMP_BLUE_PIN, 0, 1, 0);
            }
            // Warning humidity
            if (u8humi > H_humi_threshold) // LED HUmi turning blue when high humidity
            {
                led_rgb_set_color(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN, 0, 0, 1);
            }
            else if (u8humi < L_humi_threshold) // LED HUmi turning yellow when high humidity
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
            led_rgb_set_color(LED_TEMP_RED_PIN, LED_TEMP_GREEN_PIN, LED_TEMP_BLUE_PIN, 0, 0, 0);
            led_rgb_set_color(LED_HUMI_RED_PIN, LED_HUMI_GREEN_PIN, LED_HUMI_BLUE_PIN, 0, 0, 0);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/