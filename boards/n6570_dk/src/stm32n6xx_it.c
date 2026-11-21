/**
 * @file stm32n6xx_it.c
 * @brief Interrupt handlers. Names must match the vector table in
 *        startup_stm32n657xx_fsbl.s; anything not defined here goes to
 *        Default_Handler (an infinite loop) through a weak alias.
 */
#include "stm32n6xx_it.h"
#include "stm32n6xx_hal.h"

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void HardFault_Handler(void)
{
    for (;;)
    {
    }
}
