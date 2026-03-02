/**
 * @file    display.h
 * @brief   Display application layer public interface
 */

#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

void Display_Init(void);
void Display_Update(void);
void Display_ShowFault(void);
void Display_ShowRunData(void);
void Display_ShowIdle(void);

#endif /* APP_DISPLAY_H */
