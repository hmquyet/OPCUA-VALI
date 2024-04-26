#include "esp_err.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"
#include <lwip/sockets.h>
#include "freertos/event_groups.h"

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
#include "esp_task_wdt.h"

#include "driver/uart.h"
#include "esp_log.h"

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

#include "driver/spi_master.h"
#include "esp_eth.h"
#include "lwip/inet.h"

// #ifdef CONFIG_EXAMPLE_CONNECT_WIFI
// #include "esp_wifi.h"
// #include "esp_wifi_default.h"
// #define BASE_IP_EVENT WIFI_EVENT
// #define GOT_IP_EVENT IP_EVENT_STA_GOT_IP
// #define DISCONNECT_EVENT WIFI_EVENT_STA_DISCONNECTED
// #define EXAMPLE_INTERFACE TCPIP_ADAPTER_IF_STA
// #endif

int ETHERNET_CONNECTED ;

void start_connect_ethernet(void);
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data);
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data);