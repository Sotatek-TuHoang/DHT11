#ifndef BUTTON_H
#define BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

extern QueueHandle_t send_data_evt_queue;
extern QueueHandle_t interval_evt_queue;

void button_init();

#endif
