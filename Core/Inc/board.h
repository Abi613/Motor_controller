/**
 * @file    board.h
 * @brief   Pin mapping and system-wide constants for STM32F103C8T6 (Blue Pill)
 * @project Motor Controller Firmware
 * @version 1.0.0
 * @date    2024
 *
 * @note    All pin definitions reference STM32CubeIDE HAL naming conventions.
 *          Update this file ONLY when hardware layout changes.
 */

#ifndef BOARD_H
#define BOARD_H

#include "stm32f1xx_hal.h"

/*===========================================================================*/
/* SYSTEM CLOCK                                                               */
/*===========================================================================*/
#define SYS_CLOCK_HZ            72000000UL   /**< System core clock: 72 MHz  */
#define SCHEDULER_TICK_HZ       1000U        /**< Scheduler tick rate: 1 kHz  */

/*===========================================================================*/
/* GPIO PIN MAPPING                                                           */
/*===========================================================================*/

/* --- Digital Inputs --- */
#define FAULT_IN_PIN            GPIO_PIN_0
#define FAULT_IN_PORT           GPIOA
#define MODE_SEL_PIN            GPIO_PIN_1
#define MODE_SEL_PORT           GPIOA
#define ESTOP_PIN               GPIO_PIN_2
#define ESTOP_PORT              GPIOA

/* --- Digital Outputs --- */
#define LED_STATUS_PIN          GPIO_PIN_13  /**< Onboard LED (active LOW)   */
#define LED_STATUS_PORT         GPIOC
#define MOTOR_EN_PIN            GPIO_PIN_3
#define MOTOR_EN_PORT           GPIOA

/* --- phase_U --- */
#define PHASE_U_H_PIN             GPIO_PIN_8
#define PHASE_U_H_PORT            GPIOA
#define PHASE_U_H_CHANNEL         TIM_CHANNEL_1

#define PHASE_U_L_PIN             GPIO_PIN_13
#define PHASE_U_L_PORT            GPIOB
#define PHASE_U_L_CHANNEL         TIM_CHANNEL_1

/* --- Phase_V --- */
#define PHASE_V_H_PIN             GPIO_PIN_9
#define PHASE_V_H_PORT            GPIOA
#define PHASE_V_H_CHANNEL         TIM_CHANNEL_2

#define PHASE_V_L_PIN             GPIO_PIN_14
#define PHASE_V_L_PORT            GPIOB
#define PHASE_V_L_CHANNEL         TIM_CHANNEL_2

/* --- Phase_W --- */
#define PHASE_W_H_PIN             GPIO_PIN_10
#define PHASE_W_H_PORT            GPIOA
#define PHASE_W_H_CHANNEL         TIM_CHANNEL_3

#define PHASE_W_L_PIN             GPIO_PIN_15
#define PHASE_W_L_PORT            GPIOB
#define PHASE_W_L_CHANNEL         TIM_CHANNEL_3

/* --- PWM Timer --- */
#define PWM_TIMER               TIM1

/* --- ADC Inputs --- */
#define ADC_CURRENT_PIN         GPIO_PIN_4   /**< PA4 - Motor current sense  */
#define ADC_CURRENT_PORT        GPIOA
#define ADC_CURRENT_CHANNEL     ADC_CHANNEL_4
#define ADC_VBUS_PIN            GPIO_PIN_5   /**< PA5 - Bus voltage sense     */
#define ADC_VBUS_PORT           GPIOA
#define ADC_VBUS_CHANNEL        ADC_CHANNEL_5

/* --- I2C (for SSD1306 OLED) --- */
#define I2C_PERIPH              I2C1
#define I2C_SCL_PIN             GPIO_PIN_6   /**< PB6 - I2C1 SCL              */
#define I2C_SCL_PORT            GPIOB
#define I2C_SDA_PIN             GPIO_PIN_7   /**< PB7 - I2C1 SDA              */
#define I2C_SDA_PORT            GPIOB

/*===========================================================================*/
/* PERIPHERAL HANDLES (defined in main.c, extern'd here)                     */
/*===========================================================================*/
extern ADC_HandleTypeDef  hadc1;
extern I2C_HandleTypeDef  hi2c1;
extern TIM_HandleTypeDef  htim1;

/*===========================================================================*/
/* APPLICATION CONSTANTS                                                      */
/*===========================================================================*/

/* ADC */
#define ADC_RESOLUTION          4095U        /**< 12-bit ADC full scale       */
#define ADC_VREF_MV             3300U        /**< ADC reference voltage (mV)  */
#define CURRENT_SENSE_GAIN      10.0f        /**< Amplifier gain on I-sense   */
#define CURRENT_SENSE_SHUNT_mR  10U          /**< Shunt resistor value (mΩ)   */

/* PWM */
#define PWM_FREQ_HZ             20000U       /**< PWM carrier frequency       */
#define PWM_PERIOD_TICKS        3600U        /**< ARR value (72MHz/20kHz=3600)*/

/* Motor limits */
#define MOTOR_MAX_DUTY          95U          /**< Max duty cycle (%)          */
#define MOTOR_MIN_DUTY          0U           /**< Min duty cycle (%)          */
#define FAULT_CURRENT_LIMIT_A   10.0f        /**< Overcurrent trip threshold  */

/* SSD1306 */
#define OLED_I2C_ADDR           0x3CU        /**< SSD1306 default I2C address */
#define OLED_WIDTH              128U
#define OLED_HEIGHT             64U

/* Timing intervals (in scheduler ticks = ms) */
#define DISPLAY_UPDATE_MS       100U
#define FAULT_CHECK_MS          10U
#define CONTROL_LOOP_MS         1U

#endif /* BOARD_H */
