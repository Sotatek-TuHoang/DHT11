/*****************************************************************************
 *
 * @file 	bee_dht11.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project read signal DHT11, Init, macro define pin
 *
 ***************************************************************************/

#ifndef BEE_DHT11_H_
#define BEE_DHT11_H_

#include "driver/gpio.h"

#define DHT11_PIN     GPIO_NUM_4

/**
 * @brief Enumerated values representing the status of DHT11 sensor readings.
 */
enum dht11_status {
    DHT11_CRC_ERROR = -2,
    DHT11_TIMEOUT_ERROR,
    DHT11_OK
};

/**
 * @brief Data structure representing a reading from the DHT11 sensor.
 */
struct dht11_reading {
    uint8_t status;
    uint8_t temperature;
    uint8_t humidity;
};

/**
 * @brief Initialize the DHT11 temperature and humidity sensor
 * 
 * This function initializes the DHT11 sensor by waiting for 1 second to allow the device to stabilize.
 * It also sets the GPIO pin number for communication with the sensor.
 * 
 * @param gpio_num - GPIO pin number to which the DHT11 data line is connected
 */
void DHT11_init(gpio_num_t);

/**
 * @brief Task for reading DHT11 temperature and humidity sensor data
 * 
 * This task periodically reads data from the DHT11 sensor, calculates average temperature
 * and humidity values, and prints the readings to the console. It also calculates the difference
 * between current readings and the moving average for temperature and humidity.
 * 
 * @param pvParameters - Task parameters (unused)
 * 
 * @result u8Temp - Temperature
 * @result u8Humi - Humidity
 * @result u8Temp_diff - Temperature different
 * @result u8Humi_diff - Humidity different
 */
void read_dht11_task(void* pvParameters);

struct dht11_reading DHT11_read();

#endif