#ifndef __DS1307_H
#define __DS1307_H

#include "stm32f10x.h"
#include "stdio.h"
#include "i2c.h"

void IIC_init(void);
void start(void);
void stop(void);
void write_dat(u8 ADDRESS, u8 dat);
u8 read_dat(char START_ADDR);
void En_ds1307(void);
void Read_RTC(char START_ADDR);
void Set_RTC(unsigned char *p);

extern u8 g_Rtc_Dat[8];

#endif
