#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>

//#define LED_PIN GPIO_NUM_2
constexpr gpio_num_t LED_PIN = GPIO_NUM_2;

extern "C" void app_main();

void app_main()
{

    // Setup
    gpio_config_t config;
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = (1<<LED_PIN);
    config.intr_type = GPIO_INTR_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&config);
    
    // Loop
    while(1)
    {   
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    
}
