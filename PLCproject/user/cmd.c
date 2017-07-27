#include "cmd.h"
#include "24c32.h"
#include "USART_data_handle.h"

u8 g_GetEdit_Flag = 0,NextFindFlag=0;

//void Get_MAC()
//{
//	u8 i;
//	u8 buf[6] = {0};
//	
//	for(i=0; i<6; i++)
//	{
//		buf[i] = g_CntrMAC[i];
//	}
//	
//	Pack_Data(buf, 6, 00);
//	g_URT1_RxLen = 0;
//	memset(g_URT1_RxBuf, 0, sizeof(g_URT1_RxBuf));
//}

//void Get_MACList(u8 First)
//{
//	u8 Get_MACLSIT[14] = {0XE8,0XE8,0XE8,0X17,0X20,0X03,0X04,0X00,0X02,0X17,0X00,0X00,0X40,0XE9};
//	u8 sum = 0;
//	static u8 num = 0;
//	u8 i;
//	
//	if(m_MACnum == 0)
//	{
//		g_URT2_TxFinish = 0;
//		for(i=0; i<14; i++)
//		{
//			g_URT2_TxBuf[i] = Get_MACLSIT[i];
//		}
//		g_URT2_TxBuf[10] = First;
//		g_URT2_TxBuf[12] += First;
//		g_URT2_TxLen = 14;
//		Uart2_PutChar(g_URT2_TxBuf[0]);
//		while(!(g_URT2_TxFinish == 1)){};
//	}
//	else m_MACnum = 0;
//	g_URT1_RxLen = 0;
//	memset(g_URT1_RxBuf, 0, sizeof(g_URT1_RxBuf));
//}

void En_GetEdit()
{
	g_SendEditFlag = 1;
	vtimer_SetTimer(VTIM0, 5000, En_GetEdit);
}

void Get_Edition()//集中控制器注册
{
	u8 i;
	static u8 FrameNum=0x30;
	u8 SendBuf[20] = {0x3c,0x3c,0x3c,0x5a,0x30,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x02,0x01,0x02};
	u16 crc;
	SendBuf[4]=FrameNum++;
	
  crc = Get_Crc16(SendBuf,17);
		SendBuf[17]=crc&0xff;
		SendBuf[18]=(crc>>8)&0xff;
		SendBuf[19]=0xed;
		
		for(i=0;i<20;i++)
		{
			Uart1_PutChar(SendBuf[i]);
		}
}

void En_SendDat()
{
	u16 second;
	
	second=Read_RanAddr(0x03fc);
	second=second<<8;
	second=Read_RanAddr(0x03fb);
	
	g_SendDatFlga = 1;
	vtimer_SetTimer(VTIM3, second*1000, En_SendDat);
}

void Poll_Heartdata(u8 dat)//轮训心跳数据
{
	u8 i,data_len,sum = 0,MACfirstbyte;
	u8 SendBuf[] = {0xE8,0xE8,0xE8,0x17,0x41,0x03,0x0D,0x00,0x0C,0x18,0x40,
					0x20,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0x37,0x05,0x19,
					0x55,0x39,0x11,0x22,0x17,0xE9};
	
	static u8 m_MACnum = 0;
	static u8 FSN = 0X60;
	static u8 count = 0;
	
	if(RemoveDeviceFlag==1)
	{
		m_MACnum = 0;
		count=0;
		RemoveDeviceFlag=0;
	}		
	if((m_MACnum-count) < Read_RanAddr(0x3fd))
	{		
		MACfirstbyte=Read_RanAddr(m_MACnum*6);
		
		if(MACfirstbyte!=0xff)
		{	
			//printf("AA");
			for(i=0; i<22; i++)
			{
				g_URT2_TxBuf[i] = SendBuf[i];
			}
			g_URT2_TxBuf[4] = FSN;
			for(i=13; i<19; i++)
			{
			//g_URT2_TxBuf[i+6] = g_MACList[i-13+(m_MACnum*6)];
				g_URT2_TxBuf[i+6] = Read_RanAddr(i-13+m_MACnum*6);
			}
			//printf("BB");
			data_len = 18;
			g_URT2_TxBuf[6] = data_len;

			g_URT2_TxBuf[25] = dat;
			for(i=4; i<(data_len+8); i++)
			{
				sum += g_URT2_TxBuf[i]; 
			}
			g_URT2_TxBuf[data_len+8] = sum;
			g_URT2_TxBuf[data_len+9] = 0XE9;
			g_URT2_TxLen = data_len + 10;
			for(i=0;i<g_URT2_TxLen;i++)
			{
				Uart2_PutChar(g_URT2_TxBuf[i]);
				Delayms(10);
			}
			//printf("CC");
			m_MACnum++;
			FSN++;
			if(FSN >= 0x6f) FSN = 0X60;
//	printf("%x,%x,\r\n",m_MACnum,g_MAC_num);
			if((m_MACnum-count) >= Read_RanAddr(0x3fd)) 
			{
				m_MACnum = 0;
				count=0;
				//printf("DD");
			}
		}else{
			m_MACnum++;
			count++;
			 NextFindFlag=1;

			//printf("EE");
		}
	}
}

//void Set_CurXS()
//{
//	u8 i;
//	u8 SendBuf[] = {0xE8,0xE8,0xE8,0x17,0x61,0x03,0x0D,0x00,0x0C,0x10,0x40,
//					0x00,0x01,0x37,0x05,0x19,0x55,0x39,0x11,0x02,0x00,0x00,0x17,0xE9};
//	u8 sum = 0;
//	u8 data_len;
//	
//	for(i=0; i<24; i++)
//	{
//		g_URT2_TxBuf[i] = SendBuf[i];
//	}
//	for(i=13; i<19; i++)
//	{
//		g_URT2_TxBuf[i] = g_URT1_RxBuf[i-5];
//	}
//	data_len = 14;
//	g_URT2_TxBuf[6] = data_len;

//	g_URT2_TxBuf[20] = g_URT1_RxBuf[16];
//	g_URT2_TxBuf[21] = g_URT1_RxBuf[17];
//	
//	for(i=4; i<(data_len+8); i++)
//	{
//		sum += g_URT2_TxBuf[i]; 
//	}
//	g_URT2_TxBuf[data_len+8] = sum;
//	g_URT2_TxBuf[data_len+9] = 0XE9;
//	
//	g_URT2_TxLen = data_len + 10;
//	Uart2_PutChar(g_URT2_TxBuf[0]);

//	g_URT1_RxLen = 0;
//	memset(g_URT1_RxBuf, 0, sizeof(g_URT1_RxBuf));
//}

//void Find_CurXS()
//{
//	u8 i;
//	u8 SendBuf[] = {0xE8,0xE8,0xE8,0x17,0x61,0x03,0x0D,0x00,0x0C,0x10,0x40,
//					0x00,0x01,0x37,0x05,0x19,0x55,0x39,0x11,0x03,0x17,0xE9};
//	u8 sum = 0;
//	u8 data_len;
//	
//	for(i=0; i<24; i++)
//	{
//		g_URT2_TxBuf[i] = SendBuf[i];
//	}
//	for(i=13; i<19; i++)
//	{
//		g_URT2_TxBuf[i] = g_URT1_RxBuf[i-5];
//	}
//	data_len = 12;
//	g_URT2_TxBuf[6] = data_len;
//	
//	for(i=4; i<(data_len+8); i++)
//	{
//		sum += g_URT2_TxBuf[i]; 
//	}
//	g_URT2_TxBuf[data_len+8] = sum;
//	g_URT2_TxBuf[data_len+9] = 0XE9;
//	
//	g_URT2_TxLen = data_len + 10;
//	Uart2_PutChar(g_URT2_TxBuf[0]);

//	g_URT1_RxLen = 0;
//	memset(g_URT1_RxBuf, 0, sizeof(g_URT1_RxBuf));
//}

void RegisterMasterDevice()//主节点注册
{
	u8 Lshoraddr,Hshoraddr;
	
	if(g_SendEditFlag == 1)
			{			
				Lshoraddr=Read_RanAddr(0x03fe);
				Hshoraddr=Read_RanAddr(0x03ff);
				//vtimer_SetTimer(VTIM0, 5000, En_GetEdit);			
				//printf("%x %x",Lshoraddr,Hshoraddr);
				if((Lshoraddr==0xff)&&(Hshoraddr==0xff))
				{
					Get_Edition();//注册集中控制器
				}else
				{
					vtimer_KillTimer(VTIM0);
				}
				g_SendEditFlag = 0;
			}	
}

void RotationEquipment()//轮训终端心跳
{
	u8 Lshoraddr,Hshoraddr;
	
	if(g_SendDatFlga == 1)
			{			
				Lshoraddr=Read_RanAddr(0x03fe);
				Hshoraddr=Read_RanAddr(0x03ff);
				if((Lshoraddr!=0xff)&&(Hshoraddr!=0xff))
				{
					//Read_Seq(0x00,32);
				//Read_MACAddr(0x00,240);
//				for(i=0;i<242;i++)
//				{
//					printf("%x ",MACList[i]);
//				}
				//Read_Seq(0x20,32);
					//Poll_Heartdata(0x01);
				}
				g_SendDatFlga = 0;
				if(NextFindFlag==1)
				{
					g_SendDatFlga = 1;
					vtimer_SetTimer(VTIM3, 10000, En_SendDat);
					NextFindFlag=0;
				}
			}
}

void Uart1RxDatahandle()
{
	u8 Lshoraddr,Hshoraddr;
	
	if(g_URT1_RxFlag == 1)
			{				
				Lshoraddr=Read_RanAddr(0x03fe);
				Hshoraddr=Read_RanAddr(0x03ff);
				
				if((Lshoraddr!=0xff)&&(Hshoraddr!=0xff))
				USART1_RxData();   //GPRS		
				
				if((g_URT1_RxBuf[15]==1)&&(g_URT1_RxBuf[16]==0)&&(g_URT1_RxBuf[17]==0))
					USART1_RxData();   //GPRS		
				
				g_URT1_RxFlag = 0;
			}
}

void Uart2RxDatahandle()
{
	u8 Lshoraddr,Hshoraddr;
	
	if(g_URT2_RxFlag == 1)
			{
				Lshoraddr=Read_RanAddr(0x03fe);
				Hshoraddr=Read_RanAddr(0x03ff);
				
				if((Lshoraddr!=0xff)&&(Hshoraddr!=0xff))
				USART2_RxData();   //主节点
				
				g_URT2_RxFlag = 0;			
			}

}
