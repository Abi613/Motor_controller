/**
 * @file    mode.h
 * @brief   Operating mode state machine public interface
 */

#ifndef APP_MODE_H
#define APP_MODE_H

#include <stdint.h>

/** @brief All possible system operating modes */
typedef enum
{
    MODE_IDLE    = 0,   /**< System powered, motor stopped               */
    MODE_RUN     = 1,   /**< Normal motor operation                      */
    MODE_FAULT   = 2,   /**< Fault detected — motor disabled             */
    MODE_ESTOP   = 3,   /**< Emergency stop — latched, requires reset    */
    MODE_STARTUP = 4,   /**< Brief init state before entering IDLE       */
    MODE_COUNT          /**< Total number of modes (keep last)           */
} SystemMode_t;

void        Mode_Init(void);
void        Mode_Update(void);
void        Mode_Set(SystemMode_t new_mode);
SystemMode_t Mode_Get(void);
const char* Mode_GetName(SystemMode_t mode);

#endif /* APP_MODE_H */
