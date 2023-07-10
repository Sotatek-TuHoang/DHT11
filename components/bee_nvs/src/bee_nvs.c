/*****************************************************************************
 *
 * @file 	bee_nvs.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for save data, status into nvs flash
 *
 ***************************************************************************/

/****************************************************************************/
#include "bee_nvs.h"
#include "esp_system.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"

#define NVS_NAMESPACE           "storage"
#define NVS_SEND_DATA           "send_data"
#define NVS_INTERVAL            "data_interval"

void nvs_flash_function_init()
{
    esp_err_t err = nvs_flash_init(); // Initialize NVS
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

void save_data_to_nvs(bool bsend_data, uint32_t u32data_interval)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    // Mở NVS
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle! (%s)\n", esp_err_to_name(err));
        return;
    }
    // Lưu trạng thái send_data vào NVS
    err = nvs_set_u8(nvs_handle, NVS_SEND_DATA, bsend_data);
    if (err != ESP_OK)
    {
        printf("Error saving send_data to NVS! (%s)\n", esp_err_to_name(err));
    }
    // Lưu giá trị data_interval vào NVS
    err = nvs_set_u32(nvs_handle, NVS_INTERVAL, u32data_interval);
    if (err != ESP_OK)
    {
        printf("Error saving data_interval to NVS! (%s)\n", esp_err_to_name(err));
    }
    // Đóng NVS
    nvs_close(nvs_handle);
}

void load_data_from_nvs(bool* bsend_data, uint32_t* u32data_interval)
{
    nvs_handle_t nvs_handle;
    esp_err_t err;
    // Mở NVS
    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK)
    {
        printf("Error opening NVS handle! (%s)\n", esp_err_to_name(err));
        return;
    }
    // Đọc trạng thái send_data từ NVS
    err = nvs_get_u8(nvs_handle, NVS_SEND_DATA, (uint8_t*)bsend_data);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        // Khóa không tồn tại trong NVS, sử dụng giá trị mặc định
        *bsend_data = true; // Giá trị mặc định là true
    }
    else if (err != ESP_OK)
    {
        printf("Error reading send_data from NVS! (%s)\n", esp_err_to_name(err));
    }
    // Đọc giá trị data_interval từ NVS
    err = nvs_get_u32(nvs_handle, NVS_INTERVAL, u32data_interval);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        // Khóa không tồn tại trong NVS, sử dụng giá trị mặc định
        *u32data_interval = 2000; // Giá trị mặc định là 2 giây
    }
    else if (err != ESP_OK)
    {
        printf("Error reading data_interval from NVS! (%s)\n", esp_err_to_name(err));
    }
    // Đóng NVS
    nvs_close(nvs_handle);
}