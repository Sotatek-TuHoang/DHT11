/*****************************************************************************
 *
 * @file 	bee_ledc.c
 * @author 	biennd
 * @date 	13 Jun 2023
 * @brief	All functions relate to config PWM for driving RGB led
 *
 ***************************************************************************/
#ifndef RGB_LED_H_
#define RGB_LED_H_

// RGB_GPIO
#define RGB_LED_RED_GPIO         27
#define RGB_LED_GREEN_GPIO       26
#define RGB_LED_BLUE_GPIO        25

// RGB LED COLOR MIX CHANNEL
#define RGB_LED_CHANNEL_NUM     3

// RGB LED confuguration 
typedef struct
{
    int channel;
    int gpio;
    int mode;
    int timer_index;
} ledc_infor_t;
ledc_infor_t ledc_ch[RGB_LED_CHANNEL_NUM];

void rgb_led_pwm_init(void);
void rgb_led_set_color (uint8_t red, uint8_t green,uint8_t blue);

#endif