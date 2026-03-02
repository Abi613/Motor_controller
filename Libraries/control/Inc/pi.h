/**
 * @file    pi.h
 * @brief   Discrete PI controller — math only, no hardware dependencies
 */

#ifndef CONTROL_PI_H
#define CONTROL_PI_H

/**
 * @brief  PI controller state and configuration.
 *         Allocate one per control loop. Zero-initialize before calling PI_Init().
 */
typedef struct
{
    float kp;           /**< Proportional gain                               */
    float ki;           /**< Integral gain                                   */
    float dt;           /**< Sample period (seconds)                         */
    float out_min;      /**< Output clamp — minimum (anti-windup)            */
    float out_max;      /**< Output clamp — maximum (anti-windup)            */
    float integrator;   /**< Accumulated integral term (internal state)      */
    float last_output;  /**< Last computed output (for monitoring)           */
} PI_Controller_t;

void  PI_Init(PI_Controller_t *pi, float kp, float ki, float dt,
              float out_min, float out_max);
float PI_Compute(PI_Controller_t *pi, float setpoint, float measurement);
void  PI_Reset(PI_Controller_t *pi);
void  PI_SetGains(PI_Controller_t *pi, float kp, float ki);

#endif /* CONTROL_PI_H */
