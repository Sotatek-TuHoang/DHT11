/*****************************************************************************
 *
 * @file 	bee_button.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project cotrol by button
 *
 ***************************************************************************/

#include "bee_button.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define SEND_DATA_BUTTON        GPIO_NUM_32
#define INTERVAL_BUTTON         GPIO_NUM_0

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

void button_init()
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

}