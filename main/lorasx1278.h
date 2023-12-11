#ifndef LORASX1278__H
#define LORASX1278__H
#include "esp_err.h"
#include "hal/gpio_types.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/uart.h"


#define UART_NUM UART_NUM_2
#define TX_GPIO_NUM (GPIO_NUM_17)
#define RX_GPIO_NUM (GPIO_NUM_16)
#define PATTERN_CHR_NUM (3)
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

static QueueHandle_t uart2_queue = NULL;
static uart_event_t event;

void lora_uart_config();

void processing_string_Receive(char inputStrings[]);
void lora_reciever_uart(char *received_data);
void lora_tranmister_uart();

#endif