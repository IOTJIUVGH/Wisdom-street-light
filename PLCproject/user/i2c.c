#include "i2c.h"

//PB7 -- I2C1_SCL    PB6 -- I2C1_SDA

void init_i2c1()
{
	RCC->APB2ENR|=1<<3;   //CLOCK GPIOB
	GPIOB->CRL &= 0X00FFFFFF;//PB6/7 Êä³ö
	GPIOB->CRL |= 0X77000000;	   
	GPIOB->ODR |= 3 << 6;     //PB6/7 ¸ß
}

//IIC????
void IIC_Start(void)
{
	SDA_OUT();     //sda???
	IIC_SDA(0x00000040);	
	delay_us(4);
	IIC_SCL(0x00000080);
	delay_us(10);
 	IIC_SDA(0x00400000);//START:when CLK is high,DATA change form high to low 
	delay_us(10);
	IIC_SCL(0x00800000);//??I2C??,?????????? 
}	  
//??IIC????
void IIC_Stop(void)
{
	SDA_OUT();//sda???
	IIC_SDA(0x00400000);//STOP:when CLK is high DATA change form low to high
	delay_us(10);
	IIC_SCL(0x00000080);
	
 	delay_us(10);
	IIC_SDA(0x00000040);//??I2C??????
	delay_us(10);							   	
}

u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	delay_us(1);
//	IIC_SDA(0x00000040);
	SDA_IN();      //SDA?????
	IIC_SCL(0x00800000);

	delay_us(10);	   
	IIC_SCL(0x00000080);
	delay_us(10);	
	IIC_SCL(0x00800000);//0 
	delay_us(5);
	
	if(READ_SDA == 0)
     ucErrTime = 1;
   else 
     ucErrTime = 0;
		   
   delay_us(10);	
	return ucErrTime;  
} 

u8 IIC_Wait_Ack_24c32(void)
{
	u8 ucErrTime=0;
	
	delay_us(1);
	IIC_SDA(0x00000040);
	SDA_IN();      //SDA?????
	IIC_SCL(0x00800000);

	delay_us(10);	   
	IIC_SCL(0x00000080);
	delay_us(10);	
	
	
	if(READ_SDA == 0)
     ucErrTime = 1;
   else 
     ucErrTime = 0;
   IIC_SCL(0x00800000);//0 
		   
   delay_us(10);	
	return ucErrTime;  
} 

u8 IIC_waken_Ack(void)
{
	IIC_SCL(0x00800000);
	IIC_SDA(0x00000040);delay_us(10);	   
	IIC_SCL(0x00000080);delay_us(10);	
	
	IIC_SCL(0x00800000);//0 	
	delay_us(10);
	IIC_SDA(0x00400000);
	return 0;  
}

//??ACK??
void IIC_Ack(void)
{
	IIC_SCL(0x00800000);
	delay_us(10);
	SDA_OUT();
	IIC_SDA(0x00400000);
	delay_us(10);
	IIC_SCL(0x00000080);
	delay_us(10);
	IIC_SCL(0x00800000);
}
//???ACK??		    
void IIC_NAck(void)
{
	IIC_SCL(0x00800000);
	SDA_OUT();
	IIC_SDA(0x00000040);
	delay_us(10);
	IIC_SCL(0x00000080);
	delay_us(10);
	IIC_SCL(0x00800000);
}					 				  

//IIC??????
//1,???
//0,???			  
void IIC_Send_Byte(u8 txd)
{                        
   u8 t, y;   
	 SDA_OUT(); 	    
   
    for(t=0;t<8;t++)
    {
		IIC_SCL(0x00800000);
		delay_us(10);
		y = (txd&0x80)>>7;
		if(y == 0) { IIC_SDA(0x00400000);}
		else		{IIC_SDA(0x00000040);}
		delay_us(10);
		IIC_SCL(0x00000080);
		delay_us(10); 
		txd<<=1; 	  
//		delay_us(2);  
    }	 
		
	IIC_SCL(0x00800000);
	delay_us(10);
} 	    
//?1???,ack=1,??ACK,ack=0,??nACK   
u8 IIC_Read_Byte(u8 ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA?????
	delay_us(10);
	for(i=0;i<8;i++ )
	{
		IIC_SCL(0x00800000);
		delay_us(10); 
		receive<<=1;
		IIC_SCL(0x00000080);
//		delay_us(5);
		if(READ_SDA == 0x0040)
			receive = receive | 0x01;  //???1
		else
			receive = receive & 0x0fe; //???0 
		delay_us(10);
	}					 
//		IIC_SCL(0x00800000);
		if(ack==0)
			IIC_NAck();
		else
			IIC_Ack();
    return receive;
}

