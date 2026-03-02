/**
 * @file    ssd1306.h
 * @brief   SSD1306 OLED driver public interface
 */

#ifndef DRIVERS_SSD1306_H
#define DRIVERS_SSD1306_H

#include <stdint.h>

uint8_t SSD1306_Init(void);
void    SSD1306_Clear(void);
void    SSD1306_Flush(void);
void    SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void    SSD1306_PrintString(uint8_t x, uint8_t y, const char *str);

#endif /* DRIVERS_SSD1306_H */
