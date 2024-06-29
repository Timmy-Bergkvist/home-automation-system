#ifndef RELAY_H
#define RELAY_H

#include "driver/gpio.h"

class Relay
{
public:
    Relay(gpio_num_t pin);
    ~Relay();

    void init();
    void on();
    void off();

private:
    gpio_num_t gpioPin;
};

#endif
