/*****************************************************************************
 *
 * @file 	bee_DHT11Handle.c
 * @author 	biennd
 * @date 	13 Jun 2023
 * @brief	read sensor data sent to Herculus thru Uart and Server thru MQTT
 *
 ***************************************************************************/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "app_config.h"
#include "bee_uart_t.h"
#include "app_mqtt.h"
#include "bee_output_iot.h"
#include "bee_input_iot.h"
#include "bee_app_nvs.h"
#include "bee_ledc.h"
#include "bee_dht11.h" 

static const char *TAG = "DHT11";


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
// Key_pad GPIO
#define GPIO_POW          13
#define GPIO_1            12
#define GPIO_2            14
#define GPIO_3            27
#define GPIO_4            26

// DHT11_data GPIO 
#define GPIO_DHT11        5

// Default Period of Reading sensor data
#define Period            10

// Reset wifi GPIO 
#define GPIO_INPUT_IO_0   4

// Uart_config
#define EX_UART_NUM UART_NUM_2
#define PATTERN_CHR_NUM    (3)      
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)


/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
struct dht11_reading dht11; // Read sensor data
float temp_dht11; // Temperature data
float humi_dht11; // Humidity data
char data_temp[5];// char array for Sending temperature data 
char data_humi[5];// char array for Sending humidity data 


char Message[100]; //buffer for sending data thru UART
int period = Period; // Reading sensor data period
int store; // Store reading period into flash
char Message1[100]; //buffer for sending data thru UART


wifi_config_t wifi_config;


void Uart_handler(char *data, uint16_t length)
{
   period = atoi(data);
   if(period == 0)
   {
       period = Period;
       app_nvs_set_value(KEY_WORD,period);
   }
   ESP_LOGI(TAG, "Period: %d\n",period);
   app_nvs_set_value(KEY_WORD,period);
   sprintf(Message1,"Read Sensor data Period: %d\n",period);
   uart_put(Message1,sizeof(Message1));
   memset(Message1, NULL ,sizeof(Message1));
}


/****************Press 1 button to change reading data period into 5s************/
static void button1(void *pvParameters)
{
    for(;;)
    {
    if (input_io_get_level(GPIO_1)==1)
    {
        period = 5;
        ESP_LOGI(TAG, "Period: %d\n",period);
        app_nvs_set_value(KEY_WORD,period);
        sprintf(Message1,"Read Sensor data Period: %d\n",period);
        uart_put(Message1,sizeof(Message1));
        memset(Message1, NULL ,sizeof(Message1));
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

/****************Press 2 button to change reading data period into 10s************/
static void button2(void *pvParameters)
{
     for(;;)
    {
    if (input_io_get_level(GPIO_2)==1)
    {
        period = 10;
        ESP_LOGI(TAG, "Period: %d\n",period);
        app_nvs_set_value(KEY_WORD,period);
        sprintf(Message1,"Read Sensor data Period: %d\n",period);
        uart_put(Message1,sizeof(Message1));
        memset(Message1, NULL ,sizeof(Message1));
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    }
}


/****************Press 3 button to change reading data period into 15s************/
static void button3(void *pvParameters)
{
    for(;;)
    {
    if (input_io_get_level(GPIO_3)==1)
    {
        period = 15;
        ESP_LOGI(TAG, "Period: %d\n",period);
        app_nvs_set_value(KEY_WORD,period);
        sprintf(Message1,"Read Sensor data Period: %d\n",period);
        uart_put(Message1,sizeof(Message1));
        memset(Message1, NULL ,sizeof(Message1));
    }
     vTaskDelay(100/portTICK_PERIOD_MS);
    }
}


/****************Press 4 button to change reading data period into 20s************/
static void button4(void *pvParameters)
{
    for(;;)
    {
    if (input_io_get_level(GPIO_4)==1)
    {
        period = 20;
        ESP_LOGI(TAG, "Period: %d\n",period);
        app_nvs_set_value(KEY_WORD,period);
        sprintf(Message1,"Read Sensor data Period: %d\n",period);
        uart_put(Message1,sizeof(Message1));
        memset(Message1, NULL ,sizeof(Message1));
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

/****************Press GPIO4 GPIO button to reset wifi***************/
static void button5(void *pvParameters)
{
    for(;;)
    {
    if (input_io_get_level(GPIO_INPUT_IO_0)==0)
{
     wifi_config.sta.ssid[0] = 0x00;
     ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
     ESP_ERROR_CHECK(nvs_flash_init());
     ESP_ERROR_CHECK(esp_netif_init());
     ESP_ERROR_CHECK(esp_event_loop_create_default());
     app_config();
}
    vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

/****************Reading data and send these data************/
static void sensor(void *pvParameters)
{
    for(;;)
    {
        dht11 = DHT11_read();
        temp_dht11 = dht11.temperature;
        humi_dht11= dht11.humidity;
        sprintf(data_temp,"%.1f",temp_dht11);
        sprintf(data_humi,"%.1f",humi_dht11);
        ESP_LOGI(TAG, "Temperature: %.1f Celcius         Humidity: %.1f\n",temp_dht11,humi_dht11);
        sprintf(Message,"Temperature = %.1f Celcius         Humidity = %.1f%c\n ",temp_dht11,humi_dht11,'%');
        uart_put(Message,sizeof(Message));
        memset(Message,NULL,sizeof(Message));
        if(temp_dht11 > 50)
        {
            rgb_led_set_color(0,0,255);
        } else if (humi_dht11 < 50)
        {
            rgb_led_set_color(0,255,0);
        } else if (temp_dht11 > 50 && humi_dht11 < 50)
        {
            rgb_led_set_color(255,255,255);
        } else
        {
            rgb_led_set_color(255,0,0);
        }

        vTaskDelay(period*1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    input_io_create(GPIO_1, 1, GPIO_PULLDOWN_ONLY);
    input_io_create(GPIO_2, 1, GPIO_PULLDOWN_ONLY);
    input_io_create(GPIO_3, 1, GPIO_PULLDOWN_ONLY);
    input_io_create(GPIO_4, 1, GPIO_PULLDOWN_ONLY);
    input_io_create(GPIO_INPUT_IO_0, 2, GPIO_PULLUP_ONLY);
    output_io_create(GPIO_POW);
    output_io_set_level(GPIO_POW, 0);

    xTaskCreate(&button5,"button5", 4096, NULL, 15, NULL);
    
    app_config();
    app_mqtt_init();
    app_mqtt_start();
    uart_set_callback(Uart_handler);
    uart_init_cmd();
    DHT11_init(GPIO_DHT11);
    rgb_led_pwm_init();
    app_nvs_get_value(KEY_WORD,&period);
    ESP_LOGI(TAG, "Period: %d\n",period);


    xTaskCreate(&button1,"button1", 4096, NULL, 19, NULL);
    xTaskCreate(&button2,"button2", 4096, NULL, 18, NULL);
    xTaskCreate(&button3,"button3", 4096, NULL, 17, NULL);
    xTaskCreate(&button4,"button4", 4096, NULL, 16, NULL);
    xTaskCreate(&sensor,"sensor", 4096, NULL, 21, NULL);
}
