#include "DS1307.h"


unsigned char rtc_address[8]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};
u8 g_Rtc_Dat[8]={0};

void IIC_init()
{
	RCC->APB2ENR |= (0X0001 << 3) | 0X0001;  //����GPIOBʱ�ӣ��͸�������
	RCC->APB1ENR |= 0X0001 << 21;            //����I2C1ʱ��
	RCC->APB1RSTR |= 1 << 21;
    RCC->APB1RSTR &= ~(1 << 21);
	
	GPIOB->CRL &= 0x00ffffff;
	GPIOB->CRL |= 0xFF000000;               //���ù����������
	
	I2C2->CR1 = 1 << 15;     //��λ
    I2C2->CR1 &= ~(1 << 15);  //ֹͣ��λ
	I2C1->CR1 &= 0XFFFD;   //I2Cģʽ
	I2C1->CR1 |= 0X0001;   //����I2Cģʽ
	
	I2C2->CR2 |= 30 << 0;//I2C2����ʱ��Ƶ������Ϊ30MHz
	I2C2->CCR |= 1 << 15;//���óɿ���ģʽ
	I2C2->CCR |= 1 << 14;//ռ�ձ�16/9
	I2C2->CCR |= 3 << 0;//ʱ�ӿ��Ʒ�Ƶϵ������Ϊ3(400KHz = 2.5us = (16+9)*CCR*Tpclk1)
	I2C2->TRISE |= 10 << 0;//������ģʽ�µ��������ʱ��(��׼ģʽ1000ns,����ģʽ300ns,����120ns)
	
	I2C1->CR1 |= 0X0001 << 10;  //ʹ��AckӦ��
}

void En_ds1307()
{
	RCC->APB2ENR |= (0X0001 << 3) | (0X0001 << 4);  //����GPIOB GPIOCʱ��
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
	
	I2C1->CR1 |= 0X0001 << 8;   //������ʼ�ź�
	while(!(I2C1->SR1 & 0X0001)); //��ʼ�ź��Ƿ񷢳�
	I2C1->DR  = temp;          //���SR1
}

void stop()
{
	
//	temp = I2C1->SR1;
//	I2C1->DR  = temp;    //���BTFλ
	while(!(I2C2->SR1 & (1 << 2)));
	
	I2C1->CR1 |= 0X0001 << 9;   //����stop�ź�
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
