#include "i2c2.h"
#include "stdio.h"

//PB13 -- I2C2_SCL    PB12 -- I2C2_SDA

void init_i2c_2(void)
{
	RCC->APB2ENR |= 1<<3;   //CLOCK GPIOB
	GPIOB->CRH &= 0XFF00FFFF;//PB13/12 ���
	GPIOB->CRH |= 0X00330000;	   
	GPIOB->ODR |= 3 << 12;     //PB13/12 �����
}

//����IIC��ʼ�ź�
void IIC2_Start(void)
{
	SDA2_OUT();     //sda�����
	IIC2_SDA(0x00001000);	  	  
	IIC2_SCL(0x00002000);
	delay_us(5);
 	IIC2_SDA(0x10000000);//START:when CLK is high,DATA change form high to low 
	delay_us(5);
	IIC2_SCL(0x20000000);//ǯסI2C����,׼�����ͻ��߽������� 
}	  
//����IICֹͣ�ź�
void IIC2_Stop(void)
{
	SDA2_OUT();//sda�����
	IIC2_SDA(0x10000000);//STOP:when CLK is high DATA change form low to high
	delay_us(2);
	IIC2_SCL(0x00002000);
	
 	delay_us(4);
	IIC2_SDA(0x00001000);//����I2C���߽����ź�
	delay_us(4);							   	
}

u8 IIC2_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	delay_us(2);
	IIC2_SDA(0x00001000);
	SDA2_IN();      //SDA����Ϊ����
	IIC2_SCL(0x20000000);

	delay_us(2);	   
	IIC2_SCL(0x00002000);
	delay_us(5);	
	
	if(READ_SDA2 == 0)
		ucErrTime = 1;
	else 
		ucErrTime = 0;
	IIC2_SCL(0x20000000);//0 	
	return ucErrTime;  
} 

u8 IIC2_waken_Ack(void)
{
	IIC2_SCL(0x20000000);
	IIC2_SDA(0x00001000);delay_us(5);	   
	IIC2_SCL(0x00002000);delay_us(5);	
	
	IIC2_SCL(0x20000000);//0 	
	delay_us(2);
	IIC2_SDA(0x10000000);
	return 0;  
}

//����ACKӦ��
void IIC2_Ack(void)
{
	IIC2_SCL(0x20000000);
	SDA2_OUT();
	IIC2_SDA(0x10000000);
	delay_us(2);
	IIC2_SCL(0x00002000);
	delay_us(5);
	IIC2_SCL(0x20000000);
}
//������ACKӦ��		    
void IIC2_NAck(void)
{
	IIC2_SCL(0x20000000);
	SDA2_OUT();
	IIC2_SDA(0x00001000);
	delay_us(2);
	IIC2_SCL(0x00002000);
	delay_us(5);
	IIC2_SCL(0x20000000);
}					 				  

//IIC����һ���ֽ�
//1,��Ӧ��
//0,��Ӧ��			  
void IIC2_Send_Byte(u8 txd)
{                        
	u8 t, y;   
	SDA2_OUT(); 	    
   
    for(t=0;t<8;t++)
    {
		IIC2_SCL(0x20000000);//����ʱ�ӿ�ʼ���ݴ���
		delay_us(2);
		y = (txd & 0x80) >> 7;
		if(y == 0) { IIC2_SDA(0x10000000);}
		else		{IIC2_SDA(0x00001000);}
		delay_us(2);
		IIC2_SCL(0x00002000);
		delay_us(5); 
		txd<<=1; 	  
//		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
			
    }	 
		
	IIC2_SCL(0x20000000);
	delay_us(2);
//	IIC_RecvACK();
} 	    
//��1���ֽ�,ack=1,����ACK,ack=0,����nACK   
u8 IIC2_Read_Byte(u8 ack)
{
	unsigned char i,receive=0;
	SDA2_IN();//SDA����Ϊ����

	for(i=0;i<8;i++ )
	{
		IIC2_SCL(0x20000000);
		delay_us(5); 
		receive<<=1;
		IIC2_SCL(0x00002000);
//		delay_us(5);
		if(READ_SDA2 == 0x1000)
			receive = receive | 0x01;  //???1
		else
			receive = receive & 0x0fe; //???0 
		delay_us(1);
  }					 
//		IIC_SCL(0x00400000);
		if(ack==0)
			IIC2_NAck();
		else
			IIC2_Ack();

		return receive;
}

