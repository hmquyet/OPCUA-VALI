
#include <stdio.h>
#include "string.h"
#include <stddef.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_task_wdt.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "driver/timer.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_types.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "open62541.h"

#include "wifi_connect.h"
#include "mqtt_connect.h"
#include "ethernet_connect.h"
#include "input.h"
#include "opcua.h"

const char *RECONNECT_BROKER_TIMER = "RECONNECT_BROKER_TIMER";
static const char *TAG = "TEST_ESP";

static char DATA_FILE[50] = "/sdcard/dataopc.txt";

TimerHandle_t soft_timer_handle_1;
static char *received_data;
void write_to_sd(char content[], char file_path[]);
/*----------------------------------------------------------------------*/
/**
 * @brief Write content to file in SD card in append+ mode
 *
 * @param content Content to write
 * @param file_path File path to write
 */

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define PIN_NUM_CS 5
#define MOUNT_POINT "/sdcard"
 double injection_time = 0;
 double injection_cycle = 0;
  char message_mqtt[1000];
static bool error_sd_card;
const char *MACHINE_STATUS_TOPIC = "IMM/I2/Metric/MachineStatus";
const char *INJECTION_CYCLE_TOPIC = "IMM/I2/Metric/InjectionCycle";
const char *INJECTION_TIME_TOPIC = "IMM/I2/Metric/InjectionTime";
static void event_input_gpio(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            vTaskDelay(50);
            if (gpio_get_level(33) == 0)
            {
                timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL); // Set cho timer ve gia tri 0
                timer_start(TIMER_GROUP_0, TIMER_0);
            }
            else if (gpio_get_level(33) == 1)
            {
                timer_pause(TIMER_GROUP_0, TIMER_0); // Dung timer dem thoi gian chu ky
        timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_0, &injection_time);
        sprintf(message_mqtt, "[{%cname%c: %cinjectionTime%c,%cvalue%c: %f,%ctimestamp%c: %c%04d-%02d-%02dT%02d:%02d:%02d%c}]",
                34, 34, 34, 34, 34, 34, injection_time, 34, 34, 34, 23,9, 00, 0, 0, 0, 34);
      
         esp_mqtt_client_publish(mqttclient, MACHINE_STATUS_TOPIC, message_mqtt, 0, 1, 1);
            }
        //     if (gpio_get_level(32) == 0)
        //     {
        //         timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL); // Set cho timer ve gia tri 0
        //         timer_start(TIMER_GROUP_0, TIMER_0);
        //     }
        //     else if (gpio_get_level(32) == 1)
        //     {
        //         timer_pause(TIMER_GROUP_0, TIMER_0); // Dung timer dem thoi gian chu ky
        // timer_get_counter_time_sec(TIMER_GROUP_0, TIMER_0, &injection_cycle);
        // sprintf(message_mqtt, "[{%cname%c: %cinjectionCycle%c,%cvalue%c: %f,%ctimestamp%c: %c%04d-%02d-%02dT%02d:%02d:%02d%c}]",
        //         34, 34, 34, 34, 34, 34, injection_time, 34, 34, 34, 23,9, 00, 0, 0, 0, 34);
      
        //  esp_mqtt_client_publish(mqttclient, MACHINE_STATUS_TOPIC, message_mqtt, 0, 1, 1);
        //     }
        }
    }
}

void lora_task(void *pvParameter)
{
    
        char data[100];
        lora_reciever_uart(data);
        
    
}

static void vSoftTimerCallback(TimerHandle_t xTimer)
{

    if (pcTimerGetName(xTimer) == RECONNECT_BROKER_TIMER)
    {
        xTimerStop(soft_timer_handle_1, 10);
        esp_mqtt_client_reconnect(mqttclient);
        // TWDT
        esp_task_wdt_reset();
        // esp_task_wdt_delete(NULL);
    }
}
static void opcua_task(void *pvParameter)
{

    ESP_LOGI(TAG, "Start OPCUA Client Instance");

    UA_Client *opcua_client = UA_Client_new(UA_ClientConfig_default);
    UA_StatusCode status = UA_Client_connect(opcua_client, "opc.tcp://192.168.1.15:4840");

    char oldvalue[10];

    if (status == UA_STATUSCODE_GOOD)
    {
        while (1)
        {
            // LIST NODE ID
            UA_NodeId nodeIds[1];
            nodeIds[0] = UA_NODEID_NUMERIC(4, 9);
            // nodeIds[1] = UA_NODEID_NUMERIC(4, 10);
            // nodeIds[2] = UA_NODEID_NUMERIC(4, 8);
            // nodeIds[3] = UA_NODEID_NUMERIC(4, 15);
            // nodeIds[4] = UA_NODEID_NUMERIC(4, 7);
            // nodeIds[5] = UA_NODEID_NUMERIC(4, 16);
            // nodeIds[6] = UA_NODEID_NUMERIC(4, 14);

            for (int i = 0; i < 1; i++)
            {
                // Đọc tên biến của NodeId
                UA_ReadRequest readNameRequest;
                UA_ReadRequest_init(&readNameRequest);
                readNameRequest.nodesToRead = UA_ReadValueId_new();
                readNameRequest.nodesToReadSize = 1;
                readNameRequest.nodesToRead[0].nodeId = nodeIds[i];
                readNameRequest.nodesToRead[0].attributeId = UA_ATTRIBUTEID_DISPLAYNAME;

                UA_ReadResponse readNameResponse = UA_Client_Service_read(opcua_client, readNameRequest);

                if (readNameResponse.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
                {
                    // Đọc tên biến displayName
                    UA_Variant *browseNameValue = &(readNameResponse.results[0].value);

                    if (UA_Variant_hasScalarType(browseNameValue, &UA_TYPES[UA_TYPES_LOCALIZEDTEXT]))
                    {
                        UA_LocalizedText *localizedText = (UA_LocalizedText *)browseNameValue->data;

                        char *nodeName = (char *)UA_malloc((int)localizedText->text.length + 1);
                        memcpy(nodeName, localizedText->text.data, (int)localizedText->text.length);
                        nodeName[(int)localizedText->text.length] = '\0';

                        UA_ReadRequest readRequest;
                        UA_ReadRequest_init(&readRequest);
                        readRequest.nodesToRead = UA_ReadValueId_new();
                        readRequest.nodesToReadSize = 1;
                        readRequest.nodesToRead[0].nodeId = nodeIds[i];
                        readRequest.nodesToRead[0].attributeId = UA_ATTRIBUTEID_VALUE;
                        UA_ReadResponse readResponse = UA_Client_Service_read(opcua_client, readRequest);

                        if (readResponse.responseHeader.serviceResult == UA_STATUSCODE_GOOD)
                        {

                            // ĐỌc giá trị biến
                            UA_Variant *value = &(readResponse.results[0].value);

                            if (UA_Variant_hasScalarType(value, &UA_TYPES[UA_TYPES_FLOAT]))
                            {
                                float floatValue = *(float *)value->data;

                                char data_receiver_opcua[100];
                                snprintf(data_receiver_opcua, sizeof(data_receiver_opcua), "{\"name\":\"%s\",\"value\":%.2f,\"timestamp\":\"\"}\n", nodeName, floatValue);
                                printf(data_receiver_opcua);
                                //  write_to_sd(data_receiver_opcua,DATA_FILE);
                                // esp_mqtt_client_publish(mqttclient, "test/dataUart", data_receiver_opcua, 0, 0, 0);
                            }

                            else if (UA_Variant_hasScalarType(value, &UA_TYPES[UA_TYPES_DOUBLE]))
                            {
                                float doubleValue = *(float *)value->data;

                                char data_receiver_opcua[100];
                                snprintf(data_receiver_opcua, sizeof(data_receiver_opcua), "{\"name\":\"%s\",\"value\":%.2f,\"timestamp\":\"\"}\n", nodeName, doubleValue);
                                printf(data_receiver_opcua);
                                //  write_to_sd(data_receiver_opcua,DATA_FILE);
                                // esp_mqtt_client_publish(mqttclient, "test/dataUart", data_receiver_opcua, 0, 0, 0);
                            }

                            else if (UA_Variant_hasScalarType(value, &UA_TYPES[UA_TYPES_UINT16]))
                            {

                                char data_receiver_opcua[100];
                                u_int16_t intValue = *(u_int16_t *)value->data;

                                snprintf(data_receiver_opcua, sizeof(data_receiver_opcua), "{\"name\":\"%s\",\"value\":%u,\"timestamp\":\"\"}\n", nodeName, intValue);
                                printf(data_receiver_opcua);
                                // write_to_sd(data_receiver_opcua,DATA_FILE);
                                // esp_mqtt_client_publish(mqttclient, "test/dataUart", data_receiver_opcua, 0, 0, 0);
                            }
                            else if (UA_Variant_hasScalarType(value, &UA_TYPES[UA_TYPES_BOOLEAN]))
                            {
                                char data_receiver_opcua[100];
                                UA_Boolean booleanValue = *(UA_Boolean *)value->data;

                                snprintf(data_receiver_opcua, sizeof(data_receiver_opcua), "{\"name\":\"%s\",\"value\":%d,\"timestamp\":\"\"}\n", nodeName, booleanValue);

                                printf(data_receiver_opcua);
                                // write_to_sd(data_receiver_opcua,DATA_FILE);
                                //  esp_mqtt_client_publish(mqttclient, "test/dataUart", data_receiver_opcua, 0, 0, 0);
                            }
                            else
                            {
                                printf("Unsupported data type for Node %d\n", i + 1);
                            }
                            UA_ReadResponse_deleteMembers(&readResponse);
                        }
                    }
                    UA_ReadResponse_deleteMembers(&readNameResponse);
                }
            }
            printf("\n\n");
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}

// void write_to_sd(char content[], char file_path[])
// {
//     FILE *f = fopen(file_path, "a+");
//     if (f == NULL)
//     {
//         ESP_LOGE(TAG, "Failed to open file for writing --> Restart ESP");
//         esp_restart();
//         return;
//     }
//     int i = 0;
//     while (content[i] != NULL)
//         i++;

//     char buff[i + 1];
//     for (int j = 0; j < i + 1; j++)
//     {
//         buff[j] = content[j];
//     }
//     fprintf(f, buff);
//     fprintf(f, "\n");
//     fclose(f);
//     ESP_LOGI(TAG, "File written");
// }
// static void sdcard_mount()
// {
//     /*sd_card part code*/
//     esp_vfs_fat_sdmmc_mount_config_t mount_config =
//         {
//             .format_if_mount_failed = true,
//             .max_files = 5,
//             .allocation_unit_size = 16 * 1024};
//     sdmmc_card_t *card;

//     const char mount_point[] = MOUNT_POINT;
//     ESP_LOGI(TAG, "Initializing SD card");

//     ESP_LOGI(TAG, "Using SPI peripheral");

//     sdmmc_host_t host = SDSPI_HOST_DEFAULT();
//     spi_bus_config_t bus_cfg = {
//         .mosi_io_num = 23,
//         .miso_io_num = 19,
//         .sclk_io_num = 18,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1,
//         .max_transfer_sz = 4000,
//     };

//     esp_err_t ret = spi_bus_initialize(host.slot, &bus_cfg, 1);
//     if (ret != ESP_OK)
//     {
//         ESP_LOGE(TAG, "Failed to initialize bus.");
//     }

//     // This initializes the slot without card detect (CD) and write protect (WP) signals.
//     // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
//     sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
//     slot_config.gpio_cs = 14;
//     slot_config.host_id = host.slot;
//     esp_err_t err = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
// }

void app_main(void)
{

    // sdcard_mount();

     //input_io_config();

     
    lora_uart_config();
    nvs_flash_init();

    // if(MQTT_CONNECTED==0){
    //        soft_timer_handle_1 = xTimerCreate(RECONNECT_BROKER_TIMER,pdMS_TO_TICKS(10000),false,(void *)1, &vSoftTimerCallback);
    // }

    //start_connect_ethernet();

   // vTaskDelay(3000 / portTICK_PERIOD_MS);
    // wifi_init();
     //mqtt_app_start();

    //xTaskCreate(event_input_gpio, "event_input_gpio", 2048, NULL, 1, NULL);
    // xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 2, NULL);
     xTaskCreate(lora_task, "lora_task", 4096, NULL, 4, NULL);

    // xTaskCreate(&opcua_task, "opcua_task", 4096, NULL, 5, NULL);

    // xTaskCreatePinnedToCore(opcua_task, "opcua_task", 2048 * 4, NULL, 10, NULL, 0); // Core0
}