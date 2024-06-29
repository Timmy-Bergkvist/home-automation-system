#ifndef LED_H
#define LEDH

#include "driver/gpio.h"
#include "driver/ledc.h"

class Led
{
public:
    Led();
    ~Led();
    void init();
    void setRgbColor(uint8_t red, uint8_t green, uint8_t blue);

private:

};

#endif