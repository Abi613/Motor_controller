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
#include "../board.h"

/*===========================================================================*/
/* PRIVATE VARIABLES                                                          */
/*===========================================================================*/
static uint8_t s_current_duty = 0U;   /**< Last set duty cycle (0–100 %)     */

/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Initialize PWM peripheral and start output at 0 % duty.
 */
void PWM_Init(void)
{
    s_current_duty = 0U;
    HAL_TIM_PWM_Start(&htim2, PWM_CHANNEL);
    PWM_SetDuty(0U);
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
    __HAL_TIM_SET_COMPARE(&htim2, PWM_CHANNEL, ccr);
}

/**
 * @brief  Enable PWM output (re-arm after stop without re-initializing).
 */
void PWM_Start(void)
{
    HAL_TIM_PWM_Start(&htim1, PWM_CHANNEL);
}

/**
 * @brief  Set duty to 0 % and stop PWM output signal.
 */
void PWM_Stop(void)
{
    PWM_SetDuty(0U);
    HAL_TIM_PWM_Stop(&htim1, PWM_CHANNEL);
}

/**
 * @brief  Return the current duty cycle setting.
 * @retval Duty cycle [0–100 %]
 */
uint8_t PWM_GetDuty(void)
{
    return s_current_duty;
}
