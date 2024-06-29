#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/ledc.h"
#include "Dht11.h"
#include "Lcd1602.h"
#include "PirSensor.h"
#include "Relay.h"
#include "Led.h"
#include "../components/config.h" // Pins definitions

static const char* TAG = "main";

Lcd1602 lcd;
Dht11 dht11(DHT11_PIN);
PirSensor pirSensor(PIR_SENSOR_PIN);
Relay relay(RELAY_PIN);
Led rgbLed;

bool motionDetected = false;

void updateDisplay(int temperature, int humidity);
void displayAlert();
void updateRgbLed(int temperature, bool motionDetected);
void pirSensorTask(void* pvPirSensorTask);
void lcdTask(void* pvLcdTaks);
void i2c_master_init();

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Application started");

    i2c_master_init();

    lcd.lcd_init();
    dht11.init();
    pirSensor.init();
    relay.init();
    rgbLed.init();

    updateDisplay(0, 0);

    xTaskCreate(pirSensorTask, "pirSensorTask", 2048, NULL, 5, NULL);
    xTaskCreate(lcdTask, "lcdTask", 2048, NULL, 10, NULL);
}


void lcdTask(void *pvLcdTaks) {
    int temperature = 0;
    int humidity = 0;

    while (1) {
        if (!motionDetected) {
            if (dht11.readData(&temperature, &humidity) == ESP_OK) {
                ESP_LOGI(TAG, "Temp: %d", temperature);
                ESP_LOGI(TAG, "Humi: %d", humidity);
                updateDisplay(temperature, humidity);
                updateRgbLed(temperature, motionDetected);
            } else {
                ESP_LOGE(TAG, "Failed to read from DHT11 sensor");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void updateDisplay(int temperature, int humidity)
{
    char buffer[16];
    lcd.lcd_clear();

    lcd.lcd_set_cursor(0, 0);
    lcd.lcd_write_string("Temperature: ");
    sprintf(buffer, "%dC", temperature);
    lcd.lcd_write_string(buffer);

    lcd.lcd_set_cursor(0, 1);
    lcd.lcd_write_string("Humidity:    ");
    sprintf(buffer, "%d%%", humidity);
    lcd.lcd_write_string(buffer);
}

void displayAlert()
{
    lcd.lcd_clear();
    lcd.lcd_set_cursor(0, 0);
    lcd.lcd_write_string("Motion Detected!");
}

void updateRgbLed(int temperature, bool motionDetected)
{
    if (temperature >= -15 && temperature <= 19)
    {
        rgbLed.setRgbColor(0, 0, 255); // Blue
    }
    else if (temperature >= 20 && temperature <= 25)
    {
        rgbLed.setRgbColor(0, 255, 0); // Green
    }
    else if (temperature >= 26)
    {
        rgbLed.setRgbColor(255, 0, 0); // Red
    }
}

// PIR sensor
void pirSensorTask(void* pvPirSensorTask)
{
    while (1) {
        int pir_state = pirSensor.read();

        if (pir_state == 1) {
            if (!motionDetected) {
                ESP_LOGI(TAG, "Motion detected! Relay on.");
                relay.on();
                displayAlert();
                motionDetected = true;
                rgbLed.setRgbColor(0, 0, 255); // Blue
            }
        } else {
            if (motionDetected) {
                ESP_LOGI(TAG, "No motion! Relay off.");
                relay.off();
                motionDetected = false;
                updateDisplay(0, 0);
                updateRgbLed(0, false);
            }
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void i2c_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

