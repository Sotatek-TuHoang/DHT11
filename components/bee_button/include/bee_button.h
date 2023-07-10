/*****************************************************************************
 *
 * @file 	bee_button.h
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project cotrol by button
 *
 ***************************************************************************/
#ifndef BEE_BUTTON_H
#define BEE_BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t send_data_evt_queue;
extern QueueHandle_t interval_evt_queue;

void button_init();

#endif
