/*****************************************************************************
 *
 * @file 	bee_app_MQTT.c
 * @author 	biennd
 * @date 	13 Jun 2023
 * @brief	All functions relate to config MQTT
 *
 ***************************************************************************/

#ifndef __APP_MQTT_H
#define __APP_MQTT_H
#include <stdint.h>     
#define MQTT_BROKER     "mqtt://45.77.243.33:1883"
#define TOPIC           "v1/devices/me/telemetry"
typedef void (*mqtt_data_handle_t) (char *data, uint16_t len);
typedef void (*mqtt_publish_handle_t) (char *topic);
void app_mqtt_init(void);
void app_mqtt_start(void);
void app_mqtt_stop(void);
void app_mqtt_publish(char * topic, char *data, uint16_t len);
void app_mqtt_subscribe(char * topic);
void app_mqtt_set_cb_event(void *cb);
void app_mqtt_set_cb_publish(void *cb);

#endif