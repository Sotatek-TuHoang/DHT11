/***************************************************************************
* @file 	bee_mqtt.h
* @author 	tuha
* @date 	5 July 2023
* @brief	module for send data through mqtt
* @brief	and receive command from host main through mqtt
***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "mqtt_client.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
#include "esp_log.h"

#include "bee_dht11.h"
#include "bee_mqtt.h"
#include "bee_nvs.h"
#include "bee_uart.h"

/****************************************************************************/
/***        Extern Variables                                              ***/
/****************************************************************************/

extern uint8_t u8temp;
extern uint8_t u8humi;
extern uint8_t u8status;
extern uint8_t u8error_cnt;
extern uint8_t u8temp_diff;
extern uint8_t u8humi_diff;
extern uint8_t u8data_interval_mqtt;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/
static uint8_t u8warning_values;
static bool bRead_status = 0;

static uint8_t u8mac[6];
static char cMac_str[13];
static char cTopic_pub[64] = "VB/DMP/VBEEON/CUSTOM/SMH/DeviceID/telemetry";
static char cTopic_sub[64] = "VB/DMP/VBEEON/CUSTOM/SMH/DeviceID/Command";
static uint8_t u8trans_code = 0;
static char rxBuffer_MQTT[500];

static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t client = NULL;
static QueueHandle_t mqtt_cmd_queue;

static bool bMQTT_CONNECTED = false;

/****************************************************************************/
/***        Event Handler                                                 ***/
/****************************************************************************/
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            bMQTT_CONNECTED = true;
            esp_mqtt_client_subscribe(client, cTopic_sub, 0);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            bMQTT_CONNECTED = false;
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            if ((event->data) != NULL)
            {
                ESP_LOGI(TAG, "MQTT_EVENT_DATA");
                snprintf(rxBuffer_MQTT, event->data_len + 1, event->data);
                xQueueSend(mqtt_cmd_queue, &rxBuffer_MQTT, portMAX_DELAY);
            }

            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;

        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

void mqtt_func_init(void)
{
    /*Config mqtt client*/
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_ADDRESS_URI,
        .credentials.username = USERNAME,
        .credentials.authentication.password = PASSWORD
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    /* Get mac Address and set topic*/
    esp_wifi_get_mac(ESP_IF_WIFI_STA, u8mac);
    snprintf(cMac_str, sizeof(cMac_str), "%02X%02X%02X%02X%02X%02X", u8mac[0], u8mac[1], u8mac[2], u8mac[3], u8mac[4], u8mac[5]);
    snprintf(cTopic_pub, sizeof(cTopic_pub), "VB/DMP/VBEEON/CUSTOM/SMH/%s/telemetry", cMac_str);
    snprintf(cTopic_sub, sizeof(cTopic_sub),"VB/DMP/VBEEON/CUSTOM/SMH/%s/Command", cMac_str);
    ESP_LOGI(TAG, "Topic publish: %s\n", cTopic_pub);
    ESP_LOGI(TAG, "Topic subscribe: %s\n", cTopic_sub);

    mqtt_cmd_queue = xQueueCreate(10, sizeof(cJSON*));

}

void pub_data(const char *object, int values)
{
    if (bMQTT_CONNECTED == true)
    {
        cJSON *json_data = cJSON_CreateObject();
        cJSON_AddStringToObject(json_data, "thing_token", cMac_str);
        cJSON_AddStringToObject(json_data, "cmd_name", "Bee.data");
        cJSON_AddStringToObject(json_data, "object_type", object);
        cJSON_AddNumberToObject(json_data, "values", values);
        cJSON_AddNumberToObject(json_data, "trans_code", ++u8trans_code);

        char *json_str = cJSON_Print(json_data);
        esp_mqtt_client_publish(client, cTopic_pub, json_str, 0, 1, 0);

        cJSON_Delete(json_data);
        free(json_str);
    }
}

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

/**
 * @brief Checks sensor warnings and sends a warning message if conditions are met.
 * This function evaluates sensor error conditions and threshold crossings for temperature and humidity.
 * Depending on the conditions, it sets warning flags and triggers sending a warning message.
 * The warning values are composed into a byte, and if they differ from the previous state,
 * a warning message is sent using the `send_warning()` function.
 */
static void check_warning(void)
{
    uint8_t u8tmp_warning_values;
    bool bTemp_threshold = false;
    bool bHumi_threshold = false;
    bool bTemp_diff = false;
    bool bHumi_diff = false;

    if (u8error_cnt == 10)
    {
        bRead_status = 1;
    }
    else if (u8error_cnt == 0)
    {
        bRead_status = 0;
        bTemp_threshold = u8temp > TEMP_THRESHOLD;
        bHumi_threshold = u8humi > HUMI_THRESHOLD;
        bTemp_diff = u8temp_diff > TEMP_DIFF_THRESHOLD;
        bHumi_diff = u8humi_diff > HUMI_DIFF_THRESHOLD;
    }
    
    u8tmp_warning_values = (bRead_status << 4) | (bTemp_threshold << 3) | (bHumi_threshold << 2) | (bTemp_diff << 1) | bHumi_diff;
    if (u8tmp_warning_values != u8warning_values)
    {
        u8warning_values = u8tmp_warning_values;
        pub_data("bee_warning", u8warning_values);
    }
}


/**
 * @brief Sends a keep-alive MQTT message to indicate device status.
 *
 * This function constructs a JSON message containing device status information such as the thing token,
 * event type, and status. The message is then published to the MQTT broker using the configured client.
 * The transmission code is also incremented for each message sent.
 *
 * @note The MQTT client (client) and topic (cTopic_pub) must be properly configured before calling this function.
 */
static void send_keep_alive(void)
{
    cJSON *json_keep_alive = cJSON_CreateObject();
    cJSON_AddStringToObject(json_keep_alive, "thing_token", cMac_str);
    cJSON *json_values = cJSON_AddObjectToObject(json_keep_alive, "values");
    cJSON_AddStringToObject(json_values, "eventType", "refresh");
    cJSON_AddStringToObject(json_values, "status", "ONLINE");
    cJSON_AddNumberToObject(json_keep_alive, "trans_code", ++u8trans_code);

    char *json_str = cJSON_Print(json_keep_alive);
    esp_mqtt_client_publish(client, cTopic_pub, json_str, 0, 1, 0);

    cJSON_Delete(json_keep_alive);
    free(json_str);
}

/**
 * @brief Send MQTT command to UART.
 *
 * This function prepares an MQTT command based on the provided values and sends
 * it to the UART for communication with an external device.
 *
 * @param values The value indicating the type of MQTT command.
 */
static void MQTT_cmd_to_uart(int values)
{
    uint8_t ID_MQTT = 0x00;
    uint8_t HiByte_length = 0x00;
    uint8_t HiByte_values = 0x00;
    uint8_t LoByte_values = 0x00;
    uint8_t Chs = 0x00;
    char cMQTT_cmd[9] = {0x55, 0xAA, 0x03, ID_MQTT, HiByte_length, 0x01, HiByte_values, LoByte_values, Chs};
    
    if (values == 1)
    {
        ID_MQTT = 0x10;
    }
    else if (values == 2)
    {
        ID_MQTT = 0x11;
    }
    else
    {
        ID_MQTT = 0x12;
    }

    // Calculate HiByte of values
    HiByte_values = (values >> 8) & 0xFF;
    LoByte_values = values & 0xFF;

    if (HiByte_values > 0x00)
    {
        HiByte_length = 0x01;
    }
    else
    {
        HiByte_length = 0x00;
    }
    // Assign the calculated values to the corresponding elements in the array
    cMQTT_cmd[3] = ID_MQTT;
    cMQTT_cmd[4] = HiByte_length;
    cMQTT_cmd[6] = HiByte_values;
    cMQTT_cmd[7] = LoByte_values;

    // Calculate checksum and store it in Chs
    cMQTT_cmd[8] = Chs = calculate_checksum((char*)cMQTT_cmd, 8);

    // Send the command to UART
    uart_write_bytes(UART_NUM, cMQTT_cmd, sizeof(cMQTT_cmd));
}

/****************************************************************************/
/***        Tasks                                                         ***/
/****************************************************************************/

void send_mqtt_data_task(void *pvParameters)
{
    TickType_t lt_send_data_mqtt = xTaskGetTickCount();
    TickType_t lt_send_keep_alive = xTaskGetTickCount();
    TickType_t interval_data_mqtt = pdMS_TO_TICKS(u8data_interval_mqtt * 1000);
    TickType_t interval_keep_alive = pdMS_TO_TICKS(15000);

    for(;;)
    {
        if (bMQTT_CONNECTED)
        {
            interval_data_mqtt = pdMS_TO_TICKS(u8data_interval_mqtt * 1000);
            if ((u8error_cnt == 0) && ((xTaskGetTickCount() - lt_send_data_mqtt) >= interval_data_mqtt))
            {
                lt_send_data_mqtt = xTaskGetTickCount();
                pub_data("bee_temp", u8temp);
                pub_data("bee_humi", u8humi);
            }

            check_warning();

            if ((xTaskGetTickCount() - lt_send_keep_alive) >= interval_keep_alive)
            {
                lt_send_keep_alive = xTaskGetTickCount();
                send_keep_alive();
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

void receive_mqtt_config_task(void *pvParameters)
{
    for (;;)
    {
        if (xQueueReceive(mqtt_cmd_queue, &rxBuffer_MQTT, portMAX_DELAY))
        {
            cJSON *root = cJSON_Parse(rxBuffer_MQTT);
            if (root != NULL)
            {
                char* cThing_token = cJSON_GetObjectItemCaseSensitive(root, "thing_token")->valuestring;
                char* cCmd_name = cJSON_GetObjectItemCaseSensitive(root, "cmd_name")->valuestring;
                char* cObject_type = cJSON_GetObjectItemCaseSensitive(root, "object_type")->valuestring;
                int values = cJSON_GetObjectItemCaseSensitive(root, "values")->valueint;
                int trans_code = cJSON_GetObjectItemCaseSensitive(root, "trans_code")->valueint;

                ESP_LOGI(TAG, "\n Received MQTT command:");
                ESP_LOGI(TAG, "thing_token: %s", cThing_token);
                ESP_LOGI(TAG, "cmd_name: %s", cCmd_name);
                ESP_LOGI(TAG, "object: %s", cObject_type);
                ESP_LOGI(TAG, "values: %d", values);
                ESP_LOGI(TAG, "trans_code: %d\n", trans_code);
                
                if ((strcmp(cCmd_name, "Bee.conf") == 0) && (strcmp(cObject_type, "data") == 0) && (values == 1))
                {
                    pub_data("bee_temp", u8temp);
                    MQTT_cmd_to_uart(values);
                    ESP_LOGI (TAG, "cmd temp ok\n");
                }
                else if ((strcmp(cCmd_name, "Bee.conf") == 0) && (strcmp(cObject_type, "data") == 0) && (values == 2))
                {
                    pub_data("bee_humi", u8humi);
                    MQTT_cmd_to_uart(values);
                    ESP_LOGI (TAG, "cmd humi ok\n");
                }
                else if ((strcmp(cCmd_name, "Bee.conf") == 0) && (strcmp(cObject_type, "cfg") == 0)
                                                        && (values >= 10) && (values <= 240))
                {
                    u8data_interval_mqtt = values;
                    MQTT_cmd_to_uart(values);
                    save_mqtt_data_to_nvs(u8data_interval_mqtt);
                    ESP_LOGI (TAG, "cmd interval ok\n");
                }
                else 
                {
                    ESP_LOGI (TAG, "wrong cmd\n");
                }
                cJSON_Delete(root);
            }
        }
    }
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/