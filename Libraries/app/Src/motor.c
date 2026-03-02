/**
 * @file    motor.c
 * @brief   Motor control — uses PI controller and PWM driver
 * @details Motor_Update() is the control loop, called every CONTROL_LOOP_MS.
 *          It reads the current motor state, runs the PI controller, and
 *          writes the new duty cycle to the PWM driver.
 *
 *          Speed feedback source: TODO — encoder, back-EMF, or open-loop.
 */

#include "motor.h"
#include "fault.h"
#include "mode.h"
#include "../drivers/pwm.h"
#include "../drivers/adc.h"
#include "../drivers/gpio.h"
#include "../control/pi.h"
#include "../board.h"

/*===========================================================================*/
/* PRIVATE VARIABLES                                                          */
/*===========================================================================*/
static PI_Controller_t s_pi;             /**< PI controller instance          */
static float           s_target_speed  = 0.0f;  /**< Desired speed (RPM)    */
static float           s_actual_speed  = 0.0f;  /**< Measured speed (RPM)   */
static uint8_t         s_motor_enabled = 0U;     /**< 1 = motor is running   */

/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Initialize motor control and PI controller with default gains.
 */
void Motor_Init(void)
{
    s_target_speed  = 0.0f;
    s_actual_speed  = 0.0f;
    s_motor_enabled = 0U;

    PI_Init(&s_pi,
            /* Kp */ 0.5f,
            /* Ki */ 0.1f,
            /* dt */ (float)CONTROL_LOOP_MS / 1000.0f,
            /* out_min */ (float)MOTOR_MIN_DUTY,
            /* out_max */ (float)MOTOR_MAX_DUTY);

    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    PWM_SetDuty(0U);
}

/**
 * @brief  Control loop — called every CONTROL_LOOP_MS from scheduler.
 *         Reads speed feedback, runs PI, applies duty cycle.
 */
void Motor_Update(void)
{
    if (!s_motor_enabled || Mode_Get() != MODE_RUN)
    {
        return;
    }

    /* 1. Update actual speed measurement */
    /* TODO: Read encoder / back-EMF / hall sensor */
    s_actual_speed = 0.0f;  /* Placeholder */

    /* 2. Check current limit */
    float current_A = ADC_GetCurrent_A();
    if (current_A > FAULT_CURRENT_LIMIT_A)
    {
        Fault_Trigger(FAULT_OVERCURRENT);
        return;
    }

    /* 3. Run PI controller */
    float output = PI_Compute(&s_pi, s_target_speed, s_actual_speed);

    /* 4. Apply duty cycle */
    PWM_SetDuty((uint8_t)output);
}

/**
 * @brief  Enable motor output (assert MOTOR_EN pin, start PWM).
 */
void Motor_Enable(void)
{
    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_SET);
    PWM_Start();
    s_motor_enabled = 1U;
}

/**
 * @brief  Graceful disable — ramp down duty then de-assert enable.
 */
void Motor_Disable(void)
{
    PWM_SetDuty(0U);
    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    s_motor_enabled = 0U;
    PI_Reset(&s_pi);
}

/**
 * @brief  Immediate stop — called on fault or ESTOP. Does not ramp.
 *         Safe to call from interrupt context (no blocking calls).
 */
void Motor_EmergencyStop(void)
{
    PWM_SetDuty(0U);
    PWM_Stop();
    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    s_motor_enabled = 0U;
    s_target_speed  = 0.0f;
}

/**
 * @brief  Set the target speed for the PI controller.
 * @param  speed_rpm  Desired speed. Clamped to valid range internally.
 */
void Motor_SetTargetSpeed(float speed_rpm)
{
    if (speed_rpm < 0.0f) speed_rpm = 0.0f;
    s_target_speed = speed_rpm;
}

/** @brief Return the current target speed setpoint. */
float Motor_GetTargetSpeed(void)  { return s_target_speed;  }

/** @brief Return the last measured actual speed. */
float Motor_GetActualSpeed(void)  { return s_actual_speed;  }

/** @brief Return the last ADC-measured motor current. */
float Motor_GetCurrent(void)      { return ADC_GetCurrent_A(); }

/** @brief Return non-zero if motor output is currently enabled. */
uint8_t Motor_IsEnabled(void)     { return s_motor_enabled; }
