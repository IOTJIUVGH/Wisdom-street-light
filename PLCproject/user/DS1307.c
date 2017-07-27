#include "DS1307.h"


unsigned char rtc_address[8]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
u8 g_Rtc_Dat[8]={0};

void IIC_init()
{
	RCC->APB2ENR |= (0X0001 << 3) | 0X0001;  //开启GPIOB时钟，和辅助功能
	RCC->APB1ENR |= 0X0001 << 21;            //开启I2C1时钟
	RCC->APB1RSTR |= 1 << 21;
    RCC->APB1RSTR &= ~(1 << 21);
	
	GPIOB->CRL &= 0x00ffffff;
	GPIOB->CRL |= 0xFF000000;               //复用功能推挽输出
	
	I2C2->CR1 = 1 << 15;     //复位
    I2C2->CR1 &= ~(1 << 15);  //停止复位
	I2C1->CR1 &= 0XFFFD;   //I2C模式
	I2C1->CR1 |= 0X0001;   //启用I2C模式
	
	I2C2->CR2 |= 30 << 0;//I2C2输入时钟频率设置为30MHz
	I2C2->CCR |= 1 << 15;//设置成快速模式
	I2C2->CCR |= 1 << 14;//占空比16/9
	I2C2->CCR |= 3 << 0;//时钟控制分频系数设置为3(400KHz = 2.5us = (16+9)*CCR*Tpclk1)
	I2C2->TRISE |= 10 << 0;//设置主模式下的最大上升时间(标准模式1000ns,快速模式300ns,超快120ns)
	
	I2C1->CR1 |= 0X0001 << 10;  //使能Ack应答
}

void En_ds1307()
{
	RCC->APB2ENR |= (0X0001 << 3) | (0X0001 << 4);  //开启GPIOB GPIOC时钟
	GPIOB->CRL &= 0xFF0FFFFF;  //WP  24C64
	GPIOB->CRL |= 0x00300000;
	GPIOB->BSRR |= 0X00080000;
	
	GPIOC->CRH &= 0xFFfff0ff;   //SWQ  DS1307
	GPIOC->CRH |= 0x00000800;
//	GPIOC->BSRR |= 0X00000400;
}

void start()
{
	u8 temp = 0;
	
	I2C1->CR1 |= 0X0001 << 8;   //产生起始信号
	while(!(I2C1->SR1 & 0X0001)); //起始信号是否发出
	I2C1->DR  = temp;          //清除SR1
}

void stop()
{
	
//	temp = I2C1->SR1;
//	I2C1->DR  = temp;    //清除BTF位
	while(!(I2C2->SR1 & (1 << 2)));
	
	I2C1->CR1 |= 0X0001 << 9;   //产生stop信号
}

void write_dat(u8 ADDRESS, u8 dat)
{
	
	IIC_Start();
	IIC_Send_Byte(0xd0);
	while((IIC_Wait_Ack()));
	IIC_Send_Byte(ADDRESS);
	while((IIC_Wait_Ack()));
	
	IIC_Send_Byte(dat);
	while((IIC_Wait_Ack()));

	IIC_Stop();

}

void Read_RTC(char START_ADDR) 
{
	u8 i;
	
	IIC_Start();

	IIC_Send_Byte(0xd0);
	while((IIC_Wait_Ack()));

	IIC_Send_Byte(START_ADDR);
	while((IIC_Wait_Ack()));
	IIC_Stop();

	IIC_Start();
	IIC_Send_Byte(0xd1);
	while(!(IIC_Wait_Ack()));

	for(i=0; i<=7; i++)
	{
		g_Rtc_Dat[i] = IIC_Read_Byte(1);
//		if(i>=7)
//			g_Rtc_Dat[i] = IIC_Read_Byte(0);
	}

	IIC_Stop();
}

void Set_RTC(unsigned char *p)
{
	u8 i;
  
	for(i=0;i<=7;i++,p++)
	{
	  write_dat(0X00+i, *p);
	}
}
