/**
 * @file board_console.c
 * @brief Debug console for the STM32N6570-DK: USART1 -> ST-LINK virtual COM port.
 *
 * Wiring (ST BSP stm32n6570_discovery.h, COM1_*):
 *   USART1 TX = PE5 (AF7)    USART1 RX = PE6 (AF7)
 * Kernel clock: reset default; HAL reads it back and computes the divider.
 */
#include "board.h"
#include "board_console.h"
#include "stm32n6xx_hal.h"

#define CONSOLE_BAUD       115200U
#define CONSOLE_TIMEOUT_MS 100U

static UART_HandleTypeDef s_uart;

/* Called by HAL_UART_Init(): clocks and pins ("MSP" = MCU support package). */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance != USART1)
    {
        return;
    }
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6; /* TX | RX */
    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOE, &gpio);
}

void board_console_init(void)
{
    s_uart.Instance = USART1;
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
    while (len > 0U)
    {
        uint16_t chunk = (len > UINT16_MAX) ? UINT16_MAX : (uint16_t)len;
        (void)HAL_UART_Transmit(&s_uart, (const uint8_t *)data, chunk, CONSOLE_TIMEOUT_MS);
        data += chunk;
        len -= chunk;
    }
}
