/**
 * @file    fault.h
 * @brief   Fault detection and management public interface
 */

#ifndef APP_FAULT_H
#define APP_FAULT_H

#include <stdint.h>

/** @brief Fault type bitmask — multiple faults can be active simultaneously */
typedef enum
{
    FAULT_NONE        = 0x00U,
    FAULT_OVERCURRENT = 0x01U,
    FAULT_OVERVOLTAGE = 0x02U,
    FAULT_UNDERVOLT   = 0x04U,
    FAULT_OVERTEMP    = 0x08U,
    FAULT_ESTOP       = 0x10U,
    FAULT_HW_PIN      = 0x20U,   /**< External fault input asserted          */
} FaultCode_t;

void       Fault_Init(void);
void       Fault_Update(void);
void       Fault_Trigger(FaultCode_t code);
void       Fault_TriggerEstop(void);
void       Fault_Clear(FaultCode_t code);
void       Fault_ClearAll(void);
uint8_t    Fault_IsActive(FaultCode_t code);
uint8_t    Fault_AnyActive(void);
FaultCode_t Fault_GetAll(void);
const char* Fault_GetName(FaultCode_t code);

#endif /* APP_FAULT_H */
