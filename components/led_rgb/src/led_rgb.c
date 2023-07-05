/*****************************************************************************
 *
 * @file 	dht11.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project control led rgb
 *
 ***************************************************************************/

/****************************************************************************/
#include "led_rgb.h"
#include "driver/gpio.h"

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
