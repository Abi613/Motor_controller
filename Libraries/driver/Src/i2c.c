/**
 * @file    i2c.c
 * @brief   I2C driver — blocking (polling) HAL wrapper for I2C1
 * @details Timeout for all transactions is I2C_TIMEOUT_MS.
 *          For non-blocking operation, replace HAL_I2C_Master_Transmit with
 *          HAL_I2C_Master_Transmit_IT and implement the callback.
 */

#include "i2c.h"
#include "board.h"
#include "main.h"

#define I2C_TIMEOUT_MS   10U    /**< Max wait per I2C transaction (ms) */

/**
 * @brief  Verify I2C peripheral is ready (bus not stuck, clock OK).
 */
void I2C_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
    if ((data == NULL) || (len == 0U)) { return I2C_ERROR; }
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
    if ((data == NULL) || (len == 0U)) { return I2C_ERROR; }
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
