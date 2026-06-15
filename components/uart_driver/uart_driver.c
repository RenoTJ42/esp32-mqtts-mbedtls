/**
 * @file    uart_driver.c
 * @brief   Bare-metal UART0 driver — direct FIFO access
 *
 * DESIGN DECISION:
 * The ESP-IDF system owns UART0 (console). It initialises the peripheral
 * at 115200 8N1 before app_main() and installs an interrupt-driven driver.
 * Any attempt to reset, reclock, or flush the peripheral from app code
 * corrupts the IDF driver state and triggers a crash.
 *
 * Our driver therefore does NOT touch UART_CONF0_REG or UART_CLKDIV_REG.
 * Instead we write DIRECTLY to UART_FIFO_REG — the hardware TX FIFO
 * accepts bytes regardless of which software layer "owns" the peripheral.
 * We poll UART_STATUS_REG to check TXFIFO_CNT before each write.
 *
 * This is safe because:
 *   - FIFO writes are atomic at the hardware level
 *   - We only ADD bytes to the FIFO; we never modify control registers
 *   - The IDF UART driver does not lock the FIFO against external writes
 *
 * For a standalone bare-metal system (no IDF), the full init sequence
 * with DPORT clock gate + reset + clkdiv + conf0 + FIFO flush would apply.
 */

#include "uart_driver.h"
#include <stddef.h>

/* ────────────────────────────────────────────────────────────────────────── */

void uart_driver_init(void)
{
    /*
     * Intentional no-op when running under ESP-IDF.
     *
     * The IDF system startup sequence (cpu_start.c → esp_console_init)
     * has already:
     *   1. Opened the DPORT APB clock gate for UART0
     *   2. Configured UART_CLKDIV_REG for 115200 baud (APB=80MHz)
     *   3. Set UART_CONF0_REG for 8N1 frame format
     *   4. Configured GPIO matrix: GPIO1=TX, GPIO3=RX
     *   5. Installed interrupt-driven UART driver
     *
     * Touching any of these registers now would corrupt the IDF driver
     * and trigger RTCWDT_RTC_RESET boot loop (confirmed in debugging).
     *
     * Our uart_write_byte() writes directly to UART_FIFO_REG which is
     * safe — FIFO writes do not interfere with the IDF driver's state.
     */
}

/* ────────────────────────────────────────────────────────────────────────── */

void uart_write_byte(uint8_t byte)
{
    /* Poll TXFIFO_CNT (STATUS_REG bits[19:16]) until space available.
     * Then push byte directly into hardware TX FIFO. */
    uint32_t tx_count;
    do {
        tx_count = (UART_STATUS_REG & UART_TXFIFO_CNT_MASK)
                    >> UART_TXFIFO_CNT_SHIFT;
    } while (tx_count >= UART_TX_FIFO_SIZE);

    UART_FIFO_REG = (uint32_t)byte;
}

/* ────────────────────────────────────────────────────────────────────────── */

void uart_write_str(const char *str)
{
    if (str == NULL) return;
    while (*str != '\0') {
        uart_write_byte((uint8_t)*str);
        str++;
    }
}

/* ────────────────────────────────────────────────────────────────────────── */

void uart_write_buf(const uint8_t *buf, size_t len)
{
    if (buf == NULL) return;
    for (size_t i = 0; i < len; i++) {
        uart_write_byte(buf[i]);
    }
}

/* ────────────────────────────────────────────────────────────────────────── */

int uart_rx_available(void)
{
    return ((UART_STATUS_REG & UART_RXFIFO_CNT_MASK) > 0) ? 1 : 0;
}

/* ────────────────────────────────────────────────────────────────────────── */

uint8_t uart_read_byte(void)
{
    while (!uart_rx_available()) { /* spin */ }
    return (uint8_t)(UART_FIFO_REG & 0xFFUL);
}
