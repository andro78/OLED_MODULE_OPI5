# OLED_MODULE_OPI5
OLED module C source code for Orange Pi 5

IIC:
	OLED   =>    RPI(BCM)
	VCC    ->    3.3 or 5.0
	GND    ->    GND
	DIN    ->    2(SDA)
	CLK    ->    3(SCL)

You must run 'sudo orangepi-config'. and set 'i2c2-m0'.

![image](https://github.com/sagpaycokr/OLED_MODULE_OPI5/assets/70673576/4c550e7d-f450-4f38-a049-517b6a0c1533)


If you bought a 1.3-inch OLED Module (C), enter it in your home directory:
		sudo make clean
		sudo make
		sudo ./main 1.3
