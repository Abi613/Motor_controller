/**
 * @file    pi.c
 * @brief   Discrete-time PI controller with integral anti-windup (clamping)
 *
 * @details Algorithm:
 *              error      = setpoint - measurement
 *              integrator += Ki * error * dt
 *              integrator  = clamp(integrator, out_min, out_max)   // anti-windup
 *              output      = Kp * error + integrator
 *              output      = clamp(output, out_min, out_max)
 *
 * @note    This file contains NO hardware access. Unit-testable on host PC.
 */

#include "pi.h"

/*===========================================================================*/
/* PRIVATE HELPERS                                                            */
/*===========================================================================*/
static float clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Initialize a PI controller instance.
 * @param  pi       Pointer to PI_Controller_t struct to initialize
 * @param  kp       Proportional gain
 * @param  ki       Integral gain
 * @param  dt       Sample time in seconds (e.g. 0.001 for 1 ms loop)
 * @param  out_min  Minimum output value (also clamps integrator)
 * @param  out_max  Maximum output value (also clamps integrator)
 */
void PI_Init(PI_Controller_t *pi, float kp, float ki, float dt,
             float out_min, float out_max)
{
    pi->kp          = kp;
    pi->ki          = ki;
    pi->dt          = dt;
    pi->out_min     = out_min;
    pi->out_max     = out_max;
    pi->integrator  = 0.0f;
    pi->last_output = 0.0f;
}

/**
 * @brief  Compute one PI control step.
 * @param  pi           Pointer to initialized PI_Controller_t
 * @param  setpoint     Desired value (e.g. target RPM)
 * @param  measurement  Measured value (e.g. actual RPM)
 * @retval Controller output, clamped to [out_min, out_max]
 */
float PI_Compute(PI_Controller_t *pi, float setpoint, float measurement)
{
    float error = setpoint - measurement;

    /* Integral term with anti-windup clamping */
    pi->integrator += pi->ki * error * pi->dt;
    pi->integrator  = clamp(pi->integrator, pi->out_min, pi->out_max);

    /* Proportional + integral */
    float output = (pi->kp * error) + pi->integrator;
    output = clamp(output, pi->out_min, pi->out_max);

    pi->last_output = output;
    return output;
}

/**
 * @brief  Reset integrator and output (e.g. on mode transition or fault).
 * @param  pi  Pointer to PI_Controller_t to reset
 */
void PI_Reset(PI_Controller_t *pi)
{
    pi->integrator  = 0.0f;
    pi->last_output = 0.0f;
}

/**
 * @brief  Update PI gains at runtime (e.g. for auto-tuning or gain scheduling).
 * @param  pi  Pointer to PI_Controller_t
 * @param  kp  New proportional gain
 * @param  ki  New integral gain
 */
void PI_SetGains(PI_Controller_t *pi, float kp, float ki)
{
    pi->kp = kp;
    pi->ki = ki;
}
