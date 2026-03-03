/**
 * @file    adc.c
 * @brief   ADC driver — DMA or interrupt-triggered multi-channel conversion
 * @details Two channels: current sense (PA4) and bus voltage (PA5).
 *          Results are stored in s_adc_raw[] and updated via DMA or ISR.
 *          Conversion to physical units uses constants from board.h.
 */

#include "adc.h"
#include "board.h"

/*===========================================================================*/
/* PRIVATE VARIABLES                                                          */
/*===========================================================================*/
/** @brief Raw ADC results buffer. Index maps to ADC_Channel_t. */
static volatile uint16_t s_adc_raw[ADC_CH_COUNT] = {0U, 0U};

/** @brief Flag set true by ISR when new data is available */
static volatile uint8_t  s_data_ready = 0U;

/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Initialize ADC and start continuous/scan conversion.
 */
void ADC_Init(void)
{

	  /* USER CODE BEGIN ADC1_Init 0 */

	  /* USER CODE END ADC1_Init 0 */

	  ADC_ChannelConfTypeDef sConfig = {0};

	  /* USER CODE BEGIN ADC1_Init 1 */

	  /* USER CODE END ADC1_Init 1 */

	  /** Common config
	  */
	  hadc1.Instance = ADC1;
	  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	  hadc1.Init.ContinuousConvMode = DISABLE;
	  hadc1.Init.DiscontinuousConvMode = DISABLE;
	  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  hadc1.Init.NbrOfConversion = 1;
	  if (HAL_ADC_Init(&hadc1) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  /** Configure Regular Channel
	  */
	  sConfig.Channel = ADC_CHANNEL_0;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /* USER CODE BEGIN ADC1_Init 2 */

	  /* USER CODE END ADC1_Init 2 */

	}

	/**
	  * @brief I2C1 Initialization Function
	  * @param None
	  * @retval None
	  */   /* or HAL_ADC_Start_DMA for DMA mode

/**
 * @brief  Trigger a single software-start ADC conversion (polling mode).
 *         Not needed if DMA/interrupt mode is used.
 */
void ADC_StartConversion(void)
{
    HAL_ADC_Start_IT(&hadc1);
}

/**
 * @brief  Get last raw ADC count for a given channel.
 * @param  channel  ADC_CH_CURRENT or ADC_CH_VBUS
 * @retval Raw 12-bit count [0–4095]
 */
uint16_t ADC_GetRaw(ADC_Channel_t channel)
{
    if (channel >= ADC_CH_COUNT)
    {
        return 0U;
    }
    return s_adc_raw[channel];
}

/**
 * @brief  Convert raw current sense reading to amperes.
 * @retval Motor current in Amperes
 */
float ADC_GetCurrent_A(void)
{
    float voltage_mv = ((float)s_adc_raw[ADC_CH_CURRENT] / ADC_RESOLUTION) * ADC_VREF_MV;
    /* I = V_sense / (Gain * Shunt_R) */
    return (voltage_mv / 1000.0f) / (CURRENT_SENSE_GAIN * (CURRENT_SENSE_SHUNT_mR / 1000.0f));
}

/**
 * @brief  Convert raw bus voltage reading to volts.
 * @retval Bus voltage in Volts
 */
float ADC_GetVbus_V(void)
{
    /* TODO: Apply voltage divider ratio from hardware schematic */
    float voltage_mv = ((float)s_adc_raw[ADC_CH_VBUS] / ADC_RESOLUTION) * ADC_VREF_MV;
    return voltage_mv / 1000.0f;
}

/**
 * @brief  ISR callback — called from HAL_ADC_ConvCpltCallback() in main.c.
 *         Copy DMA buffer or read HAL result register into s_adc_raw[].
 */
void ADC_ConversionComplete_Callback(void)
{
    /* TODO: If using DMA, buffer is already filled. If interrupt, read here: */
    s_adc_raw[ADC_CH_CURRENT] = HAL_ADC_GetValue(&hadc1);
    s_data_ready = 1U;
}
