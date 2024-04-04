#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <freertos/queue.h>
#include <esp_log.h>
#include <esp_timer.h>

// Tema 7
#include <driver/ledc.h>

// Tema 9
#include <driver/adc.h>

// Tema 11
#include "pid.h"


constexpr gpio_num_t LED_PIN = GPIO_NUM_2;
constexpr gpio_num_t BUTTON_PIN = GPIO_NUM_13;

constexpr gpio_num_t AIN1 = GPIO_NUM_25;
constexpr gpio_num_t AIN2 = GPIO_NUM_33;
constexpr gpio_num_t BIN1 = GPIO_NUM_26;
constexpr gpio_num_t BIN2 = GPIO_NUM_27;

// Tmea 10: Mux
constexpr uint32_t NUM_SENSORS = 8;

constexpr gpio_num_t S0 = GPIO_NUM_22;
constexpr gpio_num_t S1 = GPIO_NUM_21;
constexpr gpio_num_t S2 = GPIO_NUM_19;
constexpr gpio_num_t S3 = GPIO_NUM_18;

// Tema 11: PID
PID pid(50.0f, 0.0f, 0.0f);

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

    // Tema 10: Mux
    // Definir pines Sn
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = (((uint64_t)1) << S0) |  (((uint64_t)1) << S1) |  (((uint64_t)1) << S2)|  (((uint64_t)1) << S3);
    config.intr_type = GPIO_INTR_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&config);

    uint32_t cnt=0;
    int32_t dir=1;

    uint16_t adcValue[NUM_SENSORS];
    uint16_t sensorOffset = 3; // Mi sensor 0 es el C3 de la placa de expansion
    uint16_t currentSensor = 0;
    uint16_t currentValue = 0;

    float weightened = 0;
    float acum = 0;
    uint64_t last_call = esp_timer_get_time();
    uint64_t curr_time = esp_timer_get_time();
    uint8_t first_run = 1;
    float result = 0;
    // Loop
    while(1)
    {   
        
        // Ejercicio tema 4
        // Set LED_PIN to INPUT value.
        // If INPUT == 1 then LED_PIN = 1
        // If INPUT == 0 then LED_PIN = 0
        
        //gpio_set_level(LED_PIN , !gpio_get_level(BUTTON_PIN));

        //vTaskDelay(pdMS_TO_TICKS(10));
        
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
        acum = 0;
        weightened = 0;
        printf("LineData: ");
        for( uint32_t i = 0; i < NUM_SENSORS; i++)
        {
            currentSensor = sensorOffset + i;
            gpio_set_level(S0, currentSensor & 0x1U );
            gpio_set_level(S1, (currentSensor>>1) & 0x1U );
            gpio_set_level(S2, (currentSensor>>2) & 0x1U );
            gpio_set_level(S3, (currentSensor>>3) & 0x1U );

            currentValue = adc1_get_raw(ADC1_CHANNEL_0); 
            adcValue[i] = currentValue;

            if(currentValue > 2048)
            {
                weightened += ((float)currentValue)*((float)i - ((NUM_SENSORS-1)/2.0f))*100.0f;
                acum += currentValue;
            }

            printf("%d ", adcValue[i]);
        }

        printf("%.2f ", weightened/acum);


        if(!first_run)
        {
            curr_time = esp_timer_get_time();
            result = pid.update(weightened/acum, curr_time-last_call);
            result = result / 100.0f;
            
            if (result > 100){
                result = 100;
            }
        }
        printf("%.2f ", result);
        printf("\n");
        last_call = curr_time;
        first_run = 0;

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 300+(int)result);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 300-(int)result);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);

        // Tema 11: PID
        // Calcular valor de la linea:


        /*gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(300));*/

        vTaskDelay(2);
    }
    
}