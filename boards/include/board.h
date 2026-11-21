/**
 * @file board.h
 * @brief Board Support API - the ONLY hardware interface apps may use.
 *
 * Every board in boards/<name>/ implements these functions. Apps include
 * this header and nothing vendor-specific, so the same app builds for every
 * board (and its logic can be unit-tested on a PC).
 *
 * Layering:  apps -> libs/drivers -> board.h -> boards/<name> -> ST HAL -> hardware
 */
#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Bring the board up: HAL, system tick, clocks, LED pins and the debug
 * console (so printf() works). Call first in main().
 */
void board_init(void);

/** Human-readable board name, e.g. "STM32H7S78-DK". */
const char *board_name(void);

/** Number of user LEDs; valid indexes are 0 .. board_led_count() - 1. */
unsigned board_led_count(void);

/** Switch LED @p index on or off. Out-of-range indexes are ignored. */
void board_led_set(unsigned index, bool on);

/** Invert LED @p index. Out-of-range indexes are ignored. */
void board_led_toggle(unsigned index);

/** Milliseconds since board_init() (wraps after ~49 days). */
uint32_t board_millis(void);

/** Busy-wait for @p ms milliseconds. */
void board_delay_ms(uint32_t ms);

/**
 * Send @p len raw bytes to the debug console: the ST-LINK virtual COM port,
 * 115200 baud, 8N1. Blocks until sent. printf() reaches this through
 * _write() in boards/common/syscalls.c, which also turns "\n" into "\r\n".
 */
void board_console_write(const char *data, size_t len);

/** Unrecoverable error: show it on the LEDs and stop. Never returns. */
void board_panic(void) __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
