#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <freertos/queue.h>

constexpr gpio_num_t LED_PIN = GPIO_NUM_2;
constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_13;

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

    // Ejercicio tema 4:
    // Init GPIO El que querais as INPUT
    config.mode = GPIO_MODE_INPUT;
    config.pin_bit_mask = 1 << BUTTON_PIN;
    config.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&config);

    // Ejercicio tema 4:
    // Suponiendo que el led azul caen 2V, y que quereis que pasen 2mA
    // Que R necesitais
    // V= R*I => R = V/I; Vesp=3.3, Vl = 2, Il = 0.002; R = (3.3-2)/0.002=650 ~ 680 (E6)

    // Loop
    while(1)
    {   
        
        // Ejercicio tema 4
        // Set LED_PIN to INPUT value.
        // If INPUT == 1 then LED_PIN = 1
        // If INPUT == 0 then LED_PIN = 0
        
        gpio_set_level(LED_PIN , !gpio_get_level(BUTTON_PIN));

        vTaskDelay(pdMS_TO_TICKS(10));
        /*gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(300));*/
    }
    
}