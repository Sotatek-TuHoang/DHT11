/*****************************************************************************
 *
 * @file 	bee_input_iot.c
 * @author 	biennd
 * @date 	13 Jun 2023
 * @brief	All functions relate to config input GPIO
 *
 ***************************************************************************/
#ifndef INPUT_IO_H
#define INPUT_IO_H
/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "esp_err.h"
#include "hal/gpio_types.h"

// typedef void (*input_callback_t) (int);

typedef enum{
    LO_TO_HI = 1, 
    HI_TO_LO = 2,
    ANY_EDLE = 3
}   interrupt_type_edle_t;

void input_io_create(gpio_num_t gpio_num, interrupt_type_edle_t type, gpio_pull_mode_t  pull);
int input_io_get_level(gpio_num_t gpio_num);
// void input_set_callback(void * cb);

#endif
