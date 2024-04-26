
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

#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "freertos/event_groups.h"
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
#include "esp_timer.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "open62541.h"
#include "ethernet_connect.h"
#include "ds3231.h"

static const char *TAGWIFI = "WIFI_ESP";
int retry_cnt = 0;
static const char *TAGMQTT = "MQTT_ESP";

char topic_Sub[12];
char data_Sub[12];
i2c_dev_t rtc_i2c;
#include "tcpip_adapter.h"

const char *RECONNECT_BROKER_TIMER = "RECONNECT_BROKER_TIMER";
static const char *TAG = "TEST_ESP";

static char DATA_FILE[50] = "/sdcard/dataopc.txt";

static char *received_data;

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

int MQTT_CONNECTED;
static esp_mqtt_client_handle_t mqttclient;
void mqtt_app_start(void);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

#define MAX_RETRY 10
#define EXAMPLE_ESP_WIFI_SSID "IPHONE1"  // PDA_CHA_NhaDuoi
#define EXAMPLE_ESP_WIFI_PASS "12345679" // Tiaportal

int reconnect_time;
bool boot_to_reconnect;
TimerHandle_t soft_timer_handle_1;
TimerHandle_t soft_timer_handle_7;

struct tm local_time;

int WIFI_CONNECTED;
void wifi_init(void);
esp_err_t wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

static void vSoftTimerCallback(TimerHandle_t xTimer);

static bool IRAM_ATTR timer_group_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    return high_task_awoken == pdTRUE; // return whether we need to yield at the end of ISR
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
            if (ETHERNET_CONNECTED ==1)
            {

                // LIST NODE ID
                UA_NodeId nodeIds[10];

                nodeIds[0] = UA_NODEID_NUMERIC(4, 9);  // tmChargeTime
                nodeIds[1] = UA_NODEID_NUMERIC(4, 10); // tmClpClsTime
                nodeIds[2] = UA_NODEID_NUMERIC(4, 8);  // tmClpOpnTime
                nodeIds[3] = UA_NODEID_NUMERIC(4, 15); // tmCoolingTime
                nodeIds[4] = UA_NODEID_NUMERIC(4, 7);  // tmInjTime
                nodeIds[5] = UA_NODEID_NUMERIC(4, 16); //  Injection Peak Pressure
                nodeIds[6] = UA_NODEID_NUMERIC(4, 14); // cycleTime
                nodeIds[7] = UA_NODEID_NUMERIC(4, 12); // counterShot
                nodeIds[8] = UA_NODEID_NUMERIC(4, 11); // Nozzle Temp
                nodeIds[9] = UA_NODEID_NUMERIC(4, 13); // Switch Over Pos

                for (int i = 0; i < 10; i++)
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
                                    ds3231_get_time(&rtc_i2c, &local_time);
                                    snprintf(data_receiver_opcua, sizeof(data_receiver_opcua), "{\"name\":\"%s\",\"value\":%f,\"timestamp\":\"20%04d-%02d-%02d %02d:%02d:%02d\"}",
                                             nodeName, floatValue, local_time.tm_year, local_time.tm_mon, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
                                    printf(data_receiver_opcua);
                                    switch (i)
                                    {
                                    case 0:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmChargeTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 1:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmClpClsTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 2:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmClpOpnTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 3:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmCoolingTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 4:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmInjTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 5:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/InjectionPeakPressure", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 6:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/cycleTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 7:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/counterShot", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 8:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/NozzleTemp", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 9:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/SwitchOverPos", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    default:
                                        break;
                                    }
                                }

                                else if (UA_Variant_hasScalarType(value, &UA_TYPES[UA_TYPES_DOUBLE]))
                                {
                                    float doubleValue = *(float *)value->data;

                                    char data_receiver_opcua[100];
                                    ds3231_get_time(&rtc_i2c, &local_time);
                                    snprintf(data_receiver_opcua, sizeof(data_receiver_opcua), "{\"name\":\"%s\",\"value\":%f,\"timestamp\":\"20%04d-%02d-%02d %02d:%02d:%02d\"}",
                                             nodeName, doubleValue, local_time.tm_year, local_time.tm_mon, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
                                    printf(data_receiver_opcua);

                                    switch (i)
                                    {
                                    case 0:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmChargeTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 1:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmClpClsTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 2:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmClpOpnTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 3:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmCoolingTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 4:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmInjTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 5:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/InjectionPeakPressure", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 6:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/cycleTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 7:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/counterShot", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 8:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/NozzleTemp", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 9:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/SwitchOverPos", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    default:
                                        break;
                                    }
                                }

                                else if (UA_Variant_hasScalarType(value, &UA_TYPES[UA_TYPES_UINT16]))
                                {

                                    char data_receiver_opcua[100];
                                    u_int16_t intValue = *(u_int16_t *)value->data;
                                    ds3231_get_time(&rtc_i2c, &local_time);

                                    snprintf(data_receiver_opcua, sizeof(data_receiver_opcua), "{\"name\":\"%s\",\"value\":%d,\"timestamp\":\"20%04d-%02d-%02d %02d:%02d:%02d\"}",
                                             nodeName, intValue, local_time.tm_year, local_time.tm_mon, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
                                    printf(data_receiver_opcua);

                                    switch (i)
                                    {
                                    case 0:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmChargeTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 1:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmClpClsTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 2:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmClpOpnTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 3:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmCoolingTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 4:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/tmInjTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 5:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/InjectionPeakPressure", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 6:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/cycleTime", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 7:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/counterShot", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 8:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/NozzleTemp", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    case 9:
                                        esp_mqtt_client_publish(mqttclient, "HAITHIEN/I1/Metric/SwitchOverPos", data_receiver_opcua, 0, 0, 1);
                                        break;
                                    default:
                                        break;
                                    }
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
}

void wifi_init(void)
{

    esp_event_loop_create_default();
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK},
    };

    esp_netif_init();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}

esp_err_t wifi_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI(TAGWIFI, " to Tryingconnect with Wi-Fi\n");
        break;

    case WIFI_EVENT_STA_CONNECTED:
        WIFI_CONNECTED = 1;
        ESP_LOGI(TAGWIFI, "Wi-Fi connected\n");
        ESP_LOGI(TAGWIFI, " Connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

        break;

    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI(TAGWIFI, "got ip: starting MQTT Client\n");

        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        WIFI_CONNECTED = 0;
        ESP_LOGI(TAGWIFI, "wifi disconnected: Retrying Wi-Fi\n");
        if (retry_cnt++ < MAX_RETRY)
        {
            esp_wifi_connect();
        }
        else
            ESP_LOGI(TAGWIFI, "Max Retry Failed: Wi-Fi Connection\n");
        break;

    default:
        break;
    }
    return ESP_OK;
}

// int SW_STATE;
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAGMQTT, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case WIFI_EVENT_STA_START:
        esp_wifi_connect();
        reconnect_time = 0;
        ESP_LOGI(TAGWIFI, "Trying to connect with Wi-Fi\n");
        break;

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAGMQTT, "MQTT_EVENT_CONNECTED");
        boot_to_reconnect = false;
        xTimerStop(soft_timer_handle_7, 10);

        break;
    case MQTT_EVENT_SUBSCRIBED:
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAGMQTT, "MQTT_EVENT_ERROR");
        break;

    case MQTT_EVENT_DATA:
        sprintf(topic_Sub, "%.*s\r\n", event->topic_len, event->topic);
        sprintf(data_Sub, "%.*s\r\n", event->data_len, event->data);
        ESP_LOGI(TAGMQTT, "MQTT_EVENT_DATA");
        // printf("Topic: %s\n", topic_Sub);
        // printf("Data: %s\n", data_Sub);
        break;

    default:
        ESP_LOGI(TAGMQTT, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{

    ESP_LOGI(TAGMQTT, "STARTING MQTT");
    esp_mqtt_client_config_t mqttConfig = {
        .uri = "mqtt://40.82.154.13:1883"};

    mqttclient = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(mqttclient, ESP_EVENT_ANY_ID, mqtt_event_handler, mqttclient);
    esp_mqtt_client_start(mqttclient);
}

void app_main(void)
{

    esp_err_t err = nvs_flash_init();
    soft_timer_handle_7 = xTimerCreate("BOOT_CONNECT_TIMER", pdMS_TO_TICKS(5000), false, (void *)7, &vSoftTimerCallback); // Timer Reboot to connect

    wifi_init();
    mqtt_app_start();
    start_connect_ethernet();

    xTaskCreate(&opcua_task, "opcua_task", 4096, NULL, 5, NULL);
    // xTaskCreatePinnedToCore(opcua_task, "opcua_task", 2048 * 4, NULL, 10, NULL, 0); // Core0
}
