#ifndef __I2C2_H
#define __I2C2_H

#include "stm32f10x.h"
#include "delay.h"

//PB13 -- I2C2_SCL    PB12 -- I2C2_SDA
//IO方向设置
#define SDA2_IN()  {GPIOB->CRH &= 0XFFF0FFFF; GPIOB->CRH |= 0X00080000;}
#define SDA2_OUT() {GPIOB->CRH &= 0XFFF0FFFF; GPIOB->CRH |= 0X00030000;}

//IO操作	 
#define IIC2_SCL(n)   {GPIOB->BSRR &= 0XDFFFDFFF; GPIOB->BSRR |= n;} //SCL
#define IIC2_SDA(n)   {GPIOB->BSRR &= 0XEFFFEFFF; GPIOB->BSRR |= n;} //SDA	 
#define READ_SDA2     (GPIOB->IDR & 0x1000) //输入SDA 

void init_i2c_2(void);
void IIC2_Start(void);
void IIC2_Stop(void);
u8 IIC2_Wait_Ack(void);
void IIC2_Ack(void);
void IIC2_NAck(void);
void IIC2_Send_Byte(u8 txd);
u8 IIC2_Read_Byte(u8 ack);
u8 IIC2_waken_Ack(void);








#endif
