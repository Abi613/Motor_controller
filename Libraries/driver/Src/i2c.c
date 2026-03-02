/**
 * @file    i2c.c
 * @brief   I2C driver — blocking (polling) HAL wrapper for I2C1
 * @details Timeout for all transactions is I2C_TIMEOUT_MS.
 *          For non-blocking operation, replace HAL_I2C_Master_Transmit with
 *          HAL_I2C_Master_Transmit_IT and implement the callback.
 */

#include "i2c.h"
#include "../board.h"

#define I2C_TIMEOUT_MS   10U    /**< Max wait per I2C transaction (ms) */

/**
 * @brief  Verify I2C peripheral is ready (bus not stuck, clock OK).
 */
void I2C_Init(void)
{
    /* TODO: Verify hi2c1 is initialized by MX_I2C1_Init() before calling   */
    (void)HAL_I2C_IsDeviceReady(&hi2c1, OLED_I2C_ADDR << 1, 3, I2C_TIMEOUT_MS);
}

/**
 * @brief  Write a byte buffer to an I2C device.
 * @param  dev_addr  7-bit device address (shifted internally)
 * @param  data      Pointer to data to send
 * @param  len       Number of bytes
 * @retval I2C_OK on success, I2C_ERROR on NAK/timeout
 */
I2C_Status_t I2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(
        &hi2c1, (uint16_t)(dev_addr << 1), (uint8_t *)data, len, I2C_TIMEOUT_MS);
    return (status == HAL_OK) ? I2C_OK : I2C_ERROR;
}

/**
 * @brief  Read bytes from an I2C device into a buffer.
 * @param  dev_addr  7-bit device address
 * @param  data      Buffer to fill
 * @param  len       Number of bytes to read
 * @retval I2C_OK or I2C_ERROR
 */
I2C_Status_t I2C_Read(uint8_t dev_addr, uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(
        &hi2c1, (uint16_t)(dev_addr << 1), data, len, I2C_TIMEOUT_MS);
    return (status == HAL_OK) ? I2C_OK : I2C_ERROR;
}

/**
 * @brief  Write a single register byte to an I2C device.
 * @param  dev_addr  7-bit device address
 * @param  reg       Register address
 * @param  value     Value to write
 * @retval I2C_OK or I2C_ERROR
 */
I2C_Status_t I2C_WriteReg(uint8_t dev_addr, uint8_t reg, uint8_t value)
{
    uint8_t buf[2] = {reg, value};
    return I2C_Write(dev_addr, buf, 2U);
}

/**
 * @brief  Check if a device acknowledges on the bus.
 * @param  dev_addr  7-bit device address
 * @retval 1 if device is ready, 0 otherwise
 */
uint8_t I2C_IsDeviceReady(uint8_t dev_addr)
{
    return (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(dev_addr << 1), 2, I2C_TIMEOUT_MS) == HAL_OK) ? 1U : 0U;
}
