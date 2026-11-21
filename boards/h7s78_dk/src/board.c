/**
 * @file board.c
 * @brief board.h implementation for the STM32H7S78-DK.
 *
 * LED wiring (ST BSP: Drivers/BSP/STM32H7S78-DK/stm32h7s78_discovery.[ch]):
 *   0  LD1 green   PO1   on = HIGH   port O: XSPIM_P1 I/O supply domain
 *   1  LD2 orange  PO5   on = HIGH
 *   2  LD3 red     PM2   on = LOW    port M: USB33 I/O supply domain
 *   3  LD4 blue    PM3   on = LOW
 *
 * Gotcha: ports M/N/O/P live in separate supply domains. Until firmware
 * enables that supply, writes to those pins have no visible effect.
 */
#include "board.h"
#include "stm32h7rsxx_hal.h"

#define USB33_READY_TIMEOUT_MS 100U

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_PinState on_level;
} led_t;

static const led_t s_leds[] = {
    {GPIOO, GPIO_PIN_1, GPIO_PIN_SET},   /* LD1 green  */
    {GPIOO, GPIO_PIN_5, GPIO_PIN_SET},   /* LD2 orange */
    {GPIOM, GPIO_PIN_2, GPIO_PIN_RESET}, /* LD3 red    */
    {GPIOM, GPIO_PIN_3, GPIO_PIN_RESET}, /* LD4 blue   */
};
#define LED_COUNT (sizeof(s_leds) / sizeof(s_leds[0]))

static void enable_io_supplies(void)
{
    HAL_PWREx_EnableXSPIM1(); /* port O */

    if (HAL_PWREx_EnableUSBVoltageDetector() != HAL_OK) /* port M */
    {
        board_panic();
    }
    uint32_t start = HAL_GetTick();
    while ((PWR->CSR2 & PWR_CSR2_USB33RDY) == 0U)
    {
        if ((HAL_GetTick() - start) > USB33_READY_TIMEOUT_MS)
        {
            board_panic();
        }
    }
}

static void init_leds(void)
{
    __HAL_RCC_GPIOO_CLK_ENABLE();
    __HAL_RCC_GPIOM_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    for (unsigned i = 0U; i < LED_COUNT; i++)
    {
        board_led_set(i, false); /* set the level before enabling the output */
        gpio.Pin = s_leds[i].pin;
        HAL_GPIO_Init(s_leds[i].port, &gpio);
    }
}

void board_init(void)
{
    /* SystemInit() already ran from Reset_Handler. HAL_Init() starts the 1 ms
     * SysTick. The core stays on the 64 MHz HSI (no PLL yet). */
    if (HAL_Init() != HAL_OK)
    {
        board_panic();
    }
    enable_io_supplies();
    init_leds();
}

const char *board_name(void)
{
    return "STM32H7S78-DK";
}

unsigned board_led_count(void)
{
    return (unsigned)LED_COUNT;
}

void board_led_set(unsigned index, bool on)
{
    if (index >= LED_COUNT)
    {
        return;
    }
    const led_t *led = &s_leds[index];
    GPIO_PinState off_level = (led->on_level == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(led->port, led->pin, on ? led->on_level : off_level);
}

void board_led_toggle(unsigned index)
{
    if (index < LED_COUNT)
    {
        HAL_GPIO_TogglePin(s_leds[index].port, s_leds[index].pin);
    }
}

uint32_t board_millis(void)
{
    return HAL_GetTick();
}

void board_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void board_panic(void)
{
    __disable_irq();
    for (unsigned i = 0U; i < LED_COUNT; i++)
    {
        board_led_set(i, true); /* all LEDs on = panic */
    }
    for (;;)
    {
    }
}
