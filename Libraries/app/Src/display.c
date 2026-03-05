/**
 * @file    display.c
 * @brief   Display manager — maps system state to OLED screens
 * @details Calls SSD1306 primitives. Screen content depends on current mode.
 *          All rendering writes to the SSD1306 framebuffer then calls Flush.
 *
 *          Layout (128×64):
 *          Row 0:  Mode name + fault indicator
 *          Row 16: Speed: XXXX RPM
 *          Row 32: Current: X.XX A
 *          Row 48: Duty:  XX %
 */

#include "display.h"
#include "mode.h"
#include "motor.h"
#include "fault.h"
#include "ssd1306.h"
#include "pwm.h"
#include <stdio.h>

/*===========================================================================*/
/* PRIVATE VARIABLES                                                          */
/*===========================================================================*/
static char s_line_buf[32];    /**< Scratch buffer for snprintf formatting   */

/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Initialize display module and show splash/startup screen.
 */
void Display_Init(void)
{
    (void)SSD1306_Init();
    SSD1306_Clear();
    SSD1306_PrintString(0, 0, "Motor Ctrl v1.0");
    SSD1306_PrintString(0, 24, "Initializing...");
    SSD1306_Flush();
}

/**
 * @brief  Periodic display refresh. Routes to the appropriate screen.
 *         Called every DISPLAY_UPDATE_MS by the scheduler.
 */
void Display_Update(void)
{
    SSD1306_Clear();

    switch (Mode_Get())
    {
        case MOTOR_ALIGN_1:
        case MOTOR_ALIGN_2:
        case MOTOR_OPEN_LOOP:
        case MOTOR_CLOSED_LOOP:
            Display_ShowRunData();
            break;
        case MOTOR_FAULT:
            Display_ShowFault();
            break;
        case MOTOR_IDLE:
        default:
            Display_ShowIdle();
            break;
    }

    SSD1306_Flush();
}

/**
 * @brief  Render fault screen — mode + active fault codes.
 */
void Display_ShowFault(void)
{
    SSD1306_PrintString(0, 0,  "!! FAULT !!");
    snprintf(s_line_buf, sizeof(s_line_buf), "Code: 0x%02X", (unsigned)Fault_GetAll());
    SSD1306_PrintString(0, 16, s_line_buf);
    SSD1306_PrintString(0, 32, Fault_GetName(Fault_GetAll()));
    SSD1306_PrintString(0, 48, "Reset to clear");
}

/**
 * @brief  Render run-time telemetry: speed, current, duty cycle.
 */
void Display_ShowRunData(void)
{
    SSD1306_PrintString(0, 0, "MODE: RUN");

    snprintf(s_line_buf, sizeof(s_line_buf), "Spd: %.0f RPM", Motor_GetActualSpeed());
    SSD1306_PrintString(0, 16, s_line_buf);

    snprintf(s_line_buf, sizeof(s_line_buf), "I:  %.2f A", Motor_GetCurrent());
    SSD1306_PrintString(0, 32, s_line_buf);

    snprintf(s_line_buf, sizeof(s_line_buf), "Duty: %u%%", (unsigned)PWM_GetDuty());
    SSD1306_PrintString(0, 48, s_line_buf);
}

/**
 * @brief  Render idle screen — mode name and basic status.
 */
void Display_ShowIdle(void)
{
    SSD1306_PrintString(0, 0,  "MODE: IDLE");
    SSD1306_PrintString(0, 16, "Motor stopped");
    snprintf(s_line_buf, sizeof(s_line_buf), "Vbus: %.1fV", 0.0f);  /* TODO: ADC_GetVbus_V() */
    SSD1306_PrintString(0, 32, s_line_buf);
}
