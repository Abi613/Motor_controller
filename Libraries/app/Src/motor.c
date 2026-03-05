#include "motor.h"
#include "mode.h"
#include "fault.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "pi.h"
#include "board.h"
#include <stdint.h>

/* --------------------------------------------------------------------------
 * Commutation table: high-PWM phase, low-brake phase, floating phase, ZC dir
 * -------------------------------------------------------------------------- */
typedef enum { PH_U = 0, PH_V = 1, PH_W = 2 } Phase_t;

typedef struct
{
    uint8_t high_phase;
    uint8_t low_phase;
    uint8_t float_phase;
    uint8_t zc_rising;
} CommStep_t;

static const CommStep_t s_comm_table[BLDC_STEPS] =
{
    { PH_U, PH_V, PH_W, 1U },
    { PH_U, PH_W, PH_V, 0U },
    { PH_V, PH_W, PH_U, 1U },
    { PH_V, PH_U, PH_W, 0U },
    { PH_W, PH_U, PH_V, 1U },
    { PH_W, PH_V, PH_U, 0U },
};

static PI_Controller_t s_pi;
static float   s_target_speed   = 0.0f;
static float   s_actual_speed   = 0.0f;
static uint8_t s_motor_enabled  = 0U;
static uint8_t s_comm_step      = 0U;
static uint8_t s_duty_pct       = 0U;

/* Open-loop ramp */
static uint32_t s_ol_period_ms  = OL_START_PERIOD_MS;
static uint32_t s_ol_elapsed_ms = 0U;

/* Back-EMF lock detection */
static uint8_t  s_bemf_lock_cnt = 0U;
static uint8_t  s_bemf_locked   = 0U;
static uint32_t s_blank_end_ms  = 0U;

/* Closed-loop ZC timing (written in ISR) */
static volatile uint32_t s_zc_timestamp_ms = 0U;
static volatile uint32_t s_half_period_ms  = 0U;
static volatile uint8_t  s_zc_pending      = 0U;

/* Alignment timer */
static uint32_t s_align_elapsed_ms = 0U;

static void     Commutate(uint8_t step, uint8_t duty);
static void     UpdateAlignment(SystemMode_t phase);
static void     UpdateOpenLoop(void);
static void     UpdateClosedLoop(void);
static uint32_t GetTickMs(void);

/* -------------------------------------------------------------------------- */

void Motor_Init(void)
{
    s_target_speed      = 0.0f;
    s_actual_speed      = 0.0f;
    s_motor_enabled     = 0U;
    s_comm_step         = 0U;
    s_duty_pct          = 0U;
    s_ol_period_ms      = OL_START_PERIOD_MS;
    s_ol_elapsed_ms     = 0U;
    s_bemf_lock_cnt     = 0U;
    s_bemf_locked       = 0U;
    s_zc_pending        = 0U;
    s_align_elapsed_ms  = 0U;

    PI_Init(&s_pi,
            /* Kp      */ 0.5f,
            /* Ki      */ 0.1f,
            /* dt      */ (float)CONTROL_LOOP_MS / 1000.0f,
            /* out_min */ (float)MOTOR_MIN_DUTY,
            /* out_max */ (float)MOTOR_MAX_DUTY);

    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    PWM_SetDuty(0U);
}

void Motor_Update(void)
{
    if (!s_motor_enabled) { return; }

    if (ADC_GetCurrent_A() > FAULT_CURRENT_LIMIT_A)
    {
        Fault_Trigger(FAULT_OVERCURRENT);
        return;
    }

    switch (Mode_Get())
    {
        case MOTOR_ALIGN_1:
        case MOTOR_ALIGN_2:     UpdateAlignment(Mode_Get()); break;
        case MOTOR_OPEN_LOOP:   UpdateOpenLoop();            break;
        case MOTOR_CLOSED_LOOP: UpdateClosedLoop();          break;
        default:                                             break;
    }
}

void Motor_Enable(void)
{
    s_ol_period_ms      = OL_START_PERIOD_MS;
    s_ol_elapsed_ms     = 0U;
    s_bemf_lock_cnt     = 0U;
    s_bemf_locked       = 0U;
    s_zc_pending        = 0U;
    s_align_elapsed_ms  = 0U;
    s_comm_step         = 0U;

    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_SET);
    PWM_Start();
    s_motor_enabled = 1U;
}

void Motor_Disable(void)
{
    PWM_SetDuty(0U);
    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    s_motor_enabled = 0U;
    s_bemf_locked   = 0U;
    PI_Reset(&s_pi);
}

void Motor_EmergencyStop(void)
{
    PWM_SetDuty(0U);
    PWM_Stop();
    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    s_motor_enabled = 0U;
    s_target_speed  = 0.0f;
    s_bemf_locked   = 0U;
}

void Motor_CommutateNext(void)
{
    s_comm_step    = (s_comm_step + 1U) % BLDC_STEPS;
    Commutate(s_comm_step, s_duty_pct);
    s_blank_end_ms = GetTickMs() + (s_ol_period_ms * BEMF_BLANK_PCT / 100U);
}

void Motor_BEMF_ZeroCrossISR(void)
{
    uint32_t now = GetTickMs();

    if (now < s_blank_end_ms) { return; }

    if (s_zc_timestamp_ms != 0U)
    {
        s_half_period_ms = (now - s_zc_timestamp_ms) / 2U;
    }
    s_zc_timestamp_ms = now;

    if (Mode_Get() == MOTOR_CLOSED_LOOP)
    {
        Board_ScheduleOnceMs(s_half_period_ms, Motor_CommutateNext);
    }
    else if (Mode_Get() == MOTOR_OPEN_LOOP)
    {
        if (++s_bemf_lock_cnt >= BEMF_LOCK_COUNT) { s_bemf_locked = 1U; }
    }

    s_zc_pending = 1U;
}

void Motor_SetTargetSpeed(float speed_rpm)
{
    if (speed_rpm < 0.0f) { speed_rpm = 0.0f; }
    s_target_speed = speed_rpm;
}

float   Motor_GetTargetSpeed(void) { return s_target_speed;     }
float   Motor_GetActualSpeed(void) { return s_actual_speed;     }
float   Motor_GetCurrent(void)     { return ADC_GetCurrent_A(); }
uint8_t Motor_IsEnabled(void)      { return s_motor_enabled;    }

/* -------------------------------------------------------------------------- */

static void UpdateAlignment(SystemMode_t phase)
{
    uint8_t  step    = (phase == MOTOR_ALIGN_1) ? 0U : 1U;
    uint32_t hold_ms = (phase == MOTOR_ALIGN_1) ? ALIGN1_HOLD_MS : ALIGN2_HOLD_MS;

    if (s_align_elapsed_ms == 0U)
    {
        Commutate(step, ALIGN_DUTY_PCT);
        s_comm_step = step;
    }

    s_align_elapsed_ms += (uint32_t)CONTROL_LOOP_MS;

    if (s_align_elapsed_ms >= hold_ms)
    {
        s_align_elapsed_ms = 0U;
        Mode_Set((phase == MOTOR_ALIGN_1) ? MOTOR_ALIGN_2 : MOTOR_OPEN_LOOP);
    }
}

static void UpdateOpenLoop(void)
{
    s_ol_elapsed_ms += (uint32_t)CONTROL_LOOP_MS;

    if (s_ol_elapsed_ms >= s_ol_period_ms)
    {
        s_ol_elapsed_ms = 0U;
        Motor_CommutateNext();

        if (s_ol_period_ms > OL_MIN_PERIOD_MS + OL_RAMP_STEP_MS)
        {
            s_ol_period_ms -= OL_RAMP_STEP_MS;
        }
        else
        {
            s_ol_period_ms = OL_MIN_PERIOD_MS;
        }
    }

    if (s_bemf_locked)
    {
        s_bemf_lock_cnt = 0U;
        Mode_Set(MOTOR_CLOSED_LOOP);
    }
}

static void UpdateClosedLoop(void)
{
    if (s_half_period_ms > 0U)
    {
        float half_s   = (float)s_half_period_ms / 1000.0f;
        s_actual_speed = (1.0f / (2.0f * half_s)) * 60.0f / (float)MOTOR_POLE_PAIRS;
    }

    s_duty_pct   = (uint8_t)PI_Compute(&s_pi, s_target_speed, s_actual_speed);
    s_zc_pending = 0U;
}

static void Commutate(uint8_t step, uint8_t duty)
{
    if (step >= BLDC_STEPS) { return; }
    const CommStep_t *cs = &s_comm_table[step];
    PWM_SetStep(cs->high_phase, cs->low_phase, cs->float_phase, duty);
    ADC_SelectBEMFChannel(cs->float_phase, cs->zc_rising);
}

static uint32_t GetTickMs(void)
{
    return Board_GetTickMs();
}
