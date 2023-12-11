
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

#include "open62541.h"

#include "ethernet_connect.h"
#include "wifi_connect.h"
#include "mqtt_connect.h"
#include "lorasx1278.h"
#include "input.h"

static void IRAM_ATTR gpio_input_handler(void *arg)
{

    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}
void input_io_config()
{

    gpio_config_t io_conf = {};
    // interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    // bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    // set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    // enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    // change gpio intrrupt type for one pin
    gpio_set_intr_type(gpio_input_num, GPIO_INTR_ANYEDGE);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    // start gpio task

    // install gpio isr service
    gpio_install_isr_service(0);
    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(gpio_input_num, gpio_input_handler, (void *)gpio_input_num);

    // remove isr handler for gpio number.
    gpio_isr_handler_remove(gpio_input_num);
    // hook isr handler for specific gpio pin again
    gpio_isr_handler_add(gpio_input_num, gpio_input_handler, (void *)gpio_input_num);
}

