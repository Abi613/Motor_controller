/**
 * @file    i2c.h
 * @brief   I2C driver public interface
 */

#ifndef DRIVERS_I2C_H
#define DRIVERS_I2C_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

typedef enum
{
    I2C_OK    = 0,
    I2C_ERROR = 1,
    I2C_BUSY  = 2
} I2C_Status_t;

void         I2C_Init(void);
I2C_Status_t I2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len);
I2C_Status_t I2C_Read(uint8_t dev_addr, uint8_t *data, uint16_t len);
I2C_Status_t I2C_WriteReg(uint8_t dev_addr, uint8_t reg, uint8_t value);
uint8_t      I2C_IsDeviceReady(uint8_t dev_addr);

#endif /* DRIVERS_I2C_H */
