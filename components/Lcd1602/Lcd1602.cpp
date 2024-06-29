#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "Lcd1602.h"
#include "../../components/config.h"

// Commands
#define LCD_CMD_CLEAR_DISPLAY       0x01
#define LCD_CMD_RETURN_HOME         0x02
#define LCD_CMD_ENTRY_MODE_SET      0x04
#define LCD_CMD_DISPLAY_CONTROL     0x08
#define LCD_CMD_CURSOR_SHIFT        0x10
#define LCD_CMD_FUNCTION_SET        0x20
#define LCD_CMD_SET_CGRAM_ADDR      0x40
#define LCD_CMD_SET_DDRAM_ADDR      0x80
 
#define LCD_FLAG_DISPLAY_ON         0x04
#define LCD_FLAG_DISPLAY_OFF        0x00
#define LCD_FLAG_CURSOR_ON          0x02
#define LCD_FLAG_CURSOR_OFF         0x00
#define LCD_FLAG_BLINK_ON           0x01
#define LCD_FLAG_BLINK_OFF          0x00
 
#define LCD_FLAG_8BIT_MODE          0x10
#define LCD_FLAG_4BIT_MODE          0x00
#define LCD_FLAG_2LINE              0x08
#define LCD_FLAG_1LINE              0x00
#define LCD_FLAG_5x10DOTS           0x04
#define LCD_FLAG_5x8DOTS            0x00

static const char* TAG = "LCD1602";

Lcd1602::Lcd1602() {}

Lcd1602::~Lcd1602() {}

esp_err_t Lcd1602::i2c_lcd_write_byte(uint8_t data) 
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_LCD_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    return ret;
}

void Lcd1602::lcd_send_command(uint8_t cmd) 
{
    uint8_t data[4];
    data[0] = (cmd & 0xF0) | 0x0C;
    data[1] = (cmd & 0xF0) | 0x08;
    data[2] = ((cmd << 4) & 0xF0) | 0x0C;
    data[3] = ((cmd << 4) & 0xF0) | 0x08;

    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    i2c_master_start(handle);
    i2c_master_write_byte(handle, (I2C_LCD_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(handle, data, sizeof(data), true);
    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_NUM, handle, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(handle);
}

void Lcd1602::lcd_send_data(uint8_t data) 
{
    uint8_t data_arr[4];
    data_arr[0] = (data & 0xF0) | 0x0D;
    data_arr[1] = (data & 0xF0) | 0x09;
    data_arr[2] = ((data << 4) & 0xF0) | 0x0D;
    data_arr[3] = ((data << 4) & 0xF0) | 0x09;

    i2c_cmd_handle_t handle = i2c_cmd_link_create();
    i2c_master_start(handle);
    i2c_master_write_byte(handle, (I2C_LCD_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(handle, data_arr, sizeof(data_arr), true);
    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_NUM, handle, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(handle);
}

void Lcd1602::lcd_init() 
{
    vTaskDelay(pdMS_TO_TICKS(50));
    lcd_send_command(0x30);
    vTaskDelay(pdMS_TO_TICKS(5));
    lcd_send_command(0x30);
    vTaskDelay(pdMS_TO_TICKS(1));
    lcd_send_command(0x30);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_command(0x20);
    vTaskDelay(pdMS_TO_TICKS(10));

    lcd_send_command(LCD_CMD_FUNCTION_SET | LCD_FLAG_2LINE | LCD_FLAG_5x8DOTS);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_command(LCD_CMD_DISPLAY_CONTROL | LCD_FLAG_DISPLAY_ON | LCD_FLAG_CURSOR_OFF | LCD_FLAG_BLINK_OFF);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
    vTaskDelay(pdMS_TO_TICKS(10));
    lcd_send_command(LCD_CMD_ENTRY_MODE_SET | 0x02);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void Lcd1602::lcd_clear() 
{
    lcd_send_command(LCD_CMD_CLEAR_DISPLAY);
    vTaskDelay(pdMS_TO_TICKS(10));
}

void Lcd1602::lcd_home() 
{
    lcd_send_command(LCD_CMD_RETURN_HOME);
}

void Lcd1602::lcd_set_cursor(uint8_t col, uint8_t row) 
{
    static const uint8_t row_offsets[] = {0x00, 0x40};

    if (row >= 2) 
    {
        row = 1;
    }

    lcd_send_command(LCD_CMD_SET_DDRAM_ADDR | (col + row_offsets[row]));
}

void Lcd1602::lcd_write_string(const char* str) 
{
    while (*str) 
    {
        lcd_send_data((uint8_t)(*str));
        str++;
    }
}
