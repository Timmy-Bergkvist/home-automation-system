#include "Led.h"
#include "esp_log.h"
#include "../../components/config.h"

static const char* TAG = "Led";

Led::Led() {}

Led::~Led() {}

void Led::init()
{
    // Configure the PWM channels for the RGB LED
    const int RGB_LED_FREQ = 1000;  // Frequency of PWM
    const ledc_timer_bit_t RGB_LED_RESOLUTION = LEDC_TIMER_8_BIT;
    
    ledc_channel_config_t ledc_channel;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.timer_sel = LEDC_TIMER_0;
    ledc_channel.duty = 0;
    ledc_channel.hpoint = 0;
    ledc_channel.intr_type = LEDC_INTR_DISABLE;

    // Initialize channel for red LED
    ledc_channel.gpio_num = RGB_LED_RED_PIN;
    ledc_channel.channel = LEDC_CHANNEL_0;
    ledc_channel_config(&ledc_channel);

    // Initialize channel for green LED
    ledc_channel.gpio_num = RGB_LED_GREEN_PIN;
    ledc_channel.channel = LEDC_CHANNEL_1;
    ledc_channel_config(&ledc_channel);

    // Initialize channel for blue LED
    ledc_channel.gpio_num = RGB_LED_BLUE_PIN;
    ledc_channel.channel = LEDC_CHANNEL_2;
    ledc_channel_config(&ledc_channel);

    // Configure the timer for PWM
    ledc_timer_config_t ledc_timer;
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.duty_resolution = RGB_LED_RESOLUTION;
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.freq_hz = RGB_LED_FREQ;
    ledc_timer.clk_cfg = LEDC_AUTO_CLK;

    ledc_timer_config(&ledc_timer);
}

// Common anode RGB LED
void Led::setRgbColor(uint8_t red, uint8_t green, uint8_t blue)
{
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 255 - red);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 255 - green);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 255 - blue);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);
}
