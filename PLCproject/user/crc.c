#include "stm32f10x.h"


/********Ӳ��CRC*******/
void CRC_Init()
{
	RCC->AHBENR |=0x01<<6;//ʹ��CRCʱ��
	 CRC->CR = 0x01;//��λCRC���㵥Ԫ
}

u32 CRC_CalculateCRC(u32 data)
{
	CRC->CR = 0x01;
	CRC->DR = data;//д��Ҫ���ɵ�CRC����
	
	return CRC->DR;//����CRC
}

u32 CRC_CalculateBlockCRC(u8 pBuffer[], u32 BufferLength)
 {
	u32 i;
 
	CRC->CR = 0x01;
	for(i = 0;i < BufferLength;i++)
	CRC->DR = (u32)pBuffer[i];
 
	return CRC->DR;
 }
 
 /**********���CRC***********/
 
//MODBUS CRC�㷨 ��λ����������λ
 
u16  Get_Crc16(u8 *str,u16 len)
{
	char i;
	u16 crc=0xFFFF;
	
	while(len--)
	{
		crc = crc ^ *str;    //-----------------------------�������
		for(i = 0;i < 8; i++)
		{
			if(crc & 0x01)crc = (crc >>1)^0xa001;//------------�ж�Ҫ�Ƴ���λ�Ƿ�Ϊ1����Ϊ1����Ͷ���ʽ���������һλ
			else crc >>= 1;//---------------------------------����ֱ������һλ
		}
		str++;
	}
	return crc;
}
//CRC8
//ptr:Ҫ���������
//len:���鳤��
u8 Get_Crc8(u8 *ptr,u16 len)
{
	u8 crc;
	u8 i;
	crc=0;
	while(len--)
	{
		crc^=*ptr++;
		for(i=0;i<8;i++)
		{
			if(crc&0x01)crc=(crc>>1)^0x8C;
			else crc >>= 1;
		}
	}
	return crc;
}
/*XMODEM CRC�㷨����λ���������ĵ�λ*/

u16 Get_Crc(u8 *puchMsg,u16 usDataLen)
{
  u16 crc =0;
  u8 i;
  while(usDataLen--)
  {
    crc = ( crc^(((u16)*puchMsg)<<8));
    for(i=0;i<8;i++)
    {   
      if(crc&0x8000) crc= ((crc<<1)^0x1021);//1021
      else crc <<= 1;
    }
    puchMsg++;
  }
  return crc;
}
