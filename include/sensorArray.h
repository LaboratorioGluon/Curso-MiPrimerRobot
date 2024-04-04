#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <driver/gpio.h>
#include <driver/adc.h>
#include <stdint.h>

struct MuxConfiguration{
    gpio_num_t S0;
    gpio_num_t S1;
    gpio_num_t S2; 
    gpio_num_t S3;
};



class sensorArray
{
public:
    sensorArray( struct MuxConfiguration muxConfig, 
                 uint32_t sensors, adc1_channel_t pAdcChannel);

    /**
     * @brief Initalize the all software and hardware: GPIOs, PWMs, ADCs,...
     */
    void init();

    // Resto de funciones.

    void calibrate();

    int32_t getLinePosition();

    // Miembros públicos.

private:
    // Miembros privados.
    struct MuxConfiguration muxPrivConfig;
    uint32_t numSensors;
    adc1_channel_t adcChannel;

    
};

#endif