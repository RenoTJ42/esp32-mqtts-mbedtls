/**
 * Bare-metal UART0 driver — register-level, no HAL
 *
 * Directly accesses UART0 registers at base address 0x3FF40000.
 * No esp_driver_uart component, no uart_driver_install().
 *
 * Supports: polled TX, polled RX, 8N1 frame format, 115200 baud
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stddef.h>

/* ── UART0 Base Address ─────────────────────────────────────────────────── */
#define UART0_BASE          0x3FF40000UL

/* ── Register Offsets (ESP32 TRM Table 12-2) ────────────────────────────── */
#define UART_FIFO_OFF       0x000   /* TX write / RX read FIFO              */
#define UART_STATUS_OFF     0x01C   /* FIFO fill levels                     */
#define UART_CLKDIV_OFF     0x014   /* Baud rate divisor                    */
#define UART_CONF0_OFF      0x020   /* Frame format config                  */

/* ── Register Access Macros ─────────────────────────────────────────────── */
#define UART_REG(offset)    (*((volatile uint32_t *)(UART0_BASE + (offset))))

#define UART_FIFO_REG       UART_REG(UART_FIFO_OFF)
#define UART_STATUS_REG     UART_REG(UART_STATUS_OFF)
#define UART_CLKDIV_REG     UART_REG(UART_CLKDIV_OFF)
#define UART_CONF0_REG      UART_REG(UART_CONF0_OFF)

/* ── Status Register Bit Fields ─────────────────────────────────────────── */
#define UART_RXFIFO_CNT_MASK    0x000000FFUL   /* bits [7:0]  */
#define UART_TXFIFO_CNT_MASK    0x000F0000UL   /* bits [19:16] */
#define UART_TXFIFO_CNT_SHIFT   16

/* ── CONF0 Register Values for 8N1 ─────────────────────────────────────── */
#define UART_CONF0_8N1      0x00000034UL  /* 8 data, 1 stop, no parity     */

/* ── Clock Math ─────────────────────────────────────────────────────────── */
#define UART_APB_CLK_HZ     80000000UL    /* APB bus = 80 MHz always        */
#define UART_BAUD_115200    115200UL
#define UART_CLKDIV_115200  ((7UL << 20) | (UART_APB_CLK_HZ / UART_BAUD_115200))

/* ── TX FIFO Depth ──────────────────────────────────────────────────────── */
#define UART_TX_FIFO_SIZE   128

/* ── Public API ─────────────────────────────────────────────────────────── */

/**
 * Initialise UART0 at 115200 8N1.
 *        GPIO1=TX, GPIO3=RX (DevKit default, set by ROM bootloader).
 *        We only configure the UART peripheral registers here — GPIO
 *        matrix configuration is handled by the ROM bootloader for UART0.
 */
void uart_driver_init(void);

/**
 * Transmit one byte. Blocks until TX FIFO has space.
 *  The byte to transmit
 */
void uart_write_byte(uint8_t byte);

/**
 * Transmit a null-terminated string.
 * Pointer to string
 */
void uart_write_str(const char *str);

/**
 * Transmit a buffer of known length.
 * buf   Pointer to data
 * len   Number of bytes
 */
void uart_write_buf(const uint8_t *buf, size_t len);

/**
 * Check if a byte is available in RX FIFO.
 * return 1 if byte available, 0 if FIFO empty
 */
int uart_rx_available(void);

/**
 * Read one byte from RX FIFO (blocking).
 * return Received byte
 */
uint8_t uart_read_byte(void);

#endif /* UART_DRIVER_H */
