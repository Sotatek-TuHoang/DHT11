/*****************************************************************************
 *
 * @file 	bee_button.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project cotrol by button
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include "bee_button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include <esp_log.h>

#include"bee_nvs.h"

/****************************************************************************/
/***        Static variable                                               ***/
/****************************************************************************/

static const char *TAG = "Button";
static QueueHandle_t       send_data_evt_queue;
static QueueHandle_t       interval_evt_queue;

/****************************************************************************/
/***        Extern variable                                               ***/
/****************************************************************************/

extern bool bsend_data; // Flag control on/off upload data throught uart
extern uint8_t u8data_interval_uart; // Deffault interval upload data uart

/****************************************************************************/
/***        Static function                                               ***/
/****************************************************************************/

static void IRAM_ATTR send_data_button_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(send_data_evt_queue, &gpio_num, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

static void IRAM_ATTR interval_button_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(interval_evt_queue, &gpio_num, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

void send_data_button_isr(void* arg)
{
    uint8_t gpio_num;
    for (;;) {
        if (xQueueReceive(send_data_evt_queue, &gpio_num, portMAX_DELAY))
        {
            bsend_data = !bsend_data;
            ESP_LOGI(TAG, "Send data button clicked: %d", bsend_data);
            save_uart_data_to_nvs(bsend_data, u8data_interval_uart);
        }
    }
}

void interval_button_isr(void* arg)
{
    uint8_t gpio_num;
    for (;;) {
        if (xQueueReceive(interval_evt_queue, &gpio_num, portMAX_DELAY))
        {
            if (u8data_interval_uart == 2)
            {
                u8data_interval_uart = 5;  // Change to every 5 secends
            }
            else if (u8data_interval_uart == 5)
            {
                u8data_interval_uart = 10;  // Change to every 10 secends
            }
            else if (u8data_interval_uart == 10)
            {
                u8data_interval_uart = 2;  // Change to every 2 secends
            }

            ESP_LOGI(TAG, "Interval button clicked: %ds", u8data_interval_uart);

            save_uart_data_to_nvs(bsend_data, u8data_interval_uart);
        }
    }
}

void button_init(void)
{
    gpio_install_isr_service(0);
    gpio_isr_handler_add(SEND_DATA_BUTTON, send_data_button_isr_handler, (void*) SEND_DATA_BUTTON);
    gpio_isr_handler_add(INTERVAL_BUTTON, interval_button_isr_handler, (void*) INTERVAL_BUTTON);

    esp_rom_gpio_pad_select_gpio(SEND_DATA_BUTTON);
    esp_rom_gpio_pad_select_gpio(INTERVAL_BUTTON);

    gpio_set_direction(SEND_DATA_BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SEND_DATA_BUTTON, GPIO_PULLUP_ONLY);

    gpio_set_direction(INTERVAL_BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(INTERVAL_BUTTON, GPIO_PULLUP_ONLY);

    gpio_set_intr_type(SEND_DATA_BUTTON, GPIO_INTR_POSEDGE);
    gpio_set_intr_type(INTERVAL_BUTTON, GPIO_INTR_POSEDGE);

    send_data_evt_queue = xQueueCreate(5, sizeof(uint8_t));
    interval_evt_queue = xQueueCreate(5, sizeof(uint8_t));
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/