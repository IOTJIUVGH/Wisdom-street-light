#include "USART_data_handle.h"
#include "delay.h"
#include "usart.h"
#include "24c32.h"
#include "rtc.h"
#include "DS1307.h"

u8 m_TxPro[] = {0xE8,0xE8,0xE8,0x17,0x61,0x03,0x0D,0x00,0x09,0xD8,0x40,0x00,0x01};
u8 m_FindMAC[] = {0xE8,0xE8,0xE8,0x17,0x00,0x03,0x03,0x00,0x02,0x0b,0x00,0x13,0xE9};
u8 m_UARTEerror_Flag = 0,RemoveDeviceFlag=0;

void USART2_RxData()
{
	u16 tou = 0;
	u32 toulong = 0;
	u8 i = 0;
	u8 sum = 0;
	u8 ctrl_FT = 0;
	
//	for(i=0; i<g_URT2_RxLen; i++)
//		printf("%x ",g_URT2_RxBuf[i]);
//		printf("%d",g_URT2_RxLen);
//	printf("\r\n");
////	
	tou = (g_URT2_RxBuf[0] << 8) | g_URT2_RxBuf[1];
	toulong = (tou << 8) | g_URT2_RxBuf[2];
	toulong = (toulong << 8) | g_URT2_RxBuf[3];
	g_MAC_Anum = g_URT2_RxBuf[10]; 
	
	if(tou == 0xe817)
	{
		for(i=2; i<(g_URT2_RxLen-2); i++)
		{
			sum += g_URT2_RxBuf[i];
		}
		ctrl_FT = g_URT2_RxBuf[2] & 0XC0;
	}
	else if(toulong == 0xe8e8e817)
	{
		for(i=4; i<(g_URT2_RxLen-2); i++)
		{
			sum += g_URT2_RxBuf[i];
		}
		ctrl_FT = g_URT2_RxBuf[4] & 0XC0;
	}
	
	if((tou == 0XE817) || (toulong == 0XE8E8E817))    //头
	{
		if(g_URT2_RxBuf[g_URT2_RxLen - 1] == 0xE9)  //尾
		{
			if(sum == g_URT2_RxBuf[g_URT2_RxLen - 2])  //校验和
			{
				switch(ctrl_FT)
				{
					case 0x00: Cmd_f();      //命令帧
							   break;
					case 0xc0: Reply_f();	   //回答帧
							   break;
					case 0x40: Data_f();    g_SendDatFlga = 1;vtimer_SetTimer(VTIM3, 10000, En_SendDat); Delayms(2500);//数据帧
							   break;
					case 0x80: Confirm_f(); //确认帧
							   break;
				}
			}
		}
	}
	g_URT2_RxLen = 0;
	memset(g_URT2_RxBuf, 0, sizeof(g_URT2_RxBuf));
}

void Cmd_f()
{
	g_URT2_RxLen = 0;
	memset(g_URT2_RxBuf, 0, sizeof(g_URT2_RxBuf));
}

void Reply_f()  //回答帧
{
	u16 i,j;
	u8 cmd;
	u8 mac_num = 0;
	static u8 Rx_num = 0;
 	
	cmd = g_URT2_RxBuf[9];
	mac_num = g_URT2_RxBuf[10];
	g_MAC_num = g_URT2_RxBuf[12];
//	g_MAC_Snum += g_MAC_num;
	if(cmd == 0x17)
	{
		if(mac_num > 31)
		{
			for(i=186*Rx_num; i<((g_MAC_num * 6)+186*Rx_num); i++)
			{
				g_MACList[i] = g_URT2_RxBuf[i+13-186*Rx_num];
				
			}
			Rx_num++;
			if(Rx_num>(mac_num / 31))
				Rx_num = 0;
			for(j=0; j<i; j++)
			{
				g_URT1_TxBuf[j] = g_MACList[j];
			}
			g_MAC_Snum = i/6;
			g_URT1_TxLen = i;
			Uart1_PutChar(g_URT1_TxBuf[0]);
//			Pack_Data(g_MACList, (g_MAC_num * 6 + 186), 0x01);
		}
		else
		{
			for(i=0; i<(g_MAC_num * 6); i++)
			{
				g_MACList[i] = g_URT2_RxBuf[i+13];
				printf("%x,",g_MACList[i]);
			}
			for(j=0; j<i; j++)
			{
				g_URT1_TxBuf[j] = g_MACList[j];
			}
			g_MAC_Snum = i/6;
			g_URT1_TxLen = i;
			Uart1_PutChar(g_URT1_TxBuf[0]);
		}
	}
	
	g_URT2_RxLen = 0;
	memset(g_URT2_RxBuf, 0, sizeof(g_URT2_RxBuf));
}

void Data_f()
{
	switch(g_URT2_RxBuf[23])
						{
							case 0x00: AckFrame();//注册确认
							   break;
							case 0x06: if(g_URT2_RxBuf[24]==0x00)AckFrame();if(g_URT2_RxBuf[24]==0x01)InquireFrame();	//调光曲线查询和设置确认
							   break;
							case 0x02: HeartDataUp();// 心跳数据
							   break;	
							case 0x07: if(g_URT2_RxBuf[24]==0x00)AckFrame();if(g_URT2_RxBuf[24]==0x01)InquireFrame();	//恒照度系数查询和设置确认
							   break;
						}		
}

void Confirm_f()
{
	g_URT2_RxLen = 0;
	memset(g_URT2_RxBuf, 0, sizeof(g_URT2_RxBuf));
}

void USART1_RxData()
{
	u16 head = 0;
	u32 headlong = 0;
	u8 end = 0;
	u16 crc = 0;//,pageaddr
	//u8 UseData[20];
	//u8 MacAddrNum=0;
	
	head = (g_URT1_RxBuf[0] << 8) | g_URT1_RxBuf[1];	
	headlong = (head << 8) | g_URT1_RxBuf[2];
	headlong = (headlong << 8) | g_URT1_RxBuf[3];
	end = g_URT1_RxBuf[g_URT1_RxLen - 1];

if(headlong == 0x3c3c3c5a)  //头
	{
		if(end == 0xED)
		{
			crc = Get_Crc16(g_URT1_RxBuf,(g_URT1_RxLen-1));
	//		printf("%x",crc);
			if(crc == 0)    //crc
			{
				if(g_URT1_RxBuf[16]==0)//控制字
					{				   											 															
						switch(g_URT1_RxBuf[17])
						{
							case 0x00: RegisterDevice();//注册
							   break;
							case 0x01: QueryDevice();//查询
							   break;
							case 0x02: RemoveDevice();// 删除  
							   break;				
						}						
					}
					
				if(g_URT1_RxBuf[16]==7||g_URT1_RxBuf[16]==6||g_URT1_RxBuf[16]==5)//恒照度系数、调光曲线、LED灯控制					
						SendData();					
									
				if(g_URT1_RxBuf[16]==3)//心跳间隔时间				
						HeartbeatInterval();
									
				if(g_URT1_RxBuf[16]==4)//设置时间
				{
					if(g_URT1_RxBuf[17]==0)
					{
						Set_Time();
					}else{
						InquireTimer();
					}
				}
			}
	}				
}
  g_URT1_RxLen = 0;
	memset(g_URT1_RxBuf, 0, sizeof(g_URT1_RxLen + 6));

}


void Pack_Frame(u8 *data,u8 len)//PLC协议帧
{
	u8 Frame[50]= {0xE8,0xE8,0xE8,0x17,0x61,0x03,0x0D,0x00,0x0C,0x18,0x40,
					0x20,0x01,0xff,0xff,0xff,0xff,0xff,0xff};
	u8 i,sum = 0;
	static u8 FSN = 0X60;
					
	for(i=0; i<len; i++)
	{
		Frame[19+i] = data[i];
	}
	Frame[4] = FSN;
	Frame[6] = len+11;
	
	for(i=4; i<(len+19); i++)
	{
		sum += Frame[i]; 
	}
	Frame[len+19] = sum;
	Frame[len+20] = 0XE9;
	g_URT2_RxLen=len+21;
	for(i=0;i<g_URT2_RxLen;i++)
	g_URT2_TxBuf[i]=Frame[i];
	
	for(i=0;i<len+21;i++)
	{
		Uart2_PutChar(g_URT2_TxBuf[i]);
	  Delayms(5);
	}
	FSN++;
	if(FSN >= 0x6f) FSN = 0X60;
	
	g_URT2_RxLen = 0;
	memset(g_URT2_RxBuf, 0, sizeof(g_URT2_TxLen));
}

void SelfPack_Frame(u8 *data,u8 len)//自定义协议帧
{
	u8 i,farme[50]={0x3c,0x3c,0x3c,0x5a,0x30,0x00,0x10,0x00,0x00,0x00,0x00,0x00};
	u16 crc;
	
	for(i=0;i<6;i++)
		farme[6+i]=g_URT1_RxBuf[6+i];
	for(i=0;i<len;i++)
	farme[12+i]=data[i];

	crc = Get_Crc16(farme,12+len);
	farme[12+len]=crc&0xff;
	farme[13+len]=(crc>>8)&0xff;
	farme[14+len]=0xed;
	for(i=0;i<15+len;i++)
	{
		Uart1_PutChar(farme[i]);
	}
}

void SendData(void)//下发数据
{
	u8 CmdData[20],i;
					
	for(i=0;i<g_URT1_RxBuf[14]+3;i++)  
			CmdData[i]=g_URT1_RxBuf[12+i];
	Pack_Frame(CmdData,g_URT1_RxBuf[14]+3);
}

void HeartbeatInterval()//心跳间隔时间
{
	u8 HertReply[8]={0x01,0x00,0x05,0x01,0x03,0x00,0x00,0x00};
					
	if(g_URT1_RxBuf[17]==0)//设置心跳间隔时间
	{						
		write_byte(0x03fb,g_URT1_RxBuf[18]);
		Delayms(10);
		write_byte(0x03fc,g_URT1_RxBuf[19]);
		SelfPack_Frame(HertReply,8);
	}else//查询心跳间隔时间
	{
		HertReply[6]=Read_RanAddr(0x03fb);
		HertReply[7]=Read_RanAddr(0x03fc);
		HertReply[5]=0x01;
		SelfPack_Frame(HertReply,8);
	}
}
					
void InquireTimer()//查询时间
{
	u8 i,j,InquireTime[11]={0x01,0x00,0x08,0x01,0x04,0x01,0x17,0x07,0x13,0x11,0x22};
						
	Read_RTC(0x00);
	for(i=0;i<6;i++)
	{
		if(i==3)
		{
			j=1;
		}
		else
		{
			InquireTime[6+i-j]=g_Rtc_Dat[6-i];
		}
	}
	j=0;
	//for(i=0; i<=7; i++)
	//printf("%x,",g_Rtc_Dat[i]); 
	SelfPack_Frame(InquireTime,11);
}
void Set_Time()//设置时间
{
	//u16 year=0x07d0;
	u8 TimrReply[8]={0x01,0x00,0x05,0x01,0x04,0x00,0x00,0x00};
	u8 i,j;
	u8 set_rtc_code[8]={0x00,0x27,0x18,0x02,0x04,0x07,0x17,0x13};
	
	for(i=0;i<6;i++)
	{
		if(i==3)
		{
			set_rtc_code[6-i]=0x03;
			j=1;
		}else
		{
			set_rtc_code[6-i]=g_URT1_RxBuf[18+i-j];			
		}
	}
	j=0;
	Set_RTC(set_rtc_code); //设置时间		
				
	SelfPack_Frame(TimrReply,8);
}

void AckFrame()//确认帧
{
	u8 i,Registered_Reply[22]={0x3c,0x3c,0x3c,0x5a,0x30,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x04,0x02,0x00,0x00,0x00,0x13,0x47,0xed};
			 u16 crc;
		
	for(i=0;i<6;i++)
		Registered_Reply[6+i]=g_URT1_RxBuf[6+i];
		
		if((g_URT2_RxBuf[24]==0)&&(g_URT2_RxBuf[25]==0))
		 {
				Registered_Reply[12]=g_URT2_RxBuf[19];
			  Registered_Reply[13]=g_URT2_RxBuf[20];
				Registered_Reply[15]=g_URT2_RxBuf[22];
				Registered_Reply[16]=g_URT2_RxBuf[23];
				Registered_Reply[17]=g_URT2_RxBuf[24];
		 }
		
		 crc = Get_Crc16(Registered_Reply,19);

		Registered_Reply[19]=crc&0xff;
		Registered_Reply[20]=(crc>>8)&0xff;
									
		for(i=0;i<22;i++)
		{
			Uart1_PutChar(Registered_Reply[i]);
		}			
}

void InquireFrame()//查询返回帧
{
  u8 i,Inquiredata[50]={0x3c,0x3c,0x3c,0x5a,0x30,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x04,0x02,0x00,0x00,0x00,0x13,0x47,0xed};
	u16 crc;
	
for(i=0;i<6;i++)
		Inquiredata[6+i]=g_URT1_RxBuf[6+i];
		
	for(i=0;i<g_URT2_RxBuf[21]+3;i++)
	Inquiredata[12+i]=g_URT2_RxBuf[19+i];

	crc = Get_Crc16(Inquiredata,13+i);
	Inquiredata[12+i]=crc&0xff;
		i++;
		Inquiredata[12+i]=(crc>>8)&0xff;	
		i++;
		Inquiredata[12+i]=0xed;							
		for(i=0;i<18+g_URT2_RxBuf[21];i++)
		{
			Uart1_PutChar(Inquiredata[i]);
		}	
		
}

void RegisterDevice()//注册设备
{
	u8 i,sendbuf[22],MacAddrNum=0;
	u16 crc;
	if(g_URT1_RxBuf[15]==1)//主设备
				      {
						    write_byte(0x03fe,g_URT1_RxBuf[12]);
					      Delayms(10);
				        write_byte(0x03ff,g_URT1_RxBuf[13]);
					
								for(i=0;i<g_URT1_RxLen-3;i++)
								{
									sendbuf[i]=g_URT1_RxBuf[i];
								}
								sendbuf[i]=0x00;
								sendbuf[14]=0x04;
								crc = Get_Crc16(sendbuf,19);
								sendbuf[19]=crc&0xff;
								sendbuf[20]=(crc>>8)&0xff;
								sendbuf[21]=0xed;
					
								Delayms(100);
								for(i=0;i<22;i++)
								{
									Uart1_PutChar(sendbuf[i]);
								}
							}
					
							if(g_URT1_RxBuf[15]==2)//从设备
							{
								u16 short_addr;
								u8 addr[8],i=0;
								
								short_addr=g_URT1_RxBuf[13];
								short_addr=short_addr<<8;
								short_addr=g_URT1_RxBuf[12];
															
								for(i=0;i<6;i++)
								{
									write_byte((short_addr-2)*6+i,g_URT1_RxBuf[18+i]);
									Delayms(10);
								}	
								MacAddrNum=Read_RanAddr(0x3fd);
								MacAddrNum++;
								Delayms(10);
								write_byte(0x3fd,MacAddrNum);
								
								for(i=0;i<6;i++)
								addr[i]=g_URT1_RxBuf[18+i];
								addr[6]=g_URT1_RxBuf[12];
								addr[7]=g_URT1_RxBuf[13];
								
								Pack_Frame(addr,8);
							}
}

void QueryDevice()//查询设备短地址和MAC地址 
{
		u8 i,sum=0,j,delcout=0,InquireDevReply[27]={0x3c,0x3c,0x3c,0x5a,0x30,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x02,0x00,0x01,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xed};
		u16 crc,MAC_num,pageaddr;
		
		for(i=0;i<6;i++)
		InquireDevReply[6+i]=g_URT1_RxBuf[6+i];
			
		if(g_URT1_RxBuf[15]==2)//PLC单灯控制器
		{
				if((g_URT1_RxBuf[13]==0)&&(g_URT1_RxBuf[12]==0))//查询短地址
				{
						MAC_num=Read_RanAddr(0x3fd);
									
						for(i=0;i<MAC_num;i++)
						{
							for(j=0;j<6;j++)
							{
								if(Read_RanAddr(i*6+j)==0xff)
								{
									i--;
									delcout++;
									break;
								}
								else
								{
										sum+=g_URT1_RxBuf[18+j]-Read_RanAddr(i*6+j);
										if(sum!=0)break;
											if((sum==0)&&(j==5))
											{
												crc=i+delcout+2;
													
												InquireDevReply[12]=crc&0xff;
												InquireDevReply[13]=(crc>>8)&0xff;
													
												for(i=0;i<6;i++)
												InquireDevReply[18+i]=g_URT1_RxBuf[18+i];
													
												crc = Get_Crc16(InquireDevReply,24);
												InquireDevReply[24]=crc&0xff;
												InquireDevReply[25]=(crc>>8)&0xff;
												for(i=0;i<27;i++)
												{
													Uart1_PutChar(InquireDevReply[i]);
												}
											}
									}
								}
								sum=0;
							}
					}else//查询MAC地址
						{
							pageaddr=g_URT1_RxBuf[13];
							pageaddr=pageaddr<<8;
							pageaddr=g_URT1_RxBuf[12];
																	
						  InquireDevReply[12]=g_URT1_RxBuf[12];
							InquireDevReply[13]=g_URT1_RxBuf[13];
								
							for(i=0;i<6;i++)
								InquireDevReply[18+i]=Read_RanAddr((pageaddr-2)*6+i);
									
							crc = Get_Crc16(InquireDevReply,24);
							InquireDevReply[24]=crc&0xff;
							InquireDevReply[25]=(crc>>8)&0xff;
									
							for(i=0;i<27;i++)
							{
								Uart1_PutChar(InquireDevReply[i]);
							}								
						}								
		}		
}

void RemoveDevice()//删除设备
{
	u8 i=0,DelReply[21]={0x3c,0x3c,0x3c,0x5a,0x30,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x04,0x02,0x00,0x02,0x12,0x27,0xed};
	u16 MAC_num,pageaddr;	
	
	for(i=0;i<6;i++)
		DelReply[6+i]=g_URT1_RxBuf[6+i];
		
	pageaddr=g_URT1_RxBuf[13];
	pageaddr=pageaddr<<8;
	pageaddr=g_URT1_RxBuf[12];
	if(g_URT1_RxBuf[15]==2)//从设备才删除
	{
		for(i=0;i<6;i++)
		{
			write_byte((pageaddr-2)*6+i,0xff);
			Delayms(10);
		}	
		DelReply[12]=g_URT1_RxBuf[12];
		DelReply[13]=g_URT1_RxBuf[13];
								
		MAC_num=Read_RanAddr(0x3fd);//删除设备后设备数减少一个
		MAC_num--;
		write_byte(0x3fd,MAC_num);	
								
		for(i=0;i<21;i++)
		{
			Uart1_PutChar(DelReply[i]);
		}			
	}		
	RemoveDeviceFlag=1;	
}

void HeartDataUp()
{
	u8 i,heartdata[20];
	
	for(i=0;i<g_URT2_RxBuf[21]+3;i++)
		heartdata[i]=g_URT2_RxBuf[19+i];
	
		SelfPack_Frame(heartdata,g_URT2_RxBuf[21]+3);	
}
