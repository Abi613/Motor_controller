#ifndef APP_MODE_H
#define APP_MODE_H

#include <stdint.h>

typedef enum
{
    MOTOR_IDLE         = 0,
    MOTOR_ALIGN_1,
    MOTOR_ALIGN_2,
    MOTOR_OPEN_LOOP,
    MOTOR_CLOSED_LOOP,
    MOTOR_FAULT,
    MODE_COUNT
} SystemMode_t;

void         Mode_Init(void);
void         Mode_Update(void);
void         Mode_Set(SystemMode_t new_mode);
SystemMode_t Mode_Get(void);
const char*  Mode_GetName(SystemMode_t mode);

#endif /* APP_MODE_H */
