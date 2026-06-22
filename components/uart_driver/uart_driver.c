/**
 * UART0 diagnostic output driver
 *
 * DESIGN DECISION:
 * UART0 is owned by the ESP-IDF console subsystem. Direct register access
 * to UART_FIFO_REG races with the IDF UART interrupt driver and causes
 * optimizer-induced bugs at -O2 (polling loop elimination).
 *
 * We therefore wrap esp_rom_printf() which:
 *   1. Bypasses the IDF driver and writes directly to ROM UART routines
 *   2. Is interrupt-safe (used by panic handler and bootloader)
 *   3. Works before and after scheduler start
 *   4. Requires no peripheral ownership — ROM handles it
 *
 * Our public API (uart_write_str, uart_write_byte etc.) is preserved
 * so all call sites in main.c remain unchanged.
 *
 * Bare-metal register access IS used in Phase 3 (GPIO) and Phase 4 (I2C)
 * where we have clean peripheral ownership with no IDF driver conflict.
 */

#include "uart_driver.h"
#include "esp_rom_sys.h"
#include <stddef.h>

/* ────────────────────────────────────────────────────────────────────────── */

void uart_driver_init(void)
{
    /*
     * No initialisation needed.
     * esp_rom_printf uses ROM UART routines pre-configured by bootloader.
     * UART0 baud rate (115200), frame format (8N1), and GPIO matrix
     * (GPIO1=TX, GPIO3=RX) are all set before app_main() is called.
     */
}

/* ────────────────────────────────────────────────────────────────────────── */

void uart_write_byte(uint8_t byte)
{
    esp_rom_printf("%c", (char)byte);
}

/* ────────────────────────────────────────────────────────────────────────── */

void uart_write_str(const char *str)
{
    if (str == NULL) return;
    esp_rom_printf("%s", str);
}

/* ────────────────────────────────────────────────────────────────────────── */

void uart_write_buf(const uint8_t *buf, size_t len)
{
    if (buf == NULL || len == 0) return;
    for (size_t i = 0; i < len; i++) {
        esp_rom_printf("%c", (char)buf[i]);
    }
}

/* ────────────────────────────────────────────────────────────────────────── */

int uart_rx_available(void)
{
    /*
     * RX not needed for our diagnostic use case.
     * UART RX is handled by the IDF console task in Phase 5+.
     * Return 0 — no bytes available from our driver perspective.
     */
    return 0;
}

/* ────────────────────────────────────────────────────────────────────────── */

uint8_t uart_read_byte(void)
{
    /*
     * RX reads go through IDF driver in Phase 5+.
     * Stub returns 0 for now.
     */
    return 0;
}
