#include "am2320.h"

unsigned char IIC_TX_Buffer[]={0x03,0x00,0x04}; //??????(?CRC??)
unsigned char IIC_RX_Buffer[IIC_RX_Length] = {0x00};//??????
unsigned char WR_Flag;


void waken(void)
{
	
	IIC2_Start();       // start I2C
	IIC2_Send_Byte(IIC_Add); // 发送器件地址

//	if(IIC2_Wait_Ack())
	if(IIC2_waken_Ack())
	{
		printf("---------");
	}	       
	Delayms(2);
	IIC2_Stop();
}

u8 WriteNByte(unsigned char sla,unsigned char *s,unsigned char n)
{
	unsigned char i;
   
	IIC2_Start();  
	IIC2_Send_Byte(sla);
	delay_us(2);
	if(!IIC2_Wait_Ack())
	{	
		WR_Flag = 1;
		return 0;
	}
	delay_us(15);
	for(i=0;i<n;i++)//写入8字节数据
	{
		IIC2_Send_Byte(*(s+i));
		if(!IIC2_Wait_Ack())
		{
			WR_Flag = 1;
			return(0);
		}
	}
	delay_us(5);
	IIC2_Stop();
	return 1;
}

u8 ReadNByte(unsigned char Sal, unsigned char *p,unsigned char n)
{
	unsigned char i;
	IIC2_Start();    
	IIC2_Send_Byte((Sal)| 0x01); 
	
	if(!IIC2_Wait_Ack())
	{
		WR_Flag = 1;
		return 0;
	}

	delay_us(30); 
        
	for(i=0;i<n-1;i++)  
	{
		*(p+i) = IIC2_Read_Byte(1); 
//    	IIC_SendACK(0); 
	}
	*(p+n-1) = IIC2_Read_Byte(0);        
//	IIC_SendACK(1)	;
	delay_us(5); 
	IIC2_Stop(); 
		
	return 1;	 
}
///计算CRC校验码
unsigned int CRC16(unsigned char *ptr, unsigned char len)
{
   unsigned int crc=0xffff;
   unsigned char i;
   while(len--)
   {
       crc ^=*ptr++;
       for(i=0;i<8;i++)
	   {
	       if(crc & 0x1)
		   {
		      crc>>=1;
			  crc^=0xa001;
		   }
		   else
		   {
		      crc>>=1;
		   }
	   }
   }
   return crc;
}
	//检测CRC校验码是否正确
unsigned char CheckCRC(unsigned char *ptr,unsigned char len)
{
	unsigned int crc;
	crc=(unsigned int)CRC16(ptr,len-2);
//	printf("\r\n %x",crc);
	if((ptr[len-1]==(crc>>8)) && (ptr[len-2]==(crc & 0x00ff)))
	{
	    return 0xff;
	}
	else
	{
	   return 0x0;
	}
}

void UARTSend_Nbyte() 
{
	//u8 i;
	double Tmp1,Tmp2;
	
	Clear_Data(); 
	WR_Flag = 0;
	waken();	  //唤醒
	Delayms(2);  
	//发送读指令
	WriteNByte(IIC_Add,IIC_TX_Buffer,3);//至少等待2MS
	Delayms(2);    
	ReadNByte(IIC_Add,IIC_RX_Buffer,8);
	Delayms(2);

	SDA2_OUT();
	IIC2_SDA(0x00001000);
	IIC2_SCL(0x00002000);	//确认释放总线
	
/***************************************************/
	
	if(WR_Flag == 0)
	{

		if(CheckCRC(IIC_RX_Buffer,8))
		{	
//			for(i=0; i<8; i++)
//			printf("%x\t",IIC_RX_Buffer[i]);
			Tmp1 = IIC_RX_Buffer[2]*256+IIC_RX_Buffer[3];	   	
			Tmp1=Tmp1/10;				 
			Tmp2 = IIC_RX_Buffer[4]*256+IIC_RX_Buffer[5];	  
			Tmp2=Tmp2/10;	

//			Tx_Buffer[2] = Tmp1;
//			Tx_Buffer[3] = Tmp2;
			printf("湿度:%.1f   温度:%.1f\n",Tmp1,Tmp2);
//			printf("湿度:%d   温度:%d\n",Tx_Buffer[2],Tx_Buffer[3]);
		}
		else
		{
			printf("Data: CRC Wrong\n"); 
		}
	}
	else
	{
		printf("Sensor Not Connected\n"); 
	}	    			  
} 

void Clear_Data()
{
	int i;
	for(i=0;i<IIC_RX_Length;i++)
	{
		IIC_RX_Buffer[i] = 0x00;
	}
}
