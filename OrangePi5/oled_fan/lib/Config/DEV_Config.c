/*****************************************************************************
* | File        :   DEV_Config.c
* | Function    :   Orange Pi 5 hardware interface (I2C only, /dev/i2c-*)
******************************************************************************/
#include "DEV_Config.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

static int i2c_fd = -1;

UBYTE DEV_ModuleInit(const char *i2c_dev, UBYTE addr)
{
    if ((i2c_fd = open(i2c_dev, O_RDWR)) < 0) {
        fprintf(stderr, "Failed to open %s: %s\n", i2c_dev, strerror(errno));
        return 1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, addr) < 0) {
        fprintf(stderr, "Failed to set I2C address 0x%02x: %s\n", addr, strerror(errno));
        close(i2c_fd);
        i2c_fd = -1;
        return 1;
    }
    return 0;
}

void DEV_ModuleExit(void)
{
    if (i2c_fd >= 0) {
        close(i2c_fd);
        i2c_fd = -1;
    }
}

void DEV_Delay_ms(UDOUBLE xms)
{
    usleep(xms * 1000);
}

void I2C_Write_Byte(uint8_t value, uint8_t Cmd)
{
    uint8_t wbuf[2] = {Cmd, value};
    if (write(i2c_fd, wbuf, 2) != 2)
        Debug("I2C write failed: %s\n", strerror(errno));
}

/* Control byte followed by Len bytes in a single I2C transaction */
int I2C_Write_nByte(uint8_t Cmd, const uint8_t *pData, uint32_t Len)
{
    uint8_t wbuf[1 + 128];
    if (Len > 128)
        return -1;
    wbuf[0] = Cmd;
    memcpy(&wbuf[1], pData, Len);
    if (write(i2c_fd, wbuf, Len + 1) != (ssize_t)(Len + 1)) {
        Debug("I2C write failed: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
