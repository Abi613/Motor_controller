/**
 * @file    gpio.h
 * @brief   GPIO driver public interface
 */

#ifndef DRIVERS_GPIO_H
#define DRIVERS_GPIO_H

#include "stm32f1xx_hal.h"

void GPIO_Init(void);
void GPIO_WritePin(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);
GPIO_PinState GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin);
void GPIO_TogglePin(GPIO_TypeDef *port, uint16_t pin);

#endif /* DRIVERS_GPIO_H */
