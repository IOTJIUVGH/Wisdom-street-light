#ifndef __AM2320_H
#define __AM2320_H

#include "stm32f10x.h"
#include "delay.h"
#include "i2c2.h"
#include "stdio.h"



#define IIC_Add 0xB8    //Æ÷¼þµØÖ·
#define IIC_RX_Length 15

void waken(void);
u8 WriteNByte(unsigned char sla,unsigned char *s,unsigned char n);
u8 ReadNByte(unsigned char Sal, unsigned char *p,unsigned char n);
unsigned int CRC16(unsigned char *ptr, unsigned char len);
unsigned char CheckCRC(unsigned char *ptr,unsigned char len);
void UARTSend_Nbyte(void);
void Clear_Data(void);









#endif
