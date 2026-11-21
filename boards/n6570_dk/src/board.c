/**
 * @file board.c
 * @brief board.h implementation for the STM32N6570-DK.
 *
 * LED wiring (ST BSP: Drivers/BSP/STM32N6570-DK/stm32n6570_discovery.[ch]):
 *   0  LED1 green  PO1   on = HIGH   port O: VDDIO2 supply domain
 *   1  LED2 red    PG10  on = LOW
 *
 * Gotcha: port O only works after HAL_PWREx_EnableVddIO2().
 */
#include "board.h"
#include "board_common.h"
#include "board_console.h"
#include "stm32n6xx_hal.h"

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_PinState on_level;
} led_t;

static const led_t s_leds[] = {
    {GPIOO, GPIO_PIN_1, GPIO_PIN_SET},    /* LED1 green */
    {GPIOG, GPIO_PIN_10, GPIO_PIN_RESET}, /* LED2 red   */
};
#define LED_COUNT (sizeof(s_leds) / sizeof(s_leds[0]))

static void init_leds(void)
{
    HAL_PWREx_EnableVddIO2(); /* port O supply */
    __HAL_RCC_GPIOO_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;

    for (unsigned i = 0U; i < LED_COUNT; i++)
    {
        board_led_set(i, false);
        gpio.Pin = s_leds[i].pin;
        HAL_GPIO_Init(s_leds[i].port, &gpio);
    }
}

void board_init(void)
{
    /* Runs from internal RAM: the boot ROM copied it there from external flash
     * (BOOT1 = L) or a debugger loaded it (BOOT1 = H, dev boot). */
    if (HAL_Init() != HAL_OK)
    {
        board_panic();
    }
    init_leds();
    board_console_init();
    board_common_stdio_init();
}

const char *board_name(void)
{
    return "STM32N6570-DK";
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
    board_led_set(0U, false);
    board_led_set(1U, true); /* red on = panic */
    for (;;)
    {
    }
}
