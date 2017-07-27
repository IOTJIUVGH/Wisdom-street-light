#ifndef __I2C_H
#define __I2C_H

#include "stm32f10x.h"
#include "delay.h"

//PB7 -- I2C1_SCL    PB6 -- I2C1_SDA
//IO方向设置
#define SDA_IN()  {GPIOB->CRL &= 0XF0FFFFFF; GPIOB->CRL |= 0X08000000;}
#define SDA_OUT() {GPIOB->CRL &= 0XF0FFFFFF; GPIOB->CRL |= 0X07000000;}

//IO操作	 
#define IIC_SCL(n)   {GPIOB->BSRR &= 0Xff7fff7f; GPIOB->BSRR |= n;} //SCL
#define IIC_SDA(n)   {GPIOB->BSRR &= 0XffBfffBf; GPIOB->BSRR |= n;} //SDA	 
#define READ_SDA     (GPIOB->IDR & 0x0040) //输入SDA 

void init_i2c1(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(u8 txd);
u8 IIC_Read_Byte(u8 ack);
u8 IIC_waken_Ack(void);
u8 IIC_Wait_Ack_24c32(void);







#endif
