#include "sensorArray.h"



sensorArray::sensorArray(struct MuxConfiguration muxConfig, 
                         uint32_t sensors, adc1_channel_t pAdcChannel)
{
    muxPrivConfig = muxConfig;
    numSensors = sensors;
    adcChannel = pAdcChannel;
}

/**
 * @brief Initalize the all software and hardware: GPIOs, PWMs, ADCs,...
 */
void sensorArray::init()
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(adcChannel, ADC_ATTEN_DB_11);

    // Tema 10: Mux
    // Definir pines Sn
    gpio_config_t config;
    config.mode = GPIO_MODE_OUTPUT;
    config.pin_bit_mask = 
        (((uint64_t)1) << muxPrivConfig.S0) |  
        (((uint64_t)1) << muxPrivConfig.S1) |  
        (((uint64_t)1) << muxPrivConfig.S2) |  
        (((uint64_t)1) << muxPrivConfig.S3);

    config.intr_type = GPIO_INTR_DISABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&config);
}

// Resto de funciones.

void sensorArray::calibrate()
{

}

int32_t sensorArray::getLinePosition()
{
    int32_t acum = 0;
    int32_t weightened = 0;
    uint16_t adcValue[numSensors];

    uint16_t sensorOffset = 3; // Mi sensor 0 es el C3 de la placa de expansion
    uint16_t currentSensor = 0;
    uint16_t currentValue = 0;
    printf("LineData: ");
    for( uint32_t i = 0; i < numSensors; i++)
    {
        currentSensor = sensorOffset + i;
        gpio_set_level(muxPrivConfig.S0, currentSensor & 0x1U );
        gpio_set_level(muxPrivConfig.S1, (currentSensor>>1) & 0x1U );
        gpio_set_level(muxPrivConfig.S2, (currentSensor>>2) & 0x1U );
        gpio_set_level(muxPrivConfig.S3, (currentSensor>>3) & 0x1U );

        currentValue = adc1_get_raw(adcChannel); 
        //adcValue[i] = currentValue;

        if(currentValue > 2048)
        {
            weightened += ((float)currentValue)*((float)i - ((numSensors-1)/2.0f))*100.0f;
            acum += currentValue;
        }

        printf("%d ", adcValue[i]);
    }

    printf("%.2f ", (float)weightened/acum);
    return (int32_t)weightened/acum;
}