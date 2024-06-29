#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "Dht11.h"
#include "../../components/config.h"

// Extra thanks to https://github.com/zorxx/dht/tree/main

static const char* TAG = "DHT-11";

#define DHT_TIMER_INTERVAL 2
#define DHT_DATA_BITS 40
#define DHT_DATA_BYTES (DHT_DATA_BITS / 8)

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#define PORT_ENTER_CRITICAL() portENTER_CRITICAL(&mux)
#define PORT_EXIT_CRITICAL() portEXIT_CRITICAL(&mux)

Dht11::Dht11(gpio_num_t pin) : dhtPin(pin) {}

Dht11::~Dht11() {}

void Dht11::init() 
{
    gpio_set_direction(dhtPin, GPIO_MODE_INPUT_OUTPUT);
    ESP_LOGI(TAG, "DHT11 initialized on GPIO %d", dhtPin);
}

void Dht11::startSignal()
{
    gpio_set_direction(dhtPin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dhtPin, 0);
    vTaskDelay(pdMS_TO_TICKS(18));  // 18ms delay
    gpio_set_level(dhtPin, 1);
    esp_rom_delay_us(40);  // 40us delay
    gpio_set_direction(dhtPin, GPIO_MODE_INPUT);
}

static esp_err_t dht_await_pin_state(gpio_num_t pin, uint32_t timeout, int expected_pin_state, uint32_t *duration)
{
    for (uint32_t i = 0; i < timeout; i += DHT_TIMER_INTERVAL)
    {
        esp_rom_delay_us(DHT_TIMER_INTERVAL);
        if (gpio_get_level(pin) == expected_pin_state)
        {
            if (duration)
                *duration = i;
            return ESP_OK;
        }
    }
    return ESP_ERR_TIMEOUT;
}

static esp_err_t dht_fetch_data(gpio_num_t pin, uint8_t data[DHT_DATA_BYTES])
{
    uint32_t low_duration;
    uint32_t high_duration;
    esp_err_t err;

    PORT_ENTER_CRITICAL();
    err = dht_await_pin_state(pin, 40, 0, NULL);
    if (err != ESP_OK) {
        PORT_EXIT_CRITICAL();
        ESP_LOGE(TAG, "Initialization error, problem in phase 'B'");
        return err;
    }

    err = dht_await_pin_state(pin, 88, 1, NULL);
    if (err != ESP_OK) {
        PORT_EXIT_CRITICAL();
        ESP_LOGE(TAG, "Initialization error, problem in phase 'C'");
        return err;
    }

    err = dht_await_pin_state(pin, 88, 0, NULL);
    if (err != ESP_OK) {
        PORT_EXIT_CRITICAL();
        ESP_LOGE(TAG, "Initialization error, problem in phase 'D'");
        return err;
    }

    for (int i = 0; i < DHT_DATA_BITS; i++)
    {
        err = dht_await_pin_state(pin, 65, 1, &low_duration);
        if (err != ESP_OK) {
            PORT_EXIT_CRITICAL();
            ESP_LOGE(TAG, "LOW bit timeout at bit %d", i);
            return err;
        }

        err = dht_await_pin_state(pin, 75, 0, &high_duration);
        if (err != ESP_OK) {
            PORT_EXIT_CRITICAL();
            ESP_LOGE(TAG, "HIGH bit timeout at bit %d", i);
            return err;
        }

        uint8_t b = i / 8;
        uint8_t m = i % 8;
        if (!m)
            data[b] = 0;

        data[b] |= (high_duration > low_duration) << (7 - m);
    }
    PORT_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t Dht11::readData(int *temperature, int *humidity)
{
    ESP_LOGI(TAG, "Requesting data from DHT11 sensor");

    if (!temperature || !humidity) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t data[DHT_DATA_BYTES] = {0};

    startSignal();  // Use the startSignal method to initiate communication

    esp_err_t result;

    PORT_ENTER_CRITICAL();
    result = dht_fetch_data(dhtPin, data);
    PORT_EXIT_CRITICAL();

    gpio_set_direction(dhtPin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(dhtPin, 1);

    if (result != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to fetch data from DHT11 sensor");
        return result;
    }

    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
    {
        ESP_LOGE(TAG, "Checksum failed, invalid data received from sensor");
        return ESP_ERR_INVALID_CRC;
    }

    *humidity = data[0];
    *temperature = data[2];

    ESP_LOGI(TAG, "Temperature: %d, Humidity: %d", *temperature, *humidity);

    return ESP_OK;
}
