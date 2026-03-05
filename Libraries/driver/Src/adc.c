#include "adc.h"
#include "board.h"
#include "main.h"

static volatile uint16_t s_adc_raw[ADC_CH_COUNT] = {0U};
static volatile uint8_t s_bemf_phase = 0U;
static volatile uint8_t s_bemf_rising = 1U;

static uint32_t ADC_GetHalChannel(ADC_Channel_t channel)
{
    switch (channel)
    {
        case ADC_CH_CURRENT: return ADC_CURRENT_CHANNEL;
        case ADC_CH_VBUS:    return ADC_VBUS_CHANNEL;
        case ADC_CH_BEMF_U:  return ADC_BEMF_U_CHANNEL;
        case ADC_CH_BEMF_V:  return ADC_BEMF_V_CHANNEL;
        case ADC_CH_BEMF_W:  return ADC_BEMF_W_CHANNEL;
        default:             return ADC_CURRENT_CHANNEL;
    }
}

void ADC_Init(void)
{
    ADC_ChannelConfTypeDef sConfig = {0};

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

    sConfig.Channel = ADC_CURRENT_CHANNEL;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

void ADC_StartConversion(void)
{
    (void)HAL_ADC_Start(&hadc1);
}

uint16_t ADC_GetRaw(ADC_Channel_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};

    if (channel >= ADC_CH_COUNT)
    {
        return 0U;
    }

    sConfig.Channel = ADC_GetHalChannel(channel);
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        return 0U;
    }

    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return 0U;
    }

    if (HAL_ADC_PollForConversion(&hadc1, 10U) != HAL_OK)
    {
        (void)HAL_ADC_Stop(&hadc1);
        return 0U;
    }

    s_adc_raw[channel] = (uint16_t)HAL_ADC_GetValue(&hadc1);
    (void)HAL_ADC_Stop(&hadc1);

    return s_adc_raw[channel];
}

float ADC_GetCurrent_A(void)
{
    float voltage_mv = ((float)ADC_GetRaw(ADC_CH_CURRENT) / (float)ADC_RESOLUTION) * (float)ADC_VREF_MV;
    return (voltage_mv / 1000.0f) / (CURRENT_SENSE_GAIN * ((float)CURRENT_SENSE_SHUNT_mR / 1000.0f));
}

float ADC_GetVbus_V(void)
{
    float adc_v = ((float)ADC_GetRaw(ADC_CH_VBUS) / (float)ADC_RESOLUTION) * ((float)ADC_VREF_MV / 1000.0f);
    return adc_v * VBUS_DIVIDER_RATIO;
}

float ADC_GetBemf_V(uint8_t phase)
{
    ADC_Channel_t ch = ADC_CH_BEMF_U;
    if (phase == 1U)
    {
        ch = ADC_CH_BEMF_V;
    }
    else if (phase == 2U)
    {
        ch = ADC_CH_BEMF_W;
    }

    return ((float)ADC_GetRaw(ch) / (float)ADC_RESOLUTION) * ((float)ADC_VREF_MV / 1000.0f);
}

void ADC_ConversionComplete_Callback(void)
{
}

void ADC_SelectBEMFChannel(uint8_t phase, uint8_t rising)
{
    s_bemf_phase = (phase < 3U) ? phase : 0U;
    s_bemf_rising = rising ? 1U : 0U;
}

uint8_t ADC_BEMFZeroCrossDetected(void)
{
    float bemf_v = ADC_GetBemf_V(s_bemf_phase);
    float neutral_v = (ADC_GetVbus_V() / VBUS_DIVIDER_RATIO) * BEMF_NEUTRAL_RATIO;

    if (s_bemf_rising != 0U)
    {
        return (bemf_v >= neutral_v) ? 1U : 0U;
    }

    return (bemf_v <= neutral_v) ? 1U : 0U;
}
