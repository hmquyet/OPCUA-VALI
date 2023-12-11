

#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"
#include <lwip/sockets.h>
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"

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