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


#define SEND_DATA_BUTTON        GPIO_NUM_32
#define INTERVAL_BUTTON         GPIO_NUM_0

void button_init();
void send_data_button(void* arg);
void interval_button(void* arg);


#endif
