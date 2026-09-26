/*****************************************************************************
* | File        :   main.c
* | Function    :   Orange Pi 5 - 0.96inch I2C OLED status display + PWM fan
* | Info        :   based on https://github.com/andro78/OLED_MODULE_OPI5
******************************************************************************/
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "OLED_0in96.h"
#include "fan.h"
#include "sysinfo.h"

#define UPDATE_MS 1000

static volatile sig_atomic_t running = 1;

static void Handler(int signo)
{
    running = 0;
}

static void Draw(int pwm, double temp)
{
    char buf[32];
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    int bar;

    Paint_Clear(BLACK);

    strftime(buf, sizeof(buf), "%m/%d %H:%M:%S", tm);
    Paint_DrawString_EN(0, 0, buf, &Font12, WHITE, WHITE);

    if (SYS_GetIP(buf, sizeof(buf)) != 0)
        snprintf(buf, sizeof(buf), "No network");
    Paint_DrawString_EN(0, 13, buf, &Font12, WHITE, WHITE);

    snprintf(buf, sizeof(buf), "CPU%3d%% MEM%3d%%", SYS_GetCPUUsage(), SYS_GetMemUsage());
    Paint_DrawString_EN(0, 26, buf, &Font12, WHITE, WHITE);

    snprintf(buf, sizeof(buf), "TEMP %.1fC", temp);
    Paint_DrawString_EN(0, 39, buf, &Font12, WHITE, WHITE);

    snprintf(buf, sizeof(buf), "FAN%3d%%", pwm * 100 / 255);
    Paint_DrawString_EN(0, 52, buf, &Font12, WHITE, WHITE);
    // fan duty bar
    Paint_DrawRectangle(56, 53, 126, 63, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    bar = 56 + pwm * 70 / 255;
    if (bar > 57)
        Paint_DrawRectangle(57, 54, bar, 63, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
}

int main(int argc, char *argv[])
{
    UBYTE *BlackImage = NULL;
    int oled_ok = 0, fan_ok = 0;
    int pwm = 0;
    double temp;

    signal(SIGINT, Handler);
    signal(SIGTERM, Handler);
    setvbuf(stdout, NULL, _IOLBF, 0);

    if (Fan_Init() == 0) {
        fan_ok = 1;
        printf("Fan: control ready\n");
    } else {
        fprintf(stderr, "Fan: no I2C fan MCU or writable pwm-fan hwmon (run as root)\n");
    }

    if (DEV_ModuleInit(OLED_I2C_DEV, OLED_I2C_ADDR) == 0) {
        UWORD Imagesize = ((OLED_0in96_WIDTH % 8 == 0) ? (OLED_0in96_WIDTH / 8) : (OLED_0in96_WIDTH / 8 + 1)) * OLED_0in96_HEIGHT;
        if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
            fprintf(stderr, "Failed to apply for black memory...\n");
            return 1;
        }
        OLED_0in96_Init();
        Paint_NewImage(BlackImage, OLED_0in96_WIDTH, OLED_0in96_HEIGHT, 90, BLACK);
        Paint_SelectImage(BlackImage);
        Paint_Clear(BLACK);
        oled_ok = 1;
        printf("OLED: %s addr 0x%02X\n", OLED_I2C_DEV, OLED_I2C_ADDR);
    } else {
        fprintf(stderr, "OLED: init failed, running fan control only\n");
    }

    if (!oled_ok && !fan_ok)
        return 1;

    SYS_GetCPUUsage();   // prime the /proc/stat delta
    while (running) {
        temp = SYS_GetTemp();
        if (fan_ok) {
            int new_pwm = Fan_Update(temp);
            if (new_pwm != pwm)
                printf("Temp %.1fC -> fan pwm %d\n", temp, new_pwm);
            pwm = new_pwm;
        }
        if (oled_ok) {
            Draw(pwm, temp);
            OLED_0in96_display(BlackImage);
        }
        DEV_Delay_ms(UPDATE_MS);
    }

    printf("exit\n");
    if (oled_ok) {
        OLED_0in96_off();
        DEV_ModuleExit();
        free(BlackImage);
    }
    return 0;
}
