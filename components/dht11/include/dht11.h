/*****************************************************************************
 *
 * @file 	dht11.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project read signal DHT11, Init, macro define pin
 *
 ***************************************************************************/

/****************************************************************************/
#ifndef DHT11_H_
#define DHT11_H_

#include "driver/gpio.h"

enum dht11_status {
    DHT11_CRC_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
};

struct dht11_reading {
    uint8_t status;
    uint8_t temperature;
    uint8_t humidity;
};

void DHT11_init(gpio_num_t);

struct dht11_reading DHT11_read();

#endif