/**
 * @file    gpio.c
 * @brief   GPIO driver — thin HAL wrapper
 * @details Provides named functions so upper layers never call HAL directly.
 *          All pin and port constants are defined in board.h.
 */

#include "gpio.h"
#include "../board.h"

/**
 * @brief  Configure all GPIO pins defined in board.h.
 *         Called once at startup from main.c after MX_GPIO_Init().
 */
void GPIO_Init(void)
{
    /* TODO: Enable GPIO clocks and configure modes/speeds if not done by MX */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

/**
 * @brief  Write a logic level to a GPIO pin.
 * @param  port   GPIO port (e.g. GPIOA)
 * @param  pin    GPIO pin mask (e.g. GPIO_PIN_3)
 * @param  state  GPIO_PIN_SET or GPIO_PIN_RESET
 */
void GPIO_WritePin(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state)
{
    HAL_GPIO_WritePin(port, pin, state);
}

/**
 * @brief  Read the logic level of a GPIO pin.
 * @param  port  GPIO port
 * @param  pin   GPIO pin mask
 * @retval GPIO_PIN_SET or GPIO_PIN_RESET
 */
GPIO_PinState GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin)
{
    return HAL_GPIO_ReadPin(port, pin);
}

/**
 * @brief  Toggle a GPIO output pin.
 * @param  port  GPIO port
 * @param  pin   GPIO pin mask
 */
void GPIO_TogglePin(GPIO_TypeDef *port, uint16_t pin)
{
    HAL_GPIO_TogglePin(port, pin);
}
