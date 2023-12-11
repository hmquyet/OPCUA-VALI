#include "wifi_connect.h"
static const char *TAGWIFI = "WIFI_ESP";
int retry_cnt = 0;
#include "tcpip_adapter.h"

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