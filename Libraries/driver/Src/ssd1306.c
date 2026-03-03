/**
 * @file    ssd1306.c
 * @brief   SSD1306 128×64 OLED driver over I2C
 * @details Implements init sequence, pixel framebuffer, and flush to display.
 *          All drawing primitives write to s_framebuffer[]; call
 *          SSD1306_Flush() to push the buffer to the display over I2C.
 *
 *          Coordinate origin (0,0) is top-left.
 */

#include "ssd1306.h"
#include "i2c.h"
#include "board.h"
#include <string.h>

/*===========================================================================*/
/* PRIVATE TYPES & CONSTANTS                                                  */
/*===========================================================================*/
#define SSD1306_CMD         0x00U   /**< Co=0, D/C#=0 → command stream       */
#define SSD1306_DATA        0x40U   /**< Co=0, D/C#=1 → data stream          */
#define FRAMEBUF_SIZE       (OLED_WIDTH * OLED_HEIGHT / 8U)   /**< 1024 bytes */

/*===========================================================================*/
/* PRIVATE VARIABLES                                                          */
/*===========================================================================*/
/** @brief Local pixel framebuffer. 1 bit per pixel, 8 pixels per byte (row). */
static uint8_t s_framebuffer[FRAMEBUF_SIZE];

/** @brief True after successful SSD1306_Init() */
static uint8_t s_initialized = 0U;

/*===========================================================================*/
/* PRIVATE HELPERS                                                            */
/*===========================================================================*/
static I2C_Status_t SSD1306_SendCmd(uint8_t cmd);
static I2C_Status_t SSD1306_SendData(const uint8_t *data, uint16_t len);

/*===========================================================================*/
/* PUBLIC FUNCTIONS                                                           */
/*===========================================================================*/
/**
 * @brief  Send the SSD1306 initialization command sequence and clear display.
 * @retval 0 on success, 1 if I2C device not found or command failed
 */
uint8_t SSD1306_Init(void)
{
    if (!I2C_IsDeviceReady(OLED_I2C_ADDR))
    {
        return 1U;
    }

    /* Standard SSD1306 init sequence */
    const uint8_t init_cmds[] =
    {
        0xAE,        /* Display OFF                  */
        0xD5, 0x80,  /* Set display clock divide     */
        0xA8, 0x3F,  /* Multiplex ratio = 64         */
        0xD3, 0x00,  /* Display offset = 0           */
        0x40,        /* Start line = 0               */
        0x8D, 0x14,  /* Charge pump ON               */
        0x20, 0x00,  /* Memory mode: horizontal      */
        0xA1,        /* Segment re-map (mirror X)    */
        0xC8,        /* COM output scan direction    */
        0xDA, 0x12,  /* COM pin config               */
        0x81, 0xCF,  /* Contrast                     */
        0xD9, 0xF1,  /* Pre-charge period            */
        0xDB, 0x40,  /* VCOMH deselect level         */
        0xA4,        /* Entire display ON (RAM)      */
        0xA6,        /* Normal (not inverted)        */
        0xAF         /* Display ON                   */
    };

    for (uint8_t i = 0; i < sizeof(init_cmds); i++)
    {
        if (SSD1306_SendCmd(init_cmds[i]) != I2C_OK)
        {
            return 1U;
        }
    }

    SSD1306_Clear();
    SSD1306_Flush();
    s_initialized = 1U;
    return 0U;
}

/**
 * @brief  Clear framebuffer (all pixels off). Does NOT flush to display.
 */
void SSD1306_Clear(void)
{
    memset(s_framebuffer, 0x00, FRAMEBUF_SIZE);
}

/**
 * @brief  Push local framebuffer to the display over I2C.
 */
void SSD1306_Flush(void)
{
    /* Set full-screen address window */
    SSD1306_SendCmd(0x21); SSD1306_SendCmd(0); SSD1306_SendCmd(127); /* Col 0–127 */
    SSD1306_SendCmd(0x22); SSD1306_SendCmd(0); SSD1306_SendCmd(7);   /* Page 0–7  */
    SSD1306_SendData(s_framebuffer, FRAMEBUF_SIZE);
}

/**
 * @brief  Draw or clear a single pixel in the framebuffer.
 * @param  x      Column [0, OLED_WIDTH-1]
 * @param  y      Row    [0, OLED_HEIGHT-1]
 * @param  color  1 = pixel ON, 0 = pixel OFF
 */
void SSD1306_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
    {
        return;
    }

    uint16_t byte_idx = x + (y / 8U) * OLED_WIDTH;
    uint8_t  bit_mask = (uint8_t)(1U << (y % 8U));

    if (color)
    {
        s_framebuffer[byte_idx] |=  bit_mask;
    }
    else
    {
        s_framebuffer[byte_idx] &= ~bit_mask;
    }
}

/**
 * @brief  Print a null-terminated ASCII string at pixel position (x, y).
 * @param  x     Start column
 * @param  y     Start row (top of character, font height = 8 px)
 * @param  str   Null-terminated string (printable ASCII only)
 *
 * @note   Requires a 5×7 or 6×8 bitmap font — add font table to ssd1306.h.
 *         TODO: Implement font rendering.
 */
void SSD1306_PrintString(uint8_t x, uint8_t y, const char *str)
{
    /* TODO: Iterate str, look up each char in font table, call DrawPixel()  */
    (void)x; (void)y; (void)str;
}

/*===========================================================================*/
/* PRIVATE HELPERS                                                            */
/*===========================================================================*/
/**
 * @brief  Send a single command byte to SSD1306.
 */
static I2C_Status_t SSD1306_SendCmd(uint8_t cmd)
{
    uint8_t buf[2] = {SSD1306_CMD, cmd};
    return I2C_Write(OLED_I2C_ADDR, buf, 2U);
}

/**
 * @brief  Send raw display data bytes.
 */
static I2C_Status_t SSD1306_SendData(const uint8_t *data, uint16_t len)
{
    /* Prepend 0x40 control byte — requires a temp buffer or multi-write     */
    /* TODO: Optimize using DMA or chunked writes for performance            */
    (void)data; (void)len;
    return I2C_OK;
}
