/**
 * @file stm32h7rsxx_it.c
 * @brief Interrupt handlers. Names must match the vector table in
 *        startup_stm32h7s7xx.s; any handler not defined here falls back to
 *        Default_Handler (an infinite loop) through a weak alias.
 */
#include "stm32h7rsxx_it.h"
#include "stm32h7rsxx_hal.h"

/* 1 ms system tick: drives HAL_Delay() / HAL_GetTick(). */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/* Fault: stop here so a debugger shows where it happened. */
void HardFault_Handler(void)
{
    for (;;)
    {
    }
}
