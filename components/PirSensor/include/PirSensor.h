#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include "driver/gpio.h"

class PirSensor
{
public:
    PirSensor(gpio_num_t pin);
    ~PirSensor();

    void init();
    int read();

private:
    gpio_num_t gpioPin;
};

#endif