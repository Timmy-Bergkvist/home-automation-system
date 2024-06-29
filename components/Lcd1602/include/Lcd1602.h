#ifndef LCD1602_H
#define LCD1602_H

#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_err.h"

class Lcd1602
{
public:
    Lcd1602();
    ~Lcd1602();

    void lcd_init();
    void lcd_clear();
    void lcd_home();
    void lcd_set_cursor(uint8_t col, uint8_t row);
    void lcd_write_string(const char* str);
    void lcd_write_int(int value);

private:
    esp_err_t i2c_lcd_write_byte(uint8_t data);
    void lcd_send_command(uint8_t cmd);
    void lcd_send_data(uint8_t data);
};

#endif