/**
 * @file main.c
 * @brief Hello: printf() over the ST-LINK virtual COM port.
 *
 * Open a serial terminal at 115200 8N1 on the board's "STLink Virtual COM
 * Port", then reset the board. Every second it prints a tick line and toggles
 * LED 0.
 */
#include <inttypes.h>
#include <stdio.h>

#include "board.h"

#define TICK_PERIOD_MS 1000U

int main(void)
{
    board_init();

    printf("\n*** embedded-lab hello ***\n");
    printf("board   : %s\n", board_name());
    printf("version : %s (git %s)\n", LAB_VERSION, LAB_GIT_VERSION);
    printf("built   : %s %s\n", __DATE__, __TIME__);

    /* Fixed-rate loop: wait for an absolute deadline instead of "delay 1000 ms".
     * A plain delay adds the printf() time every round, so ticks drift
     * (1000, 2003, 3006 ...). The signed difference keeps the comparison
     * correct even when the 32-bit millisecond counter wraps after ~49 days. */
    uint32_t tick = 0U;
    uint32_t next = board_millis();
    for (;;)
    {
        printf("tick %" PRIu32 "  uptime %" PRIu32 " ms\n", tick, board_millis());
        board_led_toggle(0U);
        tick++;

        next += TICK_PERIOD_MS;
        while ((int32_t)(board_millis() - next) < 0)
        {
            /* busy-wait; later: sleep with __WFI() until the next SysTick */
        }
    }
}
