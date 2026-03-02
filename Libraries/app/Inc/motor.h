/**
 * @file    motor.h
 * @brief   Motor control application layer public interface
 */

#ifndef APP_MOTOR_H
#define APP_MOTOR_H

#include <stdint.h>

void  Motor_Init(void);
void  Motor_Update(void);
void  Motor_Enable(void);
void  Motor_Disable(void);
void  Motor_EmergencyStop(void);
void  Motor_SetTargetSpeed(float speed_rpm);
float Motor_GetTargetSpeed(void);
float Motor_GetActualSpeed(void);
float Motor_GetCurrent(void);
uint8_t Motor_IsEnabled(void);

#endif /* APP_MOTOR_H */
