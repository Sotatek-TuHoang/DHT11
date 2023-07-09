/*****************************************************************************
 *
 * @file 	dht11.h
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project read signal DHT11, Init, macro define pin
 *
 ***************************************************************************/

/****************************************************************************/
#ifndef LED_RGB_H
#define LED_RGB_H

#define LED_TEMPE_RED_PIN       GPIO_NUM_12
#define LED_TEMPE_GREEN_PIN     GPIO_NUM_13
#define LED_TEMPE_BLUE_PIN      GPIO_NUM_14

#define LED_HUMI_RED_PIN        GPIO_NUM_25
#define LED_HUMI_GREEN_PIN      GPIO_NUM_26
#define LED_HUMI_BLUE_PIN       GPIO_NUM_27

#include "driver/gpio.h"


void led_rgb_init(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin);
void led_rgb_set_color(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin, bool red, bool green, bool blue);

#endif /* LED_RGB_H */