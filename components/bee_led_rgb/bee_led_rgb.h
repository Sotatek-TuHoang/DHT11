/*****************************************************************************
 *
 * @file 	bee_led_rgb.h
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project read signal DHT11, Init, macro define pin
 *
 ***************************************************************************/

/****************************************************************************/
#ifndef BEE_LED_RGB_H
#define BEE_LED_RGB_H

#define LED_TEMP_RED_PIN       GPIO_NUM_12
#define LED_TEMP_GREEN_PIN     GPIO_NUM_13
#define LED_TEMP_BLUE_PIN      GPIO_NUM_14

#define LED_HUMI_RED_PIN       GPIO_NUM_25
#define LED_HUMI_GREEN_PIN     GPIO_NUM_26
#define LED_HUMI_BLUE_PIN      GPIO_NUM_27

#define H_temp_threshold       30
#define L_temp_threshold       20
#define H_humi_threshold       80
#define L_humi_threshold       40

#include "driver/gpio.h"


void led_rgb_init(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin);
void led_rgb_set_color(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin, bool red, bool green, bool blue);

#endif /* BEE_LED_RGB_H */