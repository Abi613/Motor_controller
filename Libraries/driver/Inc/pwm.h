/**
 * @file    pwm.h
 * @brief   PWM driver public interface
 */

#ifndef DRIVERS_PWM_H
#define DRIVERS_PWM_H

#include <stdint.h>

void    PWM_Init(void);

void    PWM_SetDuty(uint8_t duty_percent);
void    PWM_Start(void);
void    PWM_Stop(void);
uint8_t PWM_GetDuty(void);

#endif /* DRIVERS_PWM_H */
