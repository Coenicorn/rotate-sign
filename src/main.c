#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "stdio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "stdint.h"

#define LED_PIN 10

#define SERVO_PIN 9

#define SERVO_MIN 3277.f
#define SERVO_MAX 6553.f

#define I2C_PORT i2c0
#define DS3231_ADDR 0x68

// Initialize the I2C
void init_i2c() {
    i2c_init(I2C_PORT, 400 * 1000);  // 400 kHz
    gpio_set_function(4, GPIO_FUNC_I2C);  // SDA
    gpio_set_function(5, GPIO_FUNC_I2C);  // SCL
    gpio_pull_up(4);
    gpio_pull_up(5);
}

// Convert BCD to decimal
int bcd_to_decimal(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

// Read time from DS3231
void read_time() {
    uint8_t time_data[7];
    i2c_write_blocking(I2C_PORT, DS3231_ADDR, (uint8_t[]){0x00}, 1, true);
    i2c_read_blocking(I2C_PORT, DS3231_ADDR, time_data, 7, false);

    int seconds = bcd_to_decimal(time_data[0]);
    int minutes = bcd_to_decimal(time_data[1]);
    int hours = bcd_to_decimal(time_data[2]);
    int day = bcd_to_decimal(time_data[4]);
    int month = bcd_to_decimal(time_data[5]);
    int year = bcd_to_decimal(time_data[6]) + 2000;

    printf("Time: %02d:%02d:%02d Date: %02d-%02d-%04d\n", hours, minutes, seconds, day, month, year);
}

static uint16_t duty_cycle = 0;

static uint slice_num = 0;

// Initialize the GPIO for the LED
void led_init(void)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

// Turn the LED on or off
void set_led(bool led_on)
{
    gpio_put(LED_PIN, led_on);
}

void init_pwm_full(void)
{
    // initialize pin for pwm
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);

    slice_num = pwm_gpio_to_slice_num(SERVO_PIN);

    // Set the PWM frequency to 50 Hz (for 20 ms period)
    uint32_t frequency = 50;  // 50 Hz = 20 ms period
    uint32_t clock = 125000000;  // RP2040 system clock runs at 125 MHz by default

    // Calculate the divider for the system clock to achieve the desired PWM frequency
    uint32_t divider = clock / (frequency * UINT_LEAST16_MAX);  // Use the maximum resolution (16-bit)
    pwm_set_clkdiv(slice_num, divider);  // Set the clock divider for the PWM slice

    // Set the wrap value (top) for 16-bit resolution
    pwm_set_wrap(slice_num, UINT_LEAST16_MAX);  // 16-bit resolution

    // Calculate the level for a 1.5 ms duty cycle (7.5% of 20 ms period)
    // Duty cycle in terms of the 16-bit range is (1.5 ms / 20 ms) * 65535
    duty_cycle = (1.0 / 20.0) * UINT_LEAST16_MAX;  // 7.5% duty cycle
    pwm_set_chan_level(slice_num, PWM_CHAN_B, duty_cycle);  // Set duty cycle for channel B (which is bound to pin 9)

    // Enable the PWM output
    pwm_set_enabled(slice_num, true);
}

// void servo_turn(float num)
// {
//     uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
//     uint16_t duty_cycle = (num / 20.f) * top_value;
//     pwm_set_chan_level(slice_num, PWM_CHAN_B, duty_cycle);
// }

void servo_write_microseconds(uint microseconds)
{
    duty_cycle = (microseconds / 20000.0) * UINT_LEAST16_MAX;
    pwm_set_chan_level(slice_num, PWM_CHAN_B, duty_cycle);
}

int main() {
    stdio_init_all();

    init_pwm_full();

    led_init();

    init_i2c();

    while (true)
    {
        servo_write_microseconds(544);

        sleep_ms(1000);

        servo_write_microseconds(2400);

        sleep_ms(1000);
    }
}