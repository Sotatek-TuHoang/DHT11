/*****************************************************************************
 *
 * @file 	bee_wifi.h
 * @author 	tuha
 * @date 	5 July 2023
 * @brief	module for connect wifi
 *
 ***************************************************************************/

/****************************************************************************/
#ifndef BEE_WIFI_H_
#define BEE_WIFI_H_

#include "freertos/event_groups.h"

extern EventGroupHandle_t s_wifi_event_group;

void wifi_init_sta(void);

#endif /* BEE_WIFI_H_ */

