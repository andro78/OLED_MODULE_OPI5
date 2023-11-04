/*****************************************************************************
* | File      	:   OLED_0in91_test.c
* | Author      :   Waveshare team
* | Function    :   0.91inch OLED Module test demo
* | Info        :
*----------------
* |	This version:   V2.0
* | Date        :   2020-08-17
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <linux/if_link.h>

#include "test.h"
#include "OLED_0in91.h"
#include "dev_hardware_i2c.h"

//Set RGB effect，0-water light，1-breathing light,2-marquee，3-rainbow lights，4-colorful lights
void setRGBEffect(int effect)
{
	char wbuf[2];
    if (effect >= 0 && effect <= 4)
    {
		wbuf[0] = 0x04;
		wbuf[1] = effect;
        DEV_HARDWARE_I2C_write(wbuf, 2);
    }  
}
//Set RGB light switching speed of the mode. 1-low speed, 2-medium speed (default), 3-high speed
void setRGBSpeed(int speed)
{
	char wbuf[2];
    if (speed >= 1 && speed <= 3)
    {
		wbuf[0] = 0x05;
		wbuf[1] = speed;
        DEV_HARDWARE_I2C_write(wbuf, 2);
    }
}
//Set the color of the water light and breathing light, 
//0-red, 1-green (default), 2-blue, 3-yellow, 4-purple, 5-cyan, 6-white. 
void setRGBColor(int color)
{
	char wbuf[2];
    if (color >= 0 && color <= 6)
    {
		wbuf[0] = 0x06;
		wbuf[1] = color;
        DEV_HARDWARE_I2C_write(wbuf, 2);
    }
}

int OLED_0in91_test(void)
{
	time_t mytime;
	struct tm *tm;
	uint8_t time_buffer[80];

	uint8_t text_buffer[100];
	int state = 0;
	char wbuf[2];


	printf("0.91inch OLED test demo\n");
	if(DEV_ModuleInit() != 0) {
		return -1;
	}

	if(USE_SPI) {
		printf("Only USE_IIC, Please revise DEV_Config.h !!!\r\n");
		return -1;
	}
	  
	printf("OLED Init...\r\n");
	OLED_0in91_Init();
	DEV_Delay_ms(500);	
	OLED_0in91_Clear();

	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_0in91_WIDTH%8==0)? (OLED_0in91_WIDTH/8): (OLED_0in91_WIDTH/8+1)) * OLED_0in91_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_0in91_HEIGHT, OLED_0in91_WIDTH, 90, BLACK);	
	
	printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);

	DEV_HARDWARE_I2C_setSlaveAddress(0x0d);

	//Close RGB
	wbuf[0] = 0x07;
	wbuf[1] = 0x00;
	DEV_HARDWARE_I2C_write(wbuf, 2);

	DEV_Delay_ms(100);

	setRGBEffect(3);
	setRGBSpeed(1);
	
	while(1) {
		DEV_HARDWARE_I2C_setSlaveAddress(0x3c);

		// Show data and time
		mytime = time(NULL);
		tm = localtime (&mytime);
		strftime(time_buffer, 80,"%x %H:%M:%S ", tm);
		printf("%s\n", time_buffer);
		Paint_DrawString_EN(10, 0, time_buffer, &Font8, WHITE, WHITE);

		/* CPU temperature  */
		int t = GetCPUTemp() ;
		printf("GetCPUTemp %d\n", t);
		snprintf ( text_buffer, sizeof(text_buffer), "CPU temp: %3d C", t );
		//printf("%s\n", text_buffer);
		Paint_DrawString_EN(10, 12, text_buffer, &Font12, WHITE, WHITE);

		// Drawing on the image
		//printf("Drawing:page 2\r\n");			
		//Paint_DrawString_EN(10, 0, "waveshare", &Font16, WHITE, WHITE);
		//Paint_DrawNum(10, 18, 123.456789, &Font12, 4, WHITE, WHITE);
		// Show image on page2
		OLED_0in91_Display(BlackImage);
		DEV_Delay_ms(400);	
		Paint_Clear(BLACK);		
		
		DEV_HARDWARE_I2C_setSlaveAddress(0x0d);

		//Close RGB
		//wbuf[0] = 0x07;
		//wbuf[1] = 0x00;
		//DEV_HARDWARE_I2C_write(wbuf, 2);

		DEV_Delay_ms(100);

		//setRGBEffect(3);
    	//setRGBSpeed(1);
    	//setRGBColor(1);

		if (t >= 55)
		{
			wbuf[0] = 0x08;
			wbuf[1] = 0x01;
			DEV_HARDWARE_I2C_write(wbuf, 2);
			//setRGBColor(0);
		}
		else if (t <= 48)
		{
			wbuf[0] = 0x08;
			wbuf[1] = 0x00;
			DEV_HARDWARE_I2C_write(wbuf, 2);
			//setRGBColor(1);
		}
	
		DEV_Delay_ms(500);

	}
}

