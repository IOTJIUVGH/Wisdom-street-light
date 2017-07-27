#include "stm32f10x.h"


/********硬件CRC*******/
void CRC_Init()
{
	RCC->AHBENR |=0x01<<6;//使能CRC时钟
	 CRC->CR = 0x01;//复位CRC计算单元
}

u32 CRC_CalculateCRC(u32 data)
{
	CRC->CR = 0x01;
	CRC->DR = data;//写入要生成的CRC数据
	
	return CRC->DR;//返回CRC
}

u32 CRC_CalculateBlockCRC(u8 pBuffer[], u32 BufferLength)
 {
	u32 i;
 
	CRC->CR = 0x01;
	for(i = 0;i < BufferLength;i++)
	CRC->DR = (u32)pBuffer[i];
 
	return CRC->DR;
 }
 
 /**********软件CRC***********/
 
//MODBUS CRC算法 高位存放在数组高位
 
u16  Get_Crc16(u8 *str,u16 len)
{
	char i;
	u16 crc=0xFFFF;
	
	while(len--)
	{
		crc = crc ^ *str;    //-----------------------------异或运算
		for(i = 0;i < 8; i++)
		{
			if(crc & 0x01)crc = (crc >>1)^0xa001;//------------判断要移出的位是否为1，若为1，则和多项式异或，再右移一位
			else crc >>= 1;//---------------------------------否则，直接右移一位
		}
		str++;
	}
	return crc;
}
//CRC8
//ptr:要检验的数组
//len:数组长度
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
/*XMODEM CRC算法，高位存放在数组的低位*/

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
