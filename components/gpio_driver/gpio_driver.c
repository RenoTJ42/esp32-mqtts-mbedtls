/**
 * @file    gpio_driver.c
 * @brief   Bare-metal GPIO2 driver — status LED, register-level
 *
 * GPIO2 = onboard D2 LED on DOIT ESP32 DevKit V1.
 * No IDF driver conflict — we have full ownership of this pin.
 */

#include "gpio_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* IO_MUX function field is bits[12:10]. Function code 2 = plain GPIO. */
#define IO_MUX_FUNC_GPIO   (2UL << 10)

/* Track current LED state in software since GPIO_OUT_REG itself
 * isn't safely readable as "our last written value" without a
 * read of GPIO_OUT_REG (not W1TS/W1TC, those are write-only). */
static volatile uint8_t led_state = 0;

void gpio_driver_init(void)
{
    /* Step 1: Configure IO_MUX for GPIO2 to select plain GPIO function.
     * This routes the pin away from any alternate peripheral function
     * (e.g. ADC, touch sensor) and into simple digital I/O mode. */
    IO_MUX_GPIO2_REG = IO_MUX_FUNC_GPIO;

    /* Step 2: Enable GPIO2 as an output.
     * Atomic — only affects bit 2, all other pins' enable state
     * untouched, regardless of which core or task calls this. */
    GPIO_ENABLE_W1TS_REG = GPIO2_BIT;

    /* Step 3: Start with LED off — deterministic known state. */
    GPIO_OUT_W1TC_REG = GPIO2_BIT;
    led_state = 0;
}

void gpio_led_on(void)
{
    /* Atomic set — single bus write, hardware sets bit2 in GPIO_OUT_REG */
    GPIO_OUT_W1TS_REG = GPIO2_BIT;
    led_state = 1;
}

void gpio_led_off(void)
{
    /* Atomic clear — single bus write, hardware clears bit2 */
    GPIO_OUT_W1TC_REG = GPIO2_BIT;
    led_state = 0;
}

void gpio_led_toggle(void)
{
    if (led_state) {
        gpio_led_off();
    } else {
        gpio_led_on();
    }
}

void gpio_led_blink(uint32_t ms)
{
    gpio_led_on();
    vTaskDelay(pdMS_TO_TICKS(ms));
    gpio_led_off();
}
