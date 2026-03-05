#include "fault.h"
#include "mode.h"
#include "adc.h"
#include "gpio.h"
#include "board.h"

static volatile FaultCode_t s_active_faults = FAULT_NONE;
static volatile uint8_t s_estop_pending = 0U;

void Fault_Init(void)
{
    s_active_faults = FAULT_NONE;
    s_estop_pending = 0U;
}

void Fault_Update(void)
{
    float vbus = ADC_GetVbus_V();

    if (s_estop_pending != 0U)
    {
        s_estop_pending = 0U;
        s_active_faults |= FAULT_ESTOP;
        Mode_Set(MOTOR_FAULT);
    }

    if (ADC_GetCurrent_A() > FAULT_CURRENT_LIMIT_A)
    {
        Fault_Trigger(FAULT_OVERCURRENT);
    }

    if (vbus > FAULT_VBUS_OV_LIMIT_V)
    {
        Fault_Trigger(FAULT_OVERVOLTAGE);
    }
    else if (vbus < FAULT_VBUS_UV_LIMIT_V)
    {
        Fault_Trigger(FAULT_UNDERVOLT);
    }

    if (GPIO_ReadPin(FAULT_IN_PORT, FAULT_IN_PIN) == GPIO_PIN_RESET)
    {
        Fault_Trigger(FAULT_HW_PIN);
    }
}

void Fault_Trigger(FaultCode_t code)
{
    s_active_faults |= (uint8_t)code;
    Mode_Set(MOTOR_FAULT);
}

void Fault_TriggerEstop(void)
{
    s_estop_pending = 1U;
}

void Fault_Clear(FaultCode_t code)
{
    if (code == FAULT_ESTOP) return;
    s_active_faults &= ~((uint8_t)code);

    if (!Fault_AnyActive())
    {
        Mode_Set(MOTOR_IDLE);
    }
}

void Fault_ClearAll(void)
{
    s_active_faults &= FAULT_ESTOP;
    if (!Fault_AnyActive())
    {
        Mode_Set(MOTOR_IDLE);
    }
}

uint8_t Fault_IsActive(FaultCode_t code)
{
    return (s_active_faults & (uint8_t)code) ? 1U : 0U;
}

uint8_t Fault_AnyActive(void)
{
    return (s_active_faults != FAULT_NONE) ? 1U : 0U;
}

FaultCode_t Fault_GetAll(void)
{
    return s_active_faults;
}

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
        case FAULT_STALL:       return "STALL";
        default:                return "MULTI";
    }
}
