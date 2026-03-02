/**
 * @file    fault.c
 * @brief   Fault detection, latching, and recovery management
 * @details Faults are bitmask-based. Multiple can be active at once.
 *          ESTOP is always latched and can only be cleared by hardware reset.
 *          Other faults can be cleared via Fault_Clear() once the condition
 *          has been resolved.
 *
 *          Fault_Update() polls analog/digital inputs and triggers faults
 *          automatically. Called every FAULT_CHECK_MS by the scheduler.
 */

#include "fault.h"
#include "mode.h"
#include "../drivers/adc.h"
#include "../drivers/gpio.h"
#include "../board.h"

/*===========================================================================*/
/* PRIVATE VARIABLES                                                          */
/*===========================================================================*/
/** @brief Bitmask of currently active faults */
static volatile FaultCode_t s_active_faults = FAULT_NONE;

/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Initialize fault module. Clear all fault flags.
 */
void Fault_Init(void)
{
    s_active_faults = FAULT_NONE;
}

/**
 * @brief  Periodic fault check — polls ADC thresholds and fault GPIO.
 *         Non-blocking. Called from scheduler every FAULT_CHECK_MS.
 */
void Fault_Update(void)
{
    /* --- Overcurrent check --- */
    if (ADC_GetCurrent_A() > FAULT_CURRENT_LIMIT_A)
    {
        Fault_Trigger(FAULT_OVERCURRENT);
    }

    /* --- Bus overvoltage check --- */
    /* TODO: Define VBUS_MAX_V in board.h and compare ADC_GetVbus_V() */

    /* --- Hardware fault pin check --- */
    if (GPIO_ReadPin(FAULT_IN_PORT, FAULT_IN_PIN) == GPIO_PIN_RESET)
    {
        Fault_Trigger(FAULT_HW_PIN);
    }
}

/**
 * @brief  Set a fault code and transition system to FAULT mode.
 * @param  code  FaultCode_t bitmask value to latch
 */
void Fault_Trigger(FaultCode_t code)
{
    s_active_faults |= (uint8_t)code;
    Mode_Set(MODE_FAULT);
}

/**
 * @brief  Assert E-STOP fault — latched, transitions to ESTOP mode.
 *         Called from GPIO EXTI interrupt (must be ISR-safe).
 */
void Fault_TriggerEstop(void)
{
    s_active_faults |= FAULT_ESTOP;
    /* Mode_Set is not ISR-safe if it calls HAL — use a flag instead        */
    /* TODO: Set a volatile flag and handle transition in Fault_Update()     */
}

/**
 * @brief  Clear a specific fault code (if condition is resolved).
 * @param  code  Fault code to clear. FAULT_ESTOP is ignored.
 */
void Fault_Clear(FaultCode_t code)
{
    if (code == FAULT_ESTOP) return;   /* ESTOP is hardware-reset-only       */
    s_active_faults &= ~((uint8_t)code);

    if (!Fault_AnyActive())
    {
        Mode_Set(MODE_IDLE);
    }
}

/** @brief Clear all non-ESTOP faults. */
void Fault_ClearAll(void)
{
    s_active_faults &= FAULT_ESTOP;   /* Preserve ESTOP bit if set          */
    if (!Fault_AnyActive())
    {
        Mode_Set(MODE_IDLE);
    }
}

/** @brief Check if a specific fault code is active. */
uint8_t Fault_IsActive(FaultCode_t code)
{
    return (s_active_faults & (uint8_t)code) ? 1U : 0U;
}

/** @brief Return non-zero if any fault is currently active. */
uint8_t Fault_AnyActive(void)
{
    return (s_active_faults != FAULT_NONE) ? 1U : 0U;
}

/** @brief Return the full fault bitmask. */
FaultCode_t Fault_GetAll(void)
{
    return s_active_faults;
}

/**
 * @brief  Return a short name string for a single fault code.
 * @param  code  Must be a single-bit fault code, not a combined mask.
 */
const char* Fault_GetName(FaultCode_t code)
{
    switch (code)
    {
        case FAULT_OVERCURRENT: return "OC";
        case FAULT_OVERVOLTAGE: return "OV";
        case FAULT_UNDERVOLT:   return "UV";
        case FAULT_OVERTEMP:    return "OT";
        case FAULT_ESTOP:       return "ESTOP";
        case FAULT_HW_PIN:      return "HW";
        default:                return "NONE";
    }
}
