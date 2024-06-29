#ifndef CONFIG_H
#define CONFIG_H

#include "driver/i2c.h"

// DHT-11 (Temperature & humidity sensor) pin
#define DHT11_PIN                   GPIO_NUM_18

// I2C pin definitions LCD1602
#define I2C_LCD_ADDRESS             0x27        // LCD1602 I2C address
#define I2C_MASTER_SCL_IO           GPIO_NUM_14 // gpio number for I2C master clock
#define I2C_MASTER_SDA_IO           GPIO_NUM_13 // gpio number for I2C master data
#define I2C_MASTER_NUM              I2C_NUM_0   // I2C port number for master dev
#define I2C_MASTER_FREQ_HZ          400000      // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE   0           // I2C master doesn't need buffer
#define I2C_MASTER_RX_BUF_DISABLE   0           // I2C master doesn't need buffer

// PIR sensor pin
#define PIR_SENSOR_PIN              GPIO_NUM_27

// Relay pin
#define RELAY_PIN                   GPIO_NUM_12

// RGB LED Pins
#define RGB_LED_RED_PIN             GPIO_NUM_15
#define RGB_LED_GREEN_PIN           GPIO_NUM_2
#define RGB_LED_BLUE_PIN            GPIO_NUM_4

#endif
