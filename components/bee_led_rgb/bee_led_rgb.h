/*****************************************************************************
 *
 * @file 	bee_led_rgb.h
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project read signal DHT11, Init, macro define pin
 *
 ***************************************************************************/

#ifndef BEE_LED_RGB_H
#define BEE_LED_RGB_H

#include "driver/gpio.h"
/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

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

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/**
 * @brief Initialize RGB LED by configuring the GPIO pins.
 *
 * This function sets up the GPIO pins for controlling an RGB LED.
 *
 * @param red_pin   GPIO pin number for the red color channel.
 * @param green_pin GPIO pin number for the green color channel.
 * @param blue_pin  GPIO pin number for the blue color channel.
 */
void led_rgb_init(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin);

/**
 * @brief Set the color of an RGB LED by controlling individual color channels.
 *
 * This function sets the color of an RGB LED by controlling the individual color channels
 * (red, green, and blue) using the specified GPIO pins.
 *
 * @param red_pin   GPIO pin number for the red color channel.
 * @param green_pin GPIO pin number for the green color channel.
 * @param blue_pin  GPIO pin number for the blue color channel.
 * @param red       Boolean value indicating whether the red color channel should be on (true) or off (false).
 * @param green     Boolean value indicating whether the green color channel should be on (true) or off (false).
 * @param blue      Boolean value indicating whether the blue color channel should be on (true) or off (false).
 */
void led_rgb_set_color(gpio_num_t red_pin, gpio_num_t green_pin, gpio_num_t blue_pin, bool red, bool green, bool blue);

/**
 * @brief Task to control the RGB LEDs based on temperature and humidity readings.
 *
 * This task monitors the temperature and humidity readings obtained from the DHT11 sensor
 * and controls the RGB LEDs based on certain threshold values. The LEDs change color to indicate
 * different temperature and humidity conditions.
 *
 * @param pvParameters Pointer to the task parameters (not used in this function).
 */
void led_control_task(void* pvParameters);

#endif /* BEE_LED_RGB_H */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
