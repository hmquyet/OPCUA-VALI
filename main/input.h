#ifndef INPUT__H
#define INPUT__H
#include "esp_err.h"

#include "hal/gpio_types.h"

#define gpio_input_num GPIO_NUM_32
#define GPIO_INPUT_PIN_SEL ((1ULL << gpio_input_num))
typedef enum
{
    LOW_TO_HIGH = 1,
    HIGH_TO_LOW = 2,
    ANY_EDLE = 3
} interrupt_type_edle_t;

static xQueueHandle gpio_evt_queue = NULL;
void input_io_config();
static void IRAM_ATTR gpio_input_handler();

// int input_io_get_level(gpio_num_t gpio_num);
// void input_set_callback(void *cb);

#endif