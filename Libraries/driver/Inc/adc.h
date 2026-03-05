/**
 * @file    adc.h
 * @brief   ADC driver public interface
 */

#ifndef DRIVERS_ADC_H
#define DRIVERS_ADC_H

#include <stdint.h>

/** @brief Enumeration of ADC channels used in this project */
typedef enum
{
    ADC_CH_CURRENT = 0,   /**< Motor phase current sense */
    ADC_CH_VBUS    = 1,   /**< Bus voltage sense          */
    ADC_CH_COUNT          /**< Total number of channels   */
} ADC_Channel_t;

void     ADC_Init(void);
void     ADC_StartConversion(void);
uint16_t ADC_GetRaw(ADC_Channel_t channel);
float    ADC_GetCurrent_A(void);
float    ADC_GetVbus_V(void);
void     ADC_ConversionComplete_Callback(void);  /**< Called from ISR         */
void     ADC_SelectBEMFChannel(uint8_t phase, uint8_t rising);

#endif /* DRIVERS_ADC_H */
