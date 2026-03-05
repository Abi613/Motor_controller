/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */
#include "main.h"
#include "board.h"
#include "gpio.h"
#include "pwm.h"
#include "adc.h"
#include "i2c.h"
#include "mode.h"
#include "fault.h"
#include "motor.h"
#include "display.h"

ADC_HandleTypeDef hadc1;
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim1;

static volatile uint32_t s_sched_due_ms = 0U;
static void (*s_sched_cb)(void) = NULL;

void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  GPIO_Init();
  ADC_Init();
  I2C_Init();
  PWM_Init();

  Mode_Init();
  Fault_Init();
  Motor_Init();
  Display_Init();

  while (1)
  {
    uint32_t now = HAL_GetTick();

    Fault_Update();
    Mode_Update();
    Motor_Update();

    if ((s_sched_cb != NULL) && (now >= s_sched_due_ms))
    {
      void (*cb)(void) = s_sched_cb;
      s_sched_cb = NULL;
      cb();
    }

    Display_Update();
    HAL_Delay(CONTROL_LOOP_MS);
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}


uint32_t Board_GetTickMs(void)
{
  return HAL_GetTick();
}

void Board_ScheduleOnceMs(uint32_t delay_ms, void (*cb)(void))
{
  if (cb == NULL)
  {
    return;
  }

  s_sched_due_ms = HAL_GetTick() + delay_ms;
  s_sched_cb = cb;
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file;
  (void)line;
}
#endif
