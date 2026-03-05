#include "mode.h"
#include "motor.h"
#include "fault.h"
#include "gpio.h"
#include "board.h"

static SystemMode_t s_current_mode  = MOTOR_IDLE;
static SystemMode_t s_previous_mode = MOTOR_IDLE;

static const char * const s_mode_names[MODE_COUNT] =
{
    "IDLE", "ALIGN_1", "ALIGN_2", "OPEN_LOOP", "CLOSED_LOOP", "FAULT"
};

static uint8_t Mode_IsTransitionValid(SystemMode_t from, SystemMode_t to);
static void    Mode_OnEnter(SystemMode_t mode);
static void    Mode_OnExit(SystemMode_t mode);

void Mode_Init(void)
{
    s_current_mode  = MOTOR_IDLE;
    s_previous_mode = MOTOR_IDLE;
}

void Mode_Update(void)
{
    if (Fault_IsActive() && (s_current_mode != MOTOR_FAULT))
    {
        Mode_Set(MOTOR_FAULT);
    }
}

void Mode_Set(SystemMode_t new_mode)
{
    if (new_mode >= MODE_COUNT)
    {
        return;
    }
    if (!Mode_IsTransitionValid(s_current_mode, new_mode))
    {
        return;
    }

    Mode_OnExit(s_current_mode);
    s_previous_mode = s_current_mode;
    s_current_mode  = new_mode;
    Mode_OnEnter(s_current_mode);
}

SystemMode_t Mode_Get(void)
{
    return s_current_mode;
}

const char* Mode_GetName(SystemMode_t mode)
{
    if (mode >= MODE_COUNT)
    {
        return "UNKNOWN";
    }
    return s_mode_names[mode];
}

static uint8_t Mode_IsTransitionValid(SystemMode_t from, SystemMode_t to)
{
    if (to == MOTOR_FAULT) return 1U;

    switch (from)
    {
        case MOTOR_IDLE:        return (to == MOTOR_ALIGN_1)       ? 1U : 0U;
        case MOTOR_ALIGN_1:     return (to == MOTOR_ALIGN_2)       ? 1U : 0U;
        case MOTOR_ALIGN_2:     return (to == MOTOR_OPEN_LOOP)     ? 1U : 0U;
        case MOTOR_OPEN_LOOP:   return ((to == MOTOR_CLOSED_LOOP) ||
                                        (to == MOTOR_IDLE))        ? 1U : 0U;
        case MOTOR_CLOSED_LOOP: return (to == MOTOR_IDLE)          ? 1U : 0U;
        case MOTOR_FAULT:       return (!Fault_IsActive() &&
                                        (to == MOTOR_IDLE))        ? 1U : 0U;
        default:                return 0U;
    }
}

static void Mode_OnEnter(SystemMode_t mode)
{
    switch (mode)
    {
        case MOTOR_ALIGN_1:
        case MOTOR_ALIGN_2:
        case MOTOR_OPEN_LOOP:
        case MOTOR_CLOSED_LOOP: Motor_Enable();        break;
        case MOTOR_FAULT:       Motor_EmergencyStop(); break;
        default:                                       break;
    }
}

static void Mode_OnExit(SystemMode_t mode)
{
    switch (mode)
    {
        case MOTOR_CLOSED_LOOP:
        case MOTOR_OPEN_LOOP:
            if (s_current_mode != MOTOR_FAULT) { Motor_Disable(); }
            break;
        default:
            break;
    }
}
