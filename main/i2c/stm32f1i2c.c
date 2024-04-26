#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "stm32f1i2c.h"
#include "main.h"

#define CHECK_ARG(ARG) do { if (!ARG) return ESP_ERR_INVALID_ARG; } while (0)

esp_err_t stm32f1_init(i2c_dev_t *dev, i2c_port_t port, gpio_num_t sda_gpio, gpio_num_t scl_gpio)
{
    CHECK_ARG(dev);

    dev->port = port;
    dev->addr = STM32F1_ADDR;
    dev->sda_io_num = sda_gpio;
    dev->scl_io_num = scl_gpio;
    dev->clk_speed = I2C_FREQ_HZ;
    return i2c_master_init(port, sda_gpio, scl_gpio);
}
esp_err_t stm32f1_get_data(i2c_dev_t *dev, char *data, size_t size)
{
    CHECK_ARG(dev);

     esp_err_t res = i2c_dev_read_reg(dev,DATA_STM32F1_ADDR , data, size);

    return res;
}

void processing_string_Receive_i2c(char *inputStrings ,char *name,char *value)
{
    char message_mqtt[1000];
    char *colonPosition1 = strchr(inputStrings, ':');

    if (colonPosition1 != NULL)
    {
        // Tìm vị trí của dấu hai chấm (:) tiếp theo
        char *colonPosition2 = strchr(colonPosition1 + 1, ':');

        if (colonPosition2 != NULL)
        {
            // Tìm vị trí của ký tự kết thúc giá trị (trước dấu #)
            char *hashPosition = strchr(colonPosition2, '#');

            if (hashPosition != NULL)
            {
                // Tính toán độ dài của giá trị
                size_t valueLength = hashPosition - (colonPosition2 + 1);

                // Sao chép tên vào mảng name
                strncpy(name, colonPosition1 + 1, colonPosition2 - (colonPosition1 + 1));
                name[colonPosition2 - (colonPosition1 + 1)] = '\0';

                // Sao chép giá trị vào mảng value
                strncpy(value, colonPosition2 + 1, valueLength);
                value[valueLength] = '\0';
            }
        }
    }

    printf("name: %s\n", name);
    printf("value: %s\n", value);


    // ds3231_get_time(&rtc_i2c, &local_time);
    // sprintf(message_mqtt, "{\"name\": \"%s\",\"value\": \"%s\",\"timestamp\": \"%04d-%02d-%02dT%02d:%02d:%02d\"}", name, value,
    //         local_time.tm_year, local_time.tm_mon, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
    // esp_mqtt_client_publish(client, MATERIAL_CODE_DONE_TOPIC, message_mqtt, 0, 1, 1);
    //printf(message_mqtt);
    if (strstr(name, "MACHITIETXONG") != NULL){			
        printf("VO DAY ROI NHA....");
				
		}


}
