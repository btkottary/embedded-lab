/**
 * @file board_console.c
 * @brief Debug console for the STM32H7S78-DK: UART4 -> ST-LINK virtual COM port.
 *
 * Wiring (ST BSP stm32h7s78_discovery.h, COM1_*):
 *   UART4 TX = PD1 (AF8)    UART4 RX = PD0 (AF8)
 * Kernel clock: PCLK1 (reset default) = 64 MHz HSI; HAL computes the divider.
 */
#include "board.h"
#include "board_console.h"
#include "stm32h7rsxx_hal.h"

#define CONSOLE_BAUD       115200U
#define CONSOLE_TIMEOUT_MS 100U

static UART_HandleTypeDef s_uart;

/* Called by HAL_UART_Init(): clocks and pins ("MSP" = MCU support package). */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance != UART4)
    {
        return;
    }
    __HAL_RCC_UART4_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_1 | GPIO_PIN_0; /* TX | RX */
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_PULLUP; /* idle-high line, also when unplugged */
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOD, &gpio);
}

void board_console_init(void)
{
    s_uart.Instance = UART4;
    s_uart.Init.BaudRate = CONSOLE_BAUD;
    s_uart.Init.WordLength = UART_WORDLENGTH_8B;
    s_uart.Init.StopBits = UART_STOPBITS_1;
    s_uart.Init.Parity = UART_PARITY_NONE;
    s_uart.Init.Mode = UART_MODE_TX_RX;
    s_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    s_uart.Init.OverSampling = UART_OVERSAMPLING_16;
    s_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    s_uart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    s_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&s_uart) != HAL_OK)
    {
        board_panic();
    }
}

void board_console_write(const char *data, size_t len)
{
    /* HAL takes a uint16_t length: send long buffers in chunks. */
    while (len > 0U)
    {
        uint16_t chunk = (len > UINT16_MAX) ? UINT16_MAX : (uint16_t)len;
        (void)HAL_UART_Transmit(&s_uart, (const uint8_t *)data, chunk, CONSOLE_TIMEOUT_MS);
        data += chunk;
        len -= chunk;
    }
}
