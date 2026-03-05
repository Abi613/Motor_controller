#include "motor.h"
#include "mode.h"
#include "fault.h"
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "pi.h"
#include "board.h"
#include <stdint.h>

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

static uint32_t s_ol_period_ms  = OL_START_PERIOD_MS;
static uint32_t s_ol_elapsed_ms = 0U;

static uint8_t  s_bemf_lock_cnt = 0U;
static uint8_t  s_bemf_locked   = 0U;
static uint32_t s_blank_end_ms  = 0U;

static volatile uint32_t s_last_zc_ms = 0U;
static volatile uint32_t s_half_period_ms  = 0U;

static uint32_t s_align_elapsed_ms = 0U;
static uint32_t s_last_comm_ms = 0U;

static void     Commutate(uint8_t step, uint8_t duty);
static void     UpdateAlignment(SystemMode_t phase);
static void     UpdateOpenLoop(void);
static void     UpdateClosedLoop(void);
static uint32_t GetTickMs(void);
static uint8_t  ReadHallStep(uint8_t *step_out);

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
    s_align_elapsed_ms  = 0U;
    s_last_zc_ms        = 0U;
    s_half_period_ms    = 0U;
    s_last_comm_ms      = 0U;

    PI_Init(&s_pi, 0.35f, 0.12f, (float)CONTROL_LOOP_MS / 1000.0f,
            (float)MOTOR_MIN_DUTY, (float)MOTOR_MAX_DUTY);

    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    PWM_SetDuty(0U);
}

void Motor_Update(void)
{
    uint32_t now = GetTickMs();

    if (!s_motor_enabled) { return; }

    if (ADC_GetCurrent_A() > FAULT_CURRENT_LIMIT_A)
    {
        Fault_Trigger(FAULT_OVERCURRENT);
        return;
    }

    if ((now - s_last_comm_ms) > STALL_TIMEOUT_MS && Mode_Get() != MOTOR_IDLE)
    {
        Fault_Trigger(FAULT_STALL);
        return;
    }

    if (MOTOR_COMM_MODE_DEFAULT == 1U)
    {
        uint8_t hall_step = 0U;
        if (ReadHallStep(&hall_step) != 0U)
        {
            s_comm_step = hall_step;
            Commutate(s_comm_step, s_duty_pct);
            s_last_comm_ms = now;
        }
    }
    else if ((Mode_Get() == MOTOR_OPEN_LOOP) || (Mode_Get() == MOTOR_CLOSED_LOOP))
    {
        if ((now >= s_blank_end_ms) && (ADC_BEMFZeroCrossDetected() != 0U))
        {
            Motor_BEMF_ZeroCrossISR();
        }
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
    s_align_elapsed_ms  = 0U;
    s_comm_step         = 0U;
    s_duty_pct          = STARTUP_DUTY_PCT;
    s_half_period_ms    = 0U;
    s_last_zc_ms        = 0U;

    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_SET);
    PWM_Start();
    Commutate(s_comm_step, s_duty_pct);
    s_last_comm_ms = GetTickMs();
    s_motor_enabled = 1U;
}

void Motor_Disable(void)
{
    PWM_Stop();
    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    s_motor_enabled = 0U;
    s_bemf_locked   = 0U;
    PI_Reset(&s_pi);
}

void Motor_EmergencyStop(void)
{
    PWM_Stop();
    GPIO_WritePin(MOTOR_EN_PORT, MOTOR_EN_PIN, GPIO_PIN_RESET);
    s_motor_enabled = 0U;
    s_target_speed  = 0.0f;
    s_bemf_locked   = 0U;
}

void Motor_CommutateNext(void)
{
    uint32_t now = GetTickMs();
    uint32_t blank_ms;

    s_comm_step = (s_comm_step + 1U) % BLDC_STEPS;
    Commutate(s_comm_step, s_duty_pct);

    blank_ms = (s_ol_period_ms * BEMF_BLANK_PCT) / 100U;
    if (blank_ms < 1U)
    {
        blank_ms = 1U;
    }

    s_blank_end_ms = now + blank_ms;
    s_last_comm_ms = now;
}

void Motor_BEMF_ZeroCrossISR(void)
{
    uint32_t now = GetTickMs();

    if (now < s_blank_end_ms) { return; }

    if (s_last_zc_ms != 0U)
    {
        uint32_t zc_period = now - s_last_zc_ms;
        s_half_period_ms = (zc_period > 1U) ? (zc_period / 2U) : 1U;
    }
    s_last_zc_ms = now;

    if (Mode_Get() == MOTOR_CLOSED_LOOP)
    {
        Board_ScheduleOnceMs(s_half_period_ms, Motor_CommutateNext);
    }
    else if (Mode_Get() == MOTOR_OPEN_LOOP)
    {
        if (++s_bemf_lock_cnt >= BEMF_LOCK_COUNT) { s_bemf_locked = 1U; }
    }
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
        float half_s = (float)s_half_period_ms / 1000.0f;
        s_actual_speed = (1.0f / (2.0f * half_s)) * 60.0f / (float)MOTOR_POLE_PAIRS;
    }

    s_duty_pct = (uint8_t)PI_Compute(&s_pi, s_target_speed, s_actual_speed);
}

static void Commutate(uint8_t step, uint8_t duty)
{
    if (step >= BLDC_STEPS) { return; }
    PWM_SetStep(s_comm_table[step].high_phase,
                s_comm_table[step].low_phase,
                s_comm_table[step].float_phase,
                duty);
    ADC_SelectBEMFChannel(s_comm_table[step].float_phase, s_comm_table[step].zc_rising);
}

static uint32_t GetTickMs(void)
{
    return Board_GetTickMs();
}

static uint8_t ReadHallStep(uint8_t *step_out)
{
    uint8_t hall_state = 0U;

    hall_state |= (GPIO_ReadPin(HALL_U_PORT, HALL_U_PIN) == GPIO_PIN_SET) ? 0x4U : 0U;
    hall_state |= (GPIO_ReadPin(HALL_V_PORT, HALL_V_PIN) == GPIO_PIN_SET) ? 0x2U : 0U;
    hall_state |= (GPIO_ReadPin(HALL_W_PORT, HALL_W_PIN) == GPIO_PIN_SET) ? 0x1U : 0U;

    switch (hall_state)
    {
        case 0x1U: *step_out = 0U; return 1U;
        case 0x5U: *step_out = 1U; return 1U;
        case 0x4U: *step_out = 2U; return 1U;
        case 0x6U: *step_out = 3U; return 1U;
        case 0x2U: *step_out = 4U; return 1U;
        case 0x3U: *step_out = 5U; return 1U;
        default:   return 0U;
    }
}
