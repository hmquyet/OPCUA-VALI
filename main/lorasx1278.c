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

#include "lorasx1278.h"
#include "input.h"
#include "esp_system.h"

#include <esp_crc.h>

static const char *TAGLORA = "LORA_ESP";
int status =1 ; 
void lora_uart_config()
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    // Install UART driver, and get the queue.
    uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 10, &uart2_queue, 0);
    uart_param_config(UART_NUM, &uart_config);
    // Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(UART_NUM, TX_GPIO_NUM, RX_GPIO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    uart_enable_pattern_det_baud_intr(UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);
    // Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(UART_NUM, 20);
}

void lora_tranmister_uart()
{
    char data_tranmister[300];

//node 1
//  snprintf(data_tranmister, sizeof(data_tranmister), "!ESP1:Temperature:1931964921981652422321745080121137167311687222892531123710394221#");
//     uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
//     printf(data_tranmister);
//     vTaskDelay(2000 / portTICK_RATE_MS);
//     snprintf(data_tranmister, sizeof(data_tranmister), "!ESP1:Humidity:4021217723315914211724122679231241174181796533106131165392051665177#");
//     uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
//      printf(data_tranmister);
//     vTaskDelay(2000 / portTICK_RATE_MS);
//     snprintf(data_tranmister, sizeof(data_tranmister), "!ESP1:Pressure:98132043181168441161810624188145240235166601397122217211020646151#");
//     uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
//      printf(data_tranmister);
//     vTaskDelay(2000 / portTICK_RATE_MS);
//     snprintf(data_tranmister, sizeof(data_tranmister), "!ESP1:Cycletime:3218916124246127152143798622155132107139181195671492910819323112101#");
//     uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
//      printf(data_tranmister);
//     vTaskDelay(2000 / portTICK_RATE_MS);
//     snprintf(data_tranmister, sizeof(data_tranmister), "!ESP1:HubCheck:9715520222014111735231250219#");
//     uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
//      printf(data_tranmister); 
//Node 2
    snprintf(data_tranmister, sizeof(data_tranmister), "!ESP2:Temperature:1931964921981652422321745080121137167311687222892531123710394221#");
    uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
    printf(data_tranmister);
    vTaskDelay(2000 / portTICK_RATE_MS);
    snprintf(data_tranmister, sizeof(data_tranmister), "!ESP2:Humidity:4021217723315914211724122679231241174181796533106131165392051665177#");
    uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
     printf(data_tranmister);
    vTaskDelay(2000 / portTICK_RATE_MS);
    snprintf(data_tranmister, sizeof(data_tranmister), "!ESP2:Pressure:98132043181168441161810624188145240235166601397122217211020646151#");
    uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
     printf(data_tranmister);
    vTaskDelay(2000 / portTICK_RATE_MS);
    snprintf(data_tranmister, sizeof(data_tranmister), "!ESP2:Cycletime:3218916124246127152143798622155132107139181195671492910819323112101#");
    uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
     printf(data_tranmister);
    vTaskDelay(2000 / portTICK_RATE_MS);
    snprintf(data_tranmister, sizeof(data_tranmister), "!ESP2:HubCheck:9715520222014111735231250219#");
    uart_write_bytes(UART_NUM, (const char *)data_tranmister, strlen(data_tranmister));
     printf(data_tranmister);   
    
}

void processing_string_Receive(char inputStrings[])
{

    if(strcmp(inputStrings, "node3\r\n") == 0){
       // printf("vo dươc day");
        
         lora_tranmister_uart();
         
    }
    // typedef struct
    // {
    //     char name[50];
    //     char value[50];
    //     char timestamp[100];
    // } Node;
    // Node nodes[5];
    // int nodeCount = 0;
    // char *nodePos = strstr(inputStrings, "!node");

    // if ((inputStrings[0] == '!') && (inputStrings[strlen(inputStrings) - 1] == '#'))
    // {
    //     for (int i = 0; i < strlen(inputStrings); i++)
    //     {

    //         if (nodePos != NULL)
    //         {

    //             int nodeIndex = inputStrings[5] - '0' - 1;

    //             // Xử lý tên, giá trị và timestamp từ chuỗi ban đầu
    //             if (strstr(inputStrings, "name:") != NULL)
    //             {
    //                 char *indexStart = strstr(inputStrings, ":") + 5 + 1;
    //                 char *indexEnd = strstr(inputStrings, "#");

    //                 strncpy(nodes[nodeIndex].name, indexStart, indexEnd - indexStart);
    //                 nodes[nodeIndex].name[indexEnd - indexStart] = '\0';
    //             }
    //             else if (strstr(inputStrings, "value:") != NULL)
    //             {
    //                 char *indexStart = strstr(inputStrings, ":") + 6 + 1;
    //                 char *indexEnd = strstr(inputStrings, "#");

    //                 strncpy(nodes[nodeIndex].value, indexStart, indexEnd - indexStart);
    //                 nodes[nodeIndex].value[indexEnd - indexStart] = '\0';
    //             }
    //             else if (strstr(inputStrings, "timestamp:") != NULL)
    //             {
    //                 char *indexStart = strstr(inputStrings, ":") + 10 + 1;
    //                 char *indexEnd = strstr(inputStrings, "#");

    //                 strncpy(nodes[nodeIndex].timestamp, indexStart, indexEnd - indexStart);
    //                 nodes[nodeIndex].timestamp[indexEnd - indexStart] = '\0';
    //             }

    //             // Ghép chuỗi JSON cho từng node
    //         }
    //     }

    //     char node1Json[500];
    //     snprintf(node1Json, sizeof(node1Json), "[{\"name\":\"%s\",\"value\":%s,\"timestamp\":%s}]\n", nodes[0].name, nodes[0].value, nodes[0].timestamp);
    //     printf(node1Json);
    //     char node2Json[500];
    //     snprintf(node2Json, sizeof(node2Json), "[{\"name\":\"%s\",\"value\":%s,\"timestamp\":%s}]\n\n", nodes[1].name, nodes[1].value, nodes[1].timestamp);
    //     printf(node2Json);
    // }


}

void lora_reciever_uart(char *data)
{

    size_t buffered_size;
    uint8_t *dtmp = (uint8_t *)malloc(RD_BUF_SIZE);
    for (;;)
    {
        // Waiting for UART event.
        
          
        if (xQueueReceive(uart2_queue, (void *)&event, (portTickType)portMAX_DELAY))
        {
            // Clear the memory block
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI("UART", "uart[%d] event:", UART_NUM);
            switch (event.type)
            {
            // Event of UART receving data
            case UART_DATA:
                vTaskDelay(100 / portTICK_RATE_MS);
                ESP_LOGI(TAGLORA, "[UART DATA]: %d", event.size);
                int len = uart_read_bytes(UART_NUM, dtmp, event.size, portMAX_DELAY);
                data = (char *)dtmp;
                printf(data);
             
                processing_string_Receive(data);
     

                break;
            // Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAGLORA, "hw fifo overflow");
                break;
            // Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAGLORA, "ring buffer full");
                break;
            // Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAGLORA, "uart rx break");
                break;
            // Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAGLORA, "uart parity error");
                break;
            // Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAGLORA, "uart frame error");
                break;
            // UART_PATTERN_DET
            case UART_PATTERN_DET:
                uart_get_buffered_data_len(UART_NUM, &buffered_size);
                int pos = uart_pattern_pop_pos(UART_NUM);
                ESP_LOGI("UART", "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                if (pos == -1)
                {

                    uart_flush_input(UART_NUM);
                }

                else
                {
                    uart_read_bytes(UART_NUM, dtmp, pos, 100 / portTICK_PERIOD_MS);
                    uint8_t pat[PATTERN_CHR_NUM + 1];
                    memset(pat, 0, sizeof(pat));
                    uart_read_bytes(UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                    ESP_LOGI(TAGLORA, "read data: %s", dtmp);
                    ESP_LOGI(TAGLORA, "read pat : %s", pat);
                }
                break;
            // Others
            default:
                ESP_LOGI("UART", "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}
