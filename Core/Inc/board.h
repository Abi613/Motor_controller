/**
 * @file    board.h
 * @brief   Pin mapping and system-wide constants for STM32F103C8T6 (Blue Pill)
 */

#ifndef BOARD_H
#define BOARD_H

#include "stm32f1xx_hal.h"

/* System */
#define SYS_CLOCK_HZ            72000000UL
#define SCHEDULER_TICK_HZ       1000U

/* Digital inputs */
#define FAULT_IN_PIN            GPIO_PIN_0
#define FAULT_IN_PORT           GPIOA
#define MODE_SEL_PIN            GPIO_PIN_1
#define MODE_SEL_PORT           GPIOA
#define ESTOP_PIN               GPIO_PIN_2
#define ESTOP_PORT              GPIOA

/* Digital outputs */
#define LED_STATUS_PIN          GPIO_PIN_13
#define LED_STATUS_PORT         GPIOC
#define MOTOR_EN_PIN            GPIO_PIN_3
#define MOTOR_EN_PORT           GPIOA

/* PWM phases (TIM1) */
#define PHASE_U_H_PIN           GPIO_PIN_8
#define PHASE_U_H_PORT          GPIOA
#define PHASE_U_H_CHANNEL       TIM_CHANNEL_1

#define PHASE_U_L_PIN           GPIO_PIN_13
#define PHASE_U_L_PORT          GPIOB
#define PHASE_U_L_CHANNEL       TIM_CHANNEL_1

#define PHASE_V_H_PIN           GPIO_PIN_9
#define PHASE_V_H_PORT          GPIOA
#define PHASE_V_H_CHANNEL       TIM_CHANNEL_2

#define PHASE_V_L_PIN           GPIO_PIN_14
#define PHASE_V_L_PORT          GPIOB
#define PHASE_V_L_CHANNEL       TIM_CHANNEL_2

#define PHASE_W_H_PIN           GPIO_PIN_10
#define PHASE_W_H_PORT          GPIOA
#define PHASE_W_H_CHANNEL       TIM_CHANNEL_3

#define PHASE_W_L_PIN           GPIO_PIN_15
#define PHASE_W_L_PORT          GPIOB
#define PHASE_W_L_CHANNEL       TIM_CHANNEL_3

/* ADC */
#define ADC_CURRENT_PIN         GPIO_PIN_4
#define ADC_CURRENT_PORT        GPIOA
#define ADC_CURRENT_CHANNEL     ADC_CHANNEL_4
#define ADC_VBUS_PIN            GPIO_PIN_5
#define ADC_VBUS_PORT           GPIOA
#define ADC_VBUS_CHANNEL        ADC_CHANNEL_5

/* I2C */
#define I2C_PERIPH              I2C1
#define I2C_SCL_PIN             GPIO_PIN_6
#define I2C_SCL_PORT            GPIOB
#define I2C_SDA_PIN             GPIO_PIN_7
#define I2C_SDA_PORT            GPIOB

/* Peripheral handles (defined in main.c) */
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim1;

/* ADC constants */
#define ADC_RESOLUTION          4095U
#define ADC_VREF_MV             3300U
#define CURRENT_SENSE_GAIN      10.0f
#define CURRENT_SENSE_SHUNT_mR  10U

/* PWM constants */
#define PWM_FREQ_HZ             20000U
#define PWM_PERIOD_TICKS        3600U

/* Motor / control */
#define MOTOR_POLE_PAIRS        4U
#define MOTOR_MIN_DUTY          5U
#define MOTOR_MAX_DUTY          95U
#define CONTROL_LOOP_MS         1U
#define FAULT_CURRENT_LIMIT_A   10.0f

/* Display */
#define OLED_I2C_ADDR           0x3CU
#define OLED_WIDTH              128U
#define OLED_HEIGHT             64U
#define DISPLAY_UPDATE_MS       100U
#define FAULT_CHECK_MS          10U

/* BSP hooks */
uint32_t Board_GetTickMs(void);
void     Board_ScheduleOnceMs(uint32_t delay_ms, void (*cb)(void));

#endif /* BOARD_H */
