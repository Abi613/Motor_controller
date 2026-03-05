/**
 * @file    adc.h
 * @brief   ADC driver public interface
 */

#ifndef DRIVERS_ADC_H
#define DRIVERS_ADC_H

#include <stdint.h>

typedef enum
{
    ADC_CH_CURRENT = 0,
    ADC_CH_VBUS,
    ADC_CH_BEMF_U,
    ADC_CH_BEMF_V,
    ADC_CH_BEMF_W,
    ADC_CH_COUNT
} ADC_Channel_t;

void     ADC_Init(void);
void     ADC_StartConversion(void);
uint16_t ADC_GetRaw(ADC_Channel_t channel);
float    ADC_GetCurrent_A(void);
float    ADC_GetVbus_V(void);
float    ADC_GetBemf_V(uint8_t phase);
void     ADC_ConversionComplete_Callback(void);
void     ADC_SelectBEMFChannel(uint8_t phase, uint8_t rising);
uint8_t  ADC_BEMFZeroCrossDetected(void);

#endif /* DRIVERS_ADC_H */
