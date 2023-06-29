/*****************************************************************************
 *
 * @file 	bee_UART.c
 * @author 	biennd
 * @date 	13 Jun 2023
 * @brief	All functions relate to config output GPIO
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#ifndef OUTPUT_IO_H
#define OUTPUT_IO_H
#include "esp_err.h"
#include "hal/gpio_types.h"

/**********************Create GPIO, set output GPIO**************************/
void output_io_create(gpio_num_t gpio_num);
/**********************set level GPIO**************************/
void output_io_set_level(gpio_num_t gpio_num, int level);
/*******************Toggle level GPIO**********************/
void output_io_toggle(gpio_num_t gpio_num);
#endif
