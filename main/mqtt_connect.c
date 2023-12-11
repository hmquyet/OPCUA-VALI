#include "mqtt_connect.h"
static const char *TAGMQTT = "MQTT_ESP";

char topic_Sub[12];
char data_Sub[12];


// int SW_STATE;
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAGMQTT, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
   esp_mqtt_client_handle_t client = event->client;

    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAGMQTT, "MQTT_EVENT_CONNECTED");
       
        // esp_mqtt_client_subscribe(client, MQTT_PUB_HUM_DHT, 0);
        // esp_mqtt_client_subscribe(client, MQTT_PUB_TEMP_DHT, 0);

        MQTT_CONNECTED = 1;
        break;

    case MQTT_EVENT_DISCONNECTED:
        MQTT_CONNECTED = 0;
        ESP_LOGI(TAGMQTT, "MQTT_EVENT_DISCONNECTED");

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
        .uri = "mqtt://test.mosquitto.org:1883"};

    mqttclient = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(mqttclient, ESP_EVENT_ANY_ID, mqtt_event_handler, mqttclient);
    esp_mqtt_client_start(mqttclient);
}


