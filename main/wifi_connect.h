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

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "ethernet_connect.h"
#include "mqtt_connect.h"



#define MAX_RETRY 10
#define EXAMPLE_ESP_WIFI_SSID "DESKTOP"      // PDA_CHA_NhaDuoi
#define EXAMPLE_ESP_WIFI_PASS "1111111" // Tiaportal

int WIFI_CONNECTED ;
void wifi_init(void);
esp_err_t wifi_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data);