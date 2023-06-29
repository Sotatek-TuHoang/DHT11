/*****************************************************************************
 *
 * @file 	bee_output_iot.c
 * @author 	biennd
 * @date 	13 Jun 2023
 * @brief	All functions relate to config output GPIO
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>
#include "bee_output_iot.h"


/**********************Create GPIO, set output GPIO**************************/
void output_io_create(gpio_num_t gpio_num)
{
    gpio_pad_select_gpio(gpio_num);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT_OUTPUT);
}

/**********************set level GPIO**************************/
void output_io_set_level(gpio_num_t gpio_num, int level)
{  
    gpio_set_level(gpio_num, level);
}

/*******************Toggle level GPIO**********************/
void output_io_toggle(gpio_num_t gpio_num)
{  
    int old_level = gpio_get_level(gpio_num);
    gpio_set_level(gpio_num, 1-old_level);
}

