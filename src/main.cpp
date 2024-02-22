#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <freertos/queue.h>
#include <esp_log.h>

// Tema 7
#include <driver/ledc.h>

// Tema 9
#include <driver/adc.h>


constexpr gpio_num_t LED_PIN = GPIO_NUM_2;
constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_13;

constexpr gpio_num_t AIN1 = GPIO_NUM_25;
constexpr gpio_num_t AIN2 = GPIO_NUM_33;
constexpr gpio_num_t BIN1 = GPIO_NUM_26;
constexpr gpio_num_t BIN2 = GPIO_NUM_27;

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

    // Tema 5 y Puente H
    ledc_timer_config_t ledcTimer;
    ledcTimer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledcTimer.freq_hz = 500;
    ledcTimer.duty_resolution = LEDC_TIMER_10_BIT;
    ledcTimer.clk_cfg = LEDC_AUTO_CLK;
    ledcTimer.timer_num = LEDC_TIMER_0;
    ledc_timer_config(&ledcTimer);

    ledc_channel_config_t ledcChannel;
    ledcChannel.channel = LEDC_CHANNEL_0;
    ledcChannel.gpio_num = GPIO_NUM_32;
    ledcChannel.duty = 0;
    ledcChannel.timer_sel = LEDC_TIMER_0;
    ledcChannel.intr_type = LEDC_INTR_DISABLE;
    ledcChannel.speed_mode = LEDC_LOW_SPEED_MODE;
    ledcChannel.flags.output_invert = 0;
    ledc_channel_config(&ledcChannel);

    ledcChannel.channel = LEDC_CHANNEL_1;
    ledcChannel.gpio_num = GPIO_NUM_14;
    ledc_channel_config(&ledcChannel);


    // Tema Puente H:
    // Definir pines AIN1, AIN2, BIN1, BIN2
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = (((uint64_t)1) << AIN1) |  (((uint64_t)1) << AIN2) |  (((uint64_t)1) << BIN1)|  (((uint64_t)1) << BIN2);
    config.intr_type = GPIO_INTR_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&config);

    uint8_t a1 = 0;
    uint8_t a2 = 1;
    uint8_t b1 = 0;
    uint8_t b2 = 1;
    gpio_set_level(AIN1, a1);
    gpio_set_level(AIN2, a2);
    gpio_set_level(BIN1, b1);
    gpio_set_level(BIN2, b2);


    // Tema 9: ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);


    uint32_t cnt=0;
    int32_t dir=1;

    uint16_t adcValue;

    // Loop
    while(1)
    {   
        
        // Ejercicio tema 4
        // Set LED_PIN to INPUT value.
        // If INPUT == 1 then LED_PIN = 1
        // If INPUT == 0 then LED_PIN = 0
        
        //gpio_set_level(LED_PIN , !gpio_get_level(BUTTON_PIN));

        vTaskDelay(pdMS_TO_TICKS(10));
        cnt += dir;
        
        /*ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, cnt);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, cnt);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

        if (cnt > 320 && dir == 1)
        {
            dir = -1;
        }
        else if(cnt == 50 && dir == -1)
        {
            dir = 1;
            a1 = (a1==1)?0:1;
            b1 = (b1==1)?0:1;
            a2 = (a2==1)?0:1;
            b2 = (b2==1)?0:1;
            gpio_set_level(AIN1, a1);
            gpio_set_level(AIN2, a2);
            gpio_set_level(BIN1, b1);
            gpio_set_level(BIN2, b2);
        }*/

        adcValue = adc1_get_raw(ADC1_CHANNEL_0);
        printf("Value: %d\n", adcValue);

        /*gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(300));*/
    }
    
}