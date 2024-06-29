#include <stdio.h>
#include "Relay.h"
#include "esp_log.h"
#include "../../components/config.h"

static const char* TAG = "Relay";

Relay::Relay(gpio_num_t pin) : gpioPin(pin)
{
}

Relay::~Relay()
{
}

void Relay::init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << gpioPin);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    ESP_LOGI(TAG, "Relay initialized on GPIO %d", gpioPin);
}

void Relay::on()
{
    gpio_set_level(gpioPin, 1);
    ESP_LOGI(TAG, "Relay on");
}

void Relay::off()
{
    gpio_set_level(gpioPin, 0);
    ESP_LOGI(TAG, "Relay off");
}
