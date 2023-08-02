/*****************************************************************************
 *
 * @file 	bee_dht11.c
 * @author 	tuha
 * @date 	3 July 2023
 * @brief	module for project read signal DHT11, Init
 *
 ***************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "esp_timer.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"

#include "bee_dht11.h"

/****************************************************************************/
/***        static Variables                                              ***/
/****************************************************************************/
static gpio_num_t dht_gpio;
static int64_t last_read_time = -2000000;
static struct dht11_reading last_read;
struct dht11_reading reading;
static uint8_t max_error_cnt = 10;
static uint8_t u8count_caculate_diff = 0;
static uint8_t u8Temp_array[num_readings];
static uint8_t u8Humi_array[num_readings];
static uint8_t u8Temp_index = 0;
static uint8_t u8Humi_index = 0;
static uint16_t u16Temp_sum = 0;
static uint16_t u16Humi_sum = 0;
static uint8_t u8Temp_avr = 0;
static uint8_t u8Humi_avr = 0;

/****************************************************************************/
/***        Global Variables                                              ***/
/****************************************************************************/
uint8_t u8status;
uint8_t u8temp;
uint8_t u8humi;
uint8_t u8error_cnt = 0;
uint8_t u8temp_diff = 0;
uint8_t u8humi_diff = 0;

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/
static int _waitOrTimeout(uint16_t microSeconds, int level) {
    int micros_ticks = 0;
    while(gpio_get_level(dht_gpio) == level) { 
        if(micros_ticks++ > microSeconds) 
            return DHT11_TIMEOUT_ERROR;
        ets_delay_us(1);
    }
    return micros_ticks;
}

static int _checkCRC(uint8_t dht11_data[]) {
    if(dht11_data[4] == (dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]))
        return DHT11_OK;
    else
        return DHT11_CRC_ERROR;
}

static void _sendStartSignal() {
    gpio_set_direction(dht_gpio, GPIO_MODE_OUTPUT);
    gpio_set_level(dht_gpio, 0);
    ets_delay_us(20 * 1000);
    gpio_set_level(dht_gpio, 1);
    ets_delay_us(40);
    gpio_set_direction(dht_gpio, GPIO_MODE_INPUT);
}

static int _checkResponse() {
    /* Wait for next step ~80us*/
    if(_waitOrTimeout(80, 0) == DHT11_TIMEOUT_ERROR)
        return DHT11_TIMEOUT_ERROR;

    /* Wait for next step ~80us*/
    if(_waitOrTimeout(80, 1) == DHT11_TIMEOUT_ERROR) 
        return DHT11_TIMEOUT_ERROR;

    return DHT11_OK;
}

static struct dht11_reading _timeoutError() {
    struct dht11_reading timeoutError = {DHT11_TIMEOUT_ERROR, -1, -1};
    return timeoutError;
}

static struct dht11_reading _crcError() {
    struct dht11_reading crcError = {DHT11_CRC_ERROR, -1, -1};
    return crcError;
}

/****************************************************************************/
/***        Init Functions                                                ***/
/****************************************************************************/
void DHT11_init(gpio_num_t gpio_num) {
    /* Wait 1 seconds to make the device pass its initial unstable status */
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    dht_gpio = gpio_num;
}

/****************************************************************************/
/***        struct                                                        ***/
/****************************************************************************/
struct dht11_reading DHT11_read() {
    /* Tried to sense too son since last read (dht11 needs ~2 seconds to make a new read) */
    if(esp_timer_get_time() - 2000000 < last_read_time) {
        return last_read;
    }

    last_read_time = esp_timer_get_time();

    uint8_t dht11_data[5] = {0,0,0,0,0};

    _sendStartSignal();

    if(_checkResponse() == DHT11_TIMEOUT_ERROR)
        return last_read = _timeoutError();
    
    /* Read response */
    for(int i = 0; i < 40; i++) {
        /* Initial dht11_data */
        if(_waitOrTimeout(50, 0) == DHT11_TIMEOUT_ERROR)
            return last_read = _timeoutError();
                
        if(_waitOrTimeout(70, 1) > 28) {
            /* Bit received was a 1 */
            dht11_data[i/8] |= (1 << (7-(i%8)));
        }
    }

    if(_checkCRC(dht11_data) != DHT11_CRC_ERROR) {
        last_read.status = DHT11_OK;
        last_read.temperature = dht11_data[2];
        last_read.humidity = dht11_data[0];
        return last_read;
    } else {
        return last_read = _crcError();
    }
}

/****************************************************************************/
/***        Read dht11 and caculate parameter task                        ***/
/****************************************************************************/
void read_dht11(void* pvParameters)
{
    TickType_t last_time_read = xTaskGetTickCount();
    TickType_t interval_read = pdMS_TO_TICKS(2000);
    for (;;)
    {
        if ((xTaskGetTickCount() - last_time_read) >= interval_read)
        {
            last_time_read = xTaskGetTickCount();
            reading = DHT11_read();
            u8temp = reading.temperature;
            u8humi = reading.humidity;
            u8status = reading.status;
            if (u8status == DHT11_OK)
            {
                u8error_cnt = 0;
                // Calculate temperature and humidity average

                //giảm giá trị của các phần tử trong mảng u8Temp_array tại chỉ mục u8Temp_index
                //khỏi tổng u16Temp_sum để chuẩn bị cho việc cộng các giá trị mới vào tổng
                u16Temp_sum -= u8Temp_array[u8Temp_index]; 
                u16Humi_sum -= u8Humi_array[u8Humi_index];

                u16Temp_sum += u8temp; // Tính tổng các giá trị nhiệt độ
                u16Humi_sum += u8humi;

                //gán giá trị nhiệt độ u8temp mới vào mảng u8Temp_array tại chỉ mục u8Temp_index.
                u8Temp_array[u8Temp_index] = u8temp;

                //cập nhật chỉ mục u8Temp_index để lưu trữ các giá trị mới nhất vào mảng.
                //Chỉ mục này sẽ được lặp lại từ 0 đến num_readings - 1 để duy trì kích thước của mảng. 
                u8Temp_index = (u8Temp_index + 1) % num_readings;

                u8Humi_array[u8Humi_index] = u8humi;
                u8Humi_index = (u8Humi_index + 1) % num_readings;

                // Tính giá trị nhiệt độ trung bình với num_readings lần đọc gần nhất
                u8Temp_avr = u16Temp_sum / num_readings; 
                u8Humi_avr = u16Humi_sum / num_readings;

                if (u8count_caculate_diff >= num_readings) //Chỉ khi lấy được num_readings lần mới tính độ chênh lệch
                {
                // Tính độ chênh lệch nhiệt độ với trung bình num_readings lần gần nhất
                u8temp_diff = abs(u8temp - u8Temp_avr);
                u8humi_diff = abs(u8humi - u8Humi_avr);
                }
                else
                {
                    u8count_caculate_diff ++;
                }

                printf("Temperature: %d C\nHumidity: %d%%\n", u8temp, u8humi);
                printf("Temperature Different: %d C\nHumidity Different: %d%%\n", u8temp_diff, u8humi_diff);
                
            }
            else if ((u8status != DHT11_OK) && (u8error_cnt < max_error_cnt))
            {
                ++u8error_cnt;
                printf("Error reading from DHT11: %d\nError read count: %d\n", u8status, u8error_cnt);
            }
            else
            {
                printf("Error reading from DHT11: %d\n", u8status);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/
