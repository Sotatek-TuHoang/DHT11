/*****************************************************************************
 *
 * @file 	bee_nvs.h
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for save data, status into nvs flash
 *
 ***************************************************************************/
#ifndef BEE_NVS_H_
#define BEE_NVS_H_

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define NVS_DATA_CONFIG         "data_config"
#define NVS_SEND_DATA           "send_data"
#define NVS_UART_INTERVAL       "uart_interval"
#define NVS_MQTT_INTERVAL       "mqtt_interval"
#define NVS_PROVISIONED         "provisioned"

#define NVS_WIFI_CRED           "wifi_cred"
#define NVS_WIFI_PASS           "wifi_pass"
#define NVS_WIFI_SSID           "wifi_ssid"

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/**
 * @brief Initialize NVS Flash.
 *
 * This function initializes the Non-Volatile Storage (NVS) Flash. It first attempts to initialize NVS,
 * and if it encounters errors indicating no free pages or a new version found, it erases the NVS and
 * attempts initialization again. Any other errors are handled using ESP_ERROR_CHECK.
 */
void nvs_flash_func_init();

/**
 * @brief Save UART data configuration to NVS.
 *
 * This function saves the UART data configuration (send_data flag and data_interval) to the Non-Volatile Storage (NVS).
 * It first opens the NVS, then saves the send_data flag and data_interval values.
 * If an error occurs, an appropriate error message is printed.
 * The NVS handle is then closed.
 *
 * @param bsend_data The send_data flag to be saved.
 * @param u8data_interval_uart The UART data interval value to be saved.
 */
void save_uart_data_to_nvs(bool bsend_data, uint8_t u8data_interval_uart);

/**
 * @brief Load data configuration from NVS.
 *
 * This function loads the data configuration (send_data flag, UART data interval, MQTT data interval) from the
 * Non-Volatile Storage (NVS). It first opens the NVS, then reads the values for send_data flag, UART data interval,
 * and MQTT data interval. If a value is not found in NVS, default values are used. If an error occurs during reading,
 * an appropriate error message is printed. The NVS handle is then closed.
 *
 * @param bsend_data Pointer to the send_data flag to be loaded.
 * @param u8data_interval_uart Pointer to the UART data interval value to be loaded.
 * @param u8data_interval_mqtt Pointer to the MQTT data interval value to be loaded.
 */
void load_data_from_nvs(bool* bsend_data, uint8_t* u8data_interval_uart, uint8_t* u8data_interval_mqtt);

/**
 * @brief Save MQTT data interval to NVS.
 *
 * This function saves the MQTT data interval to the Non-Volatile Storage (NVS).
 * It first opens the NVS, then saves the interval value.
 * If an error occurs, an appropriate error message is printed.
 * The NVS handle is then closed.
 *
 * @param u8data_interval_mqtt The MQTT data interval value to be saved.
 */
void save_mqtt_data_to_nvs(uint8_t u8data_interval_mqtt);

/**
 * @brief Save Wi-Fi credentials to NVS.
 *
 * This function saves the provided Wi-Fi SSID and password to the NVS (Non-Volatile Storage).
 *
 * @param cSsid     Pointer to the Wi-Fi SSID string.
 * @param cPassword Pointer to the Wi-Fi password string.
 */
void save_wifi_cred_to_nvs(const char *cSsid, const char *cPassword);

/**
 * @brief Load old Wi-Fi credentials from NVS.
 *
 * This function loads the old Wi-Fi SSID and password from the Non-Volatile Storage (NVS).
 * It first opens the NVS, then reads the SSID and password values if they exist.
 * If not found or if any error occurs, appropriate error messages are printed.
 * The NVS handle is then closed.
 *
 * @param cSsid Buffer to store the loaded Wi-Fi SSID.
 * @param cPassword Buffer to store the loaded Wi-Fi password.
 */
void load_old_wifi_cred(char *cSsid, char *cPassword);

#endif /* BEE_NVS_H */
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
