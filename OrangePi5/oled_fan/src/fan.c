/*****************************************************************************
* | File        :   fan.c
* | Function    :   Fan control for the HAT fan (I2C MCU at 0x0D on /dev/i2c-2)
*                   and the board's kernel pwm-fan header, if present
* | Info        :   MCU register 0x08 : 0x00 off, 0x01 full, 0x02-0x09 = 20-90%
*                   /sys/class/hwmon/hwmonN (name = "pwmfan") / pwm1 : 0-255
*                   Curve follows the board device tree:
*                   rockchip,temp-trips = 50 55 60 65 70 C
*                   cooling-levels      = 0 50 100 150 200 255
******************************************************************************/
#include "fan.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define FAN_I2C_DEV    "/dev/i2c-2"
#define FAN_I2C_ADDR   0x0D
#define FAN_REG        0x08

#define HYSTERESIS_C   3.0   // drop a level only when this far below its trip
#define KICK_PWM       255   // spin-up pulse when starting from a stop
#define KICK_MS        500

static const double trips[]  = {50, 55, 60, 65, 70};
static const int    levels[] = {0, 50, 100, 150, 200, 255};
#define NUM_TRIPS (int)(sizeof(trips) / sizeof(trips[0]))

static char pwm_path[300];
static int  i2c_fd    = -1;
static int  cur_level = -1;
static int  cur_pwm   = -1;

static int Fan_I2CInit(void)
{
    unsigned char reg = FAN_REG;

    i2c_fd = open(FAN_I2C_DEV, O_RDWR);
    if (i2c_fd < 0)
        return -1;
    // probe the MCU so a missing HAT is reported instead of silently ignored
    if (ioctl(i2c_fd, I2C_SLAVE, FAN_I2C_ADDR) < 0 || write(i2c_fd, &reg, 1) != 1) {
        close(i2c_fd);
        i2c_fd = -1;
        return -1;
    }
    return 0;
}

static int Fan_HwmonInit(void)
{
    DIR *d = opendir("/sys/class/hwmon");
    struct dirent *e;
    char path[300], name[32];

    if (!d)
        return -1;
    while ((e = readdir(d)) != NULL) {
        if (e->d_name[0] == '.')
            continue;
        snprintf(path, sizeof(path), "/sys/class/hwmon/%s/name", e->d_name);
        FILE *f = fopen(path, "r");
        if (!f)
            continue;
        if (fgets(name, sizeof(name), f) && strncmp(name, "pwmfan", 6) == 0) {
            snprintf(pwm_path, sizeof(pwm_path), "/sys/class/hwmon/%s/pwm1", e->d_name);
            fclose(f);
            closedir(d);
            if (access(pwm_path, W_OK) == 0)
                return 0;
            pwm_path[0] = '\0';
            return -1;
        }
        fclose(f);
    }
    closedir(d);
    return -1;
}

int Fan_Init(void)
{
    int i2c_ok   = Fan_I2CInit() == 0;
    int hwmon_ok = Fan_HwmonInit() == 0;

    printf("Fan: I2C MCU %s 0x%02X %s, pwm-fan hwmon %s\n", FAN_I2C_DEV, FAN_I2C_ADDR,
           i2c_ok ? "found" : "not found", hwmon_ok ? "found" : "not found");
    return (i2c_ok || hwmon_ok) ? 0 : -1;
}

// map a 0-255 duty to the MCU speed code
static unsigned char Fan_Code(int pwm)
{
    int pct;

    if (pwm <= 0)
        return 0x00;
    if (pwm >= 255)
        return 0x01;
    pct = (pwm * 10 + 127) / 255;   // nearest 10%
    if (pct < 2) pct = 2;
    if (pct > 9) pct = 9;
    return (unsigned char)pct;
}

int Fan_SetPWM(int pwm)
{
    int ok = 0;
    FILE *f;

    if (pwm < 0)   pwm = 0;
    if (pwm > 255) pwm = 255;

    if (i2c_fd >= 0) {
        unsigned char buf[2] = {FAN_REG, Fan_Code(pwm)};
        if (write(i2c_fd, buf, 2) == 2)
            ok = 1;
    }
    if (pwm_path[0] && (f = fopen(pwm_path, "w")) != NULL) {
        fprintf(f, "%d\n", pwm);
        if (fclose(f) == 0)
            ok = 1;
    }
    if (!ok)
        return -1;
    cur_pwm = pwm;
    return 0;
}

int Fan_GetPWM(void)
{
    return cur_pwm < 0 ? 0 : cur_pwm;
}

int Fan_Update(double temp_c)
{
    int level = cur_level < 0 ? 0 : cur_level;

    while (level < NUM_TRIPS && temp_c >= trips[level])
        level++;
    while (level > 0 && temp_c < trips[level - 1] - HYSTERESIS_C)
        level--;

    if (level != cur_level) {
        // small fans often won't start at a low duty, so give a short kick
        if (Fan_GetPWM() == 0 && levels[level] > 0 && levels[level] < KICK_PWM) {
            Fan_SetPWM(KICK_PWM);
            usleep(KICK_MS * 1000);
        }
        if (Fan_SetPWM(levels[level]) == 0)
            cur_level = level;
    }
    return Fan_GetPWM();
}
