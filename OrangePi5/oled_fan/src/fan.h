#ifndef _FAN_H_
#define _FAN_H_

int  Fan_Init(void);             // locate the pwm-fan hwmon node, 0 on success
int  Fan_Update(double temp_c);  // apply the temperature curve, returns duty 0-255
int  Fan_SetPWM(int pwm);        // write a raw duty 0-255
int  Fan_GetPWM(void);

#endif
