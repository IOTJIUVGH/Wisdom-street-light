#ifndef __24C32_H
#define __24C32_H

#include "stm32f10x.h"
#include "i2c.h"
#include "stdio.h"

extern u8 g_MACAddr[240],MACList[250];
extern u16 MAC_NumL;
void en_24c32(void);
void write_byte(u16 ADDRESS, u8 dat);
void Read_CurAddr(void) ;
u8 Read_RanAddr(u16 ADDRESS)  ;
void write_page(u16 ADDRESS, u8 *dat, u8 num);
void Read_Seq(u16 ADDRESS, u8 num);
void Storage_MACAddr(u8 *data,u16 len);
void Read_MACAddr(u16 addr,u16 len);

#endif
