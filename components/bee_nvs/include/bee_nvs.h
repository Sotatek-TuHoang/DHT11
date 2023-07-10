/*****************************************************************************
 *
 * @file 	bee_nvs.h
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for save data, status into nvs flash
 *
 ***************************************************************************/

/****************************************************************************/
#ifndef BEE_NVS_H_
#define BEE_NVS_H_

#include <stdbool.h>
#include <stdint.h>

void nvs_flash_function_init();
void save_data_to_nvs(bool bsend_data, uint32_t u32data_interval);
void load_data_from_nvs(bool* bsend_data, uint32_t* u32data_interval);

#endif /* BEE_NVS_H */
