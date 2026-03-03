/**
 * @file    mode.c
 * @brief   System operating mode state machine
 * @details Transitions between modes are validated here. Illegal transitions
 *          (e.g. FAULT → RUN without clearing) are silently rejected.
 *
 *          Valid transitions:
 *          STARTUP → IDLE
 *          IDLE    → RUN
 *          RUN     → IDLE
 *          ANY     → FAULT
 *          ANY     → ESTOP   (latched — only hardware reset clears)
 *          FAULT   → IDLE    (via Fault_Clear())
 */

#include "mode.h"
#include "fault.h"
#include "motor.h"
#include "gpio.h"
#include "board.h"
#include <string.h>

/*===========================================================================*/
/* PRIVATE VARIABLES                                                          */
/*===========================================================================*/
static SystemMode_t s_current_mode = MODE_STARTUP;
static SystemMode_t s_previous_mode = MODE_STARTUP;

/** @brief Lookup table for mode name strings (matches SystemMode_t order) */
static const char * const s_mode_names[MODE_COUNT] =
{
    "IDLE", "RUN", "FAULT", "ESTOP", "STARTUP"
};

/*===========================================================================*/
/* PRIVATE FUNCTIONS                                                          */
/*===========================================================================*/
static uint8_t Mode_IsTransitionValid(SystemMode_t from, SystemMode_t to);
static void    Mode_OnEnter(SystemMode_t mode);
static void    Mode_OnExit(SystemMode_t mode);

/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Set initial mode to STARTUP, then transition to IDLE.
 */
void Mode_Init(void)
{
    s_current_mode  = MODE_STARTUP;
    s_previous_mode = MODE_STARTUP;
    Mode_Set(MODE_IDLE);
}

/**
 * @brief  Periodic mode update. Reads mode selector GPIO and handles transitions.
 *         Called by scheduler at DISPLAY_UPDATE_MS interval.
 */
void Mode_Update(void)
{
    /* TODO: Read MODE_SEL_PIN and determine requested mode */
    /* TODO: If fault condition is active, force MODE_FAULT */
}

/**
 * @brief  Request a mode transition. Validates the transition before applying.
 * @param  new_mode  Desired new mode.
 */
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

/**
 * @brief  Return the current operating mode.
 * @retval Current SystemMode_t value
 */
SystemMode_t Mode_Get(void)
{
    return s_current_mode;
}

/**
 * @brief  Return a human-readable name for a mode (for display/logging).
 * @param  mode  Mode to look up
 * @retval Null-terminated string, e.g. "RUN"
 */
const char* Mode_GetName(SystemMode_t mode)
{
    if (mode >= MODE_COUNT)
    {
        return "UNKNOWN";
    }
    return s_mode_names[mode];
}

/*===========================================================================*/
/* PRIVATE FUNCTIONS                                                          */
/*===========================================================================*/
/**
 * @brief  Validate that the requested transition is allowed.
 * @retval 1 if allowed, 0 if rejected
 */
static uint8_t Mode_IsTransitionValid(SystemMode_t from, SystemMode_t to)
{
    if (to == MODE_ESTOP) return 1U;  /* ESTOP always allowed */
    if (to == MODE_FAULT) return 1U;  /* FAULT always allowed */

    switch (from)
    {
        case MODE_STARTUP: return (to == MODE_IDLE)  ? 1U : 0U;
        case MODE_IDLE:    return (to == MODE_RUN)   ? 1U : 0U;
        case MODE_RUN:     return (to == MODE_IDLE)  ? 1U : 0U;
        case MODE_FAULT:   return (to == MODE_IDLE)  ? 1U : 0U;
        case MODE_ESTOP:   return 0U;  /* Latched — no exit without reset */
        default:           return 0U;
    }
}

/**
 * @brief  Actions to perform when entering a new mode.
 */
static void Mode_OnEnter(SystemMode_t mode)
{
    switch (mode)
    {
        case MODE_RUN:
            Motor_Enable();
            break;
        case MODE_FAULT:
        case MODE_ESTOP:
            Motor_EmergencyStop();
            break;
        default:
            break;
    }
}

/**
 * @brief  Actions to perform when exiting a mode.
 */
static void Mode_OnExit(SystemMode_t mode)
{
    switch (mode)
    {
        case MODE_RUN:
            Motor_Disable();
            break;
        default:
            break;
    }
}
