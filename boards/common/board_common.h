/**
 * @file board_common.h
 * @brief Helpers shared by all board implementations. Not for apps.
 */
#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

/**
 * Give stdout a small static line buffer, so printf() never calls malloc().
 * Call once from board_init(), after the console UART is ready.
 */
void board_common_stdio_init(void);

#endif /* BOARD_COMMON_H */
