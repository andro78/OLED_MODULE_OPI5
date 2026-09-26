/*****************************************************************************
* | File        :   DEV_Config.h
* | Function    :   Orange Pi 5 hardware interface (I2C only)
* | Info        :   Based on Waveshare DEV_Config, trimmed for an I2C SSD1306.
*                   The I2C module has no RST/DC/CS lines, so the GPIO macros
*                   are no-ops (the original exported BCM pin numbers, which
*                   map to unrelated pins on RK3588).
******************************************************************************/
#ifndef _DEV_CONFIG_H_
#define _DEV_CONFIG_H_

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "Debug.h"

#define USE_SPI 0
#define USE_IIC 1
#define IIC_CMD        0X00
#define IIC_RAM        0X40

/**
 * data
**/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

// I2C bus / address (i2c2-m0 : pin 3 SDA, pin 5 SCL)
#define OLED_I2C_DEV    "/dev/i2c-2"
#define OLED_I2C_ADDR   0x3C

#define OLED_CS_0
#define OLED_CS_1
#define OLED_RST_0
#define OLED_RST_1
#define OLED_DC_0
#define OLED_DC_1

UBYTE DEV_ModuleInit(const char *i2c_dev, UBYTE addr);
void  DEV_ModuleExit(void);
void  DEV_Delay_ms(UDOUBLE xms);

void  I2C_Write_Byte(uint8_t value, uint8_t Cmd);
int   I2C_Write_nByte(uint8_t Cmd, const uint8_t *pData, uint32_t Len);

#endif
