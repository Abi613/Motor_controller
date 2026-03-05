#ifndef APP_MOTOR_H
#define APP_MOTOR_H

#include <stdint.h>

#define BLDC_STEPS              6U

#define OL_START_PERIOD_MS      20U
#define OL_MIN_PERIOD_MS        4U
#define OL_RAMP_STEP_MS         1U

#define BEMF_BLANK_PCT          30U
#define BEMF_LOCK_COUNT         6U

#define ALIGN1_HOLD_MS          200U
#define ALIGN2_HOLD_MS          100U
#define ALIGN_DUTY_PCT          30U

void    Motor_Init(void);
void    Motor_Update(void);
void    Motor_Enable(void);
void    Motor_Disable(void);
void    Motor_EmergencyStop(void);
void    Motor_SetTargetSpeed(float speed_rpm);
float   Motor_GetTargetSpeed(void);
float   Motor_GetActualSpeed(void);
float   Motor_GetCurrent(void);
uint8_t Motor_IsEnabled(void);
void    Motor_CommutateNext(void);
void    Motor_BEMF_ZeroCrossISR(void);

#endif /* APP_MOTOR_H */
