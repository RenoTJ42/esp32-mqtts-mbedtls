#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>

/* ── GPIO Base Address ──────────────────────────────────────────── */
#define GPIO_BASE   0x3FF44000UL

/* ── Register Offsets ───────────────────────────────────────────── */
#define GPIO_OUT_W1TS_OFF      0x008    /* atomic set HIGH */
#define GPIO_OUT_W1TC_OFF      0x00C    /* atomic set LOW  */
#define GPIO_ENABLE_W1TS_OFF   0x024    /* atomic enable output */
#define GPIO_IN_OFF            0x03C    /* read pin states */

/* ── Register Access Macro ─────────────────────────────────────── */
#define GPIO_REG(offset)  (*((volatile uint32_t *)(GPIO_BASE + (offset))))

#define GPIO_OUT_W1TS_REG     GPIO_REG(GPIO_OUT_W1TS_OFF)
#define GPIO_OUT_W1TC_REG     GPIO_REG(GPIO_OUT_W1TC_OFF)
#define GPIO_ENABLE_W1TS_REG  GPIO_REG(GPIO_ENABLE_W1TS_OFF)
#define GPIO_IN_REG           GPIO_REG(GPIO_IN_OFF)

/* ── GPIO2 bit mask ─────────────────────────────────────────────── */
#define GPIO2_BIT   (1UL << 2UL)

/* ── IO_MUX for pin function select ────────────────────────────── */
#define IO_MUX_BASE        0x3FF49000UL
#define IO_MUX_GPIO2_OFF   0x088
#define IO_MUX_GPIO2_REG   (*((volatile uint32_t *)(IO_MUX_BASE + IO_MUX_GPIO2_OFF)))

/* ── Public API ──────────────────────────────────────────────────── */
void gpio_driver_init(void);
void gpio_led_on(void);
void gpio_led_off(void);
void gpio_led_toggle(void);
void gpio_led_blink(uint32_t ms);

#endif
