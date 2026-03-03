/**
 * @file    pwm.c
 * @brief   PWM driver using TIM2 Channel 1
 * @details Duty cycle is expressed 0–100%. Internal conversion maps to
 *          timer CCR register using PWM_PERIOD_TICKS from board.h.
 *
 * @note    PWM_Stop() sets duty to 0 but leaves the timer running to avoid
 *          glitches. Use Motor_EmergencyStop() for safety-critical shutdown.
 */

#include "pwm.h"
#include "board.h"
#include "stm32f1xx_hal_tim.h"

/*===========================================================================*/
/* PRIVATE VARIABLES                                                          */
/*===========================================================================*/
static uint8_t s_current_duty = 0U; /**< Last set duty cycle (0–100 %)     */

void Error_Handler();
/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Initialize PWM peripheral and start output at 0 % duty.
 */
void PWM_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED3;
  htim1.Init.Period = 1800;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
     Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
     Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 72;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}
/**
 * @brief  Set PWM duty cycle.
 * @param  duty_percent  Duty cycle [0, MOTOR_MAX_DUTY]. Clamped internally.
 */
void PWM_SetDuty(uint8_t duty_percent)
{
    if (duty_percent > MOTOR_MAX_DUTY)
    {
        duty_percent = MOTOR_MAX_DUTY;
    }

    s_current_duty = duty_percent;
    uint32_t ccr = ((uint32_t)duty_percent * PWM_PERIOD_TICKS) / 100U;
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1, ccr);
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2, ccr);
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3, ccr);

}

/**
 * @brief  Enable PWM output (re-arm after stop without re-initializing).
 */
void PWM_Start(void)
{
	 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

	  __HAL_TIM_MOE_ENABLE(&htim1);
}

/**
 * @brief  Set duty to 0 % and stop PWM output signal.
 */
void PWM_Stop(void)
{
    PWM_SetDuty(0U);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);
}

/**
 * @brief  Return the current duty cycle setting.
 * @retval Duty cycle [0–100 %]
 */
uint8_t PWM_GetDuty(void)
{
    return s_current_duty;
}
