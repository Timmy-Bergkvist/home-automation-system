#include <stdio.h>
#include "PirSensor.h"
#include "esp_log.h"
#include "../../components/config.h"

static const char* TAG = "PIR Sensor";

PirSensor::PirSensor(gpio_num_t pin) : gpioPin(pin)
{
}

PirSensor::~PirSensor()
{
}

void PirSensor::init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << gpioPin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    ESP_LOGI(TAG, "PIR sensor initialized on GPIO %d", gpioPin);
}

int PirSensor::read()
{
    return gpio_get_level(gpioPin);
}
