/**
 * @file main.c
 * @brief Blink: light each user LED in turn.
 *
 * Uses only board.h, so the same file runs on every board:
 * a 4-LED chase on the H7S78-DK, green/red alternating on the N6570-DK.
 */
#include "board.h"

#define STEP_MS 200U

int main(void)
{
    board_init();

    const unsigned count = board_led_count();
    unsigned current = 0U;

    for (;;)
    {
        board_led_set(current, true);
        board_delay_ms(STEP_MS);
        board_led_set(current, false);
        current = (current + 1U) % count;
    }
}
