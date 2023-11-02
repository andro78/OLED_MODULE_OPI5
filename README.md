# OLED_MODULE_OPI5
OLED module C source code for Orange Pi 5

IIC:
	OLED   =>    RPI(BCM)
	VCC    ->    3.3 or 5.0
	GND    ->    GND
	DIN    ->    2(SDA)
	CLK    ->    3(SCL)

You must run 'sudo orangepi-config'. and set 'i2c2-m0'.

$ sudo i2cdetect -y 2
[sudo] password for orangepi: 
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:                         -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- 3c -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                        


If you bought a 1.3-inch OLED Module (C), enter it in your home directory:
		sudo make clean
		sudo make
		sudo ./main 1.3
