/*****************************************************************************
 *
 * @file 	wifi_function.h
 * @author 	tuha
 * @date 	5 July 2023
 * @brief	module for connect wifi
 *
 ***************************************************************************/

/****************************************************************************/
#ifndef WIFI_FUNCTION_H_
#define WIFI_FUNCTION_H_

#include "freertos/event_groups.h"

extern EventGroupHandle_t s_wifi_event_group;

void wifi_init_sta(void);

#endif /* WIFI_FUNCTION_H_ */

