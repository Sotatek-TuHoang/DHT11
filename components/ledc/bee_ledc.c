/*****************************************************************************
 *
 * @file 	bee_ledc.c
 * @author 	biennd
 * @date 	13 Jun 2023
 * @brief	All functions relate to config PWM for driving RGB led
 *
 ***************************************************************************/


/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include <stdio.h>
#include "bee_ledc.h"
#include "driver/ledc.h"


/***************************Init PWM*****************************************/
void rgb_led_pwm_init(void)
{
    int ch;

    ledc_ch[0].channel = LEDC_CHANNEL_0;
    ledc_ch[0].gpio = RGB_LED_RED_GPIO;
    ledc_ch[0].mode = LEDC_HIGH_SPEED_MODE;
    ledc_ch[0].timer_index = LEDC_TIMER_1;

    ledc_ch[1].channel = LEDC_CHANNEL_1;
    ledc_ch[1].gpio = RGB_LED_GREEN_GPIO;
    ledc_ch[1].mode = LEDC_HIGH_SPEED_MODE;
    ledc_ch[1].timer_index = LEDC_TIMER_1;

    ledc_ch[2].channel = LEDC_CHANNEL_2;
    ledc_ch[2].gpio = RGB_LED_BLUE_GPIO;
    ledc_ch[2].mode = LEDC_HIGH_SPEED_MODE;
    ledc_ch[2].timer_index = LEDC_TIMER_1;

    // confugure timer
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT, // resolution of PWM duty
        .freq_hz = 100,                      // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,  // timer mode
        .timer_num = LEDC_TIMER_1,           // timer index
    };
    ledc_timer_config(&ledc_timer);

    
    //Confugure channel
    for (ch = 0; ch < RGB_LED_CHANNEL_NUM; ch++) 
    {
        ledc_channel_config_t ledc_channel =
        {
            .channel    = ledc_ch[ch].channel,
            .duty       = 0,
            .gpio_num   = ledc_ch[ch].gpio,
            .speed_mode = ledc_ch[ch].mode,
            .hpoint     = 0,
            .timer_sel  = ledc_ch[ch].timer_index,
        };
        ledc_channel_config(&ledc_channel);
    }
}
    // SET THE RGB COLOR
void rgb_led_set_color (uint8_t red, uint8_t green,uint8_t blue)
{
    ledc_set_duty(ledc_ch[0].mode, ledc_ch[0].channel, red);
    ledc_update_duty(ledc_ch[0].mode, ledc_ch[0].channel);

    ledc_set_duty(ledc_ch[1].mode, ledc_ch[1].channel, green);
    ledc_update_duty(ledc_ch[1].mode, ledc_ch[1].channel);

    ledc_set_duty(ledc_ch[2].mode, ledc_ch[2].channel, blue);
    ledc_update_duty(ledc_ch[2].mode, ledc_ch[2].channel);
}


