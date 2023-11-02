/*****************************************************************************
* | File      	:   OLED_1in3_test.c
* | Author      :   Waveshare team
* | Function    :   1.3inch OLED test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-08-13
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
#include "OLED_1in3.h"

float GetMemUsage() {
    int FileHandler;
    char FileBuffer[1024];
    int memTotal, memFree, memAvail, memBuf,memCached;
    float result;
    FileHandler = open("/proc/meminfo", O_RDONLY);
    if(FileHandler < 0) {
        return -1; }
    read(FileHandler, FileBuffer, sizeof(FileBuffer) - 1);
    sscanf(FileBuffer, "MemTotal:         %d kB\n MemFree:          %d kB\n MemAvailable:          %d kB\n Buffers:           %d kB\n Cached:           %d kB",
     &memTotal, &memFree, &memAvail, &memBuf, &memCached);
    close(FileHandler);
    result = 1.0 - (float)(memFree + memCached) / memTotal;
    return result;
}

int GetCPUTemp() {
   int FileHandler;
   char FileBuffer[1024];
   int CPU_temp;
   FileHandler = open("/sys/devices/virtual/thermal/thermal_zone0/temp", O_RDONLY);
   if(FileHandler < 0) {
      return -1; }
   read(FileHandler, FileBuffer, sizeof(FileBuffer) - 1);
   sscanf(FileBuffer, "%d", &CPU_temp);
   close(FileHandler);
   return CPU_temp / 1000;
}

void showIP_wlan()
{
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if( (strcmp(ifa->ifa_name,"wlan0")==0)&&( ifa->ifa_addr->sa_family==AF_INET)  )
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            printf("\tInterface : <%s>\n",ifa->ifa_name );
            printf("\t  Address : <%s>\n", host);

			//Paint_DrawString_EN(3, 48, "IP:", &Font12, WHITE, WHITE);
			Paint_DrawString_EN(3, 48, host, &Font12, WHITE, WHITE);
        } 
		
		if (ifa->ifa_addr->sa_family == AF_INET &&
            strncmp(ifa->ifa_name, "wlP", 3) == 0) { // "wlP"로 시작하는 인터페이스 확인
            char host[NI_MAXHOST];
            int s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            
            //printf("\tInterface : <%s>\n", ifa->ifa_name);
            printf("\t  Address : <%s>\n", host);
			Paint_DrawString_EN(3, 48, host, &Font12, WHITE, WHITE);
        }
		
    }

    freeifaddrs(ifaddr);
}

void showIP_eth()
{
	uint8_t text_buffer[100];
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if( (strcmp(ifa->ifa_name,"eth0")==0)&&( ifa->ifa_addr->sa_family==AF_INET)  )
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            printf("\tInterface : <%s>\n",ifa->ifa_name );
            printf("\t  Address : <%s>\n", host);
			//Paint_DrawString_EN(3, 36, "IP:", &Font12, WHITE, WHITE);
			Paint_DrawString_EN(3, 36, host, &Font12, WHITE, WHITE);
        }
    }

    freeifaddrs(ifaddr);
}

int OLED_1in3_test(void)
{
	time_t mytime;
	struct tm *tm;
	uint8_t time_buffer[80];
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n, row;
	uint8_t text_buffer[100];
	char host[NI_MAXHOST];

	printf("[OPI5] 1.3inch OLED test demo\n");
	if(DEV_ModuleInit() != 0) {
		return -1;
	}
	  
	printf("OLED Init...\r\n");
	OLED_1IN3_Init();
	DEV_Delay_ms(500);	
	// 0.Create a new image cache
	UBYTE *BlackImage;
	UWORD Imagesize = ((OLED_1IN3_WIDTH%8==0)? (OLED_1IN3_WIDTH/8): (OLED_1IN3_WIDTH/8+1)) * OLED_1IN3_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
	printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_1IN3_WIDTH, OLED_1IN3_HEIGHT, 90, BLACK);	

	printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	DEV_Delay_ms(500);
	Paint_Clear(BLACK);

	Paint_SetMirroring(MIRROR_NONE);

	while(1) {
		OLED_1IN3_Display(BlackImage);
		DEV_Delay_ms(500);
		Paint_Clear(BLACK);

		// Show data and time
		mytime = time(NULL);
		tm = localtime (&mytime);
		strftime(time_buffer, 80,"%x %H:%M:%S ", tm);
		Paint_DrawString_EN(5, 0, time_buffer, &Font8, WHITE, WHITE);

		/* Memory usage */
		float m = GetMemUsage();
		snprintf ( text_buffer, sizeof(text_buffer), "Mem used: %3.0f%%", m*100 );
		printf("%s\n", text_buffer);
		Paint_DrawString_EN(3, 12, text_buffer, &Font12, WHITE, WHITE);

		/* CPU temperature  */
		int t = GetCPUTemp() ;
		snprintf ( text_buffer, sizeof(text_buffer), "CPU temp: %3d C", t );
		printf("%s\n", text_buffer);
		Paint_DrawString_EN(3, 24, text_buffer, &Font12, WHITE, WHITE);

		showIP_wlan();
	}

#if 0
	while(1) {
		
		// 2.Drawing on the image		
		printf("Drawing:page 1\r\n");
		Paint_DrawPoint(20, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
		Paint_DrawPoint(30, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
		Paint_DrawPoint(40, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
		Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(20, 20, 20, 30, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(30, 30, 30, 40, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawLine(40, 40, 40, 50, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawCircle(60, 30, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawCircle(100, 40, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);			
		Paint_DrawRectangle(50, 30, 60, 40, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawRectangle(90, 30, 110, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);		
		// 3.Show image on page1
		OLED_1IN3_Display(BlackImage);
		DEV_Delay_ms(2000);			
		Paint_Clear(BLACK);
		
		// Drawing on the image
		printf("Drawing:page 2\r\n");			
		Paint_DrawString_EN(10, 0, "waveshare", &Font16, WHITE, WHITE);
		Paint_DrawString_EN(10, 17, "hello world", &Font8, WHITE, WHITE);
		Paint_DrawNum(10, 30, 123.456789, &Font8, 4, WHITE, WHITE);
		Paint_DrawNum(10, 43, 987654, &Font12, 5, WHITE, WHITE);
		// Show image on page2
		OLED_1IN3_Display(BlackImage);
		DEV_Delay_ms(2000);	
		Paint_Clear(BLACK);		
		
		// Drawing on the image
		printf("Drawing:page 3\r\n");
		Paint_DrawString_CN(10, 0,"���Abc", &Font12CN, WHITE, WHITE);
		Paint_DrawString_CN(0, 20, "΢ѩ����", &Font24CN, WHITE, WHITE);
		// Show image on page3
		OLED_1IN3_Display(BlackImage);
		DEV_Delay_ms(2000);		
		Paint_Clear(BLACK);		

		// Drawing on the image
		printf("Drawing:page 4\r\n");
		GUI_ReadBmp("./pic/1in3.bmp", 0, 0);
		// Show image on page4
		OLED_1IN3_Display(BlackImage);
		DEV_Delay_ms(2000);		
		Paint_Clear(BLACK);	
	}
#endif	
}

