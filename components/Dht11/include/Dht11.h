#ifndef DHT11_H
#define DHT11_H

#include "driver/gpio.h"
#include "esp_err.h"

class Dht11
{
public:
    Dht11(gpio_num_t pin);
    ~Dht11();
    void init();
    esp_err_t readData(int *temperature, int *humidity);

private:
    gpio_num_t dhtPin;
    void startSignal();
};

#endif
