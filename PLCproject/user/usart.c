#include "usart.h"

u16 g_URT1_RxLen = 0;
u16 g_URT1_TxLen = 0;
u8 g_URT1_RxBuf[200] = {0};
u8 g_URT1_TxBuf[350];
u16 m_URT1_TxNum = 1;
u8 g_URT1_RxFlag = 0;
u8 g_URT1_TxEn = 0;

u8 g_URT2_RxFlag = 0;
u8 g_URT2_RxBuf[200] = {0};
u16 g_URT2_RxLen = 0;
u8 g_URT2_TxEn = 0;
u8 g_URT2_TxBuf[200] = {0};
u8 m_URT2_TxNum = 1;
u16 g_URT2_TxLen = 0;
u8 g_CntrMAC[6] = {0x10,0x00,0x00,0x00,0x00,0x00};
u8 g_URT1_RxDat[100] = {0};
u8 g_URT1_RxDtLng = 0;
u8 g_MACList[350] = {0};
u8 g_MAC_num = 0;
u8 g_MAC_Anum = 0;
u8 g_SendEditFlag = 0;
u8 g_SendDatFlga = 0;
u8 g_URT2_TxFinish = 0;

u16 g_MAC_Snum = 0; 

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}

//////////////////////////////////////////////////////////////////

void Uart1_PutChar(u8 ch)
{
	u8 a;
	
  a=USART1->SR;
	
	USART1->DR=ch;
  while((USART1->SR&0X40)==0);//�ȴ����ͽ���
}

void Uart2_PutChar(u8 ch)
{
  USART2->DR=ch;
  while((USART1->SR&0X40)==0);//�ȴ����ͽ���
}
//void Uart1_PutChar(u8 ch)
//{
//	if(g_URT1_TxEn == 0)
//	{
//		while((USART1->SR & 0X40) == 0){}//�ȴ����ͽ���
//		USART1->DR = ch;
//		g_URT1_TxEn = 1;
//		USART1->CR1 |= 1<<6;    //��������ж�ʹ��
//	}
//}
//void Uart2_PutChar(u8 ch)
//{
//	while((USART2->SR & 0X40) == 0){}//�ȴ����ͽ���
//    USART2->DR = ch;
//	g_URT2_TxEn = 1;
//	USART2->CR1 |= 1<<6;    //��������ж�ʹ��
//}

//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������
void uart1_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp = (float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa = temp;				 //�õ���������
	fraction = (temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F; 
	GPIOA->CRH|=0X000008B0;//IO״̬����PA9,PA10
		  
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1 |= 0X200C;  //1λֹͣ,��У��λ.
//	USART1->CR1 |= 0X340C;  //1λֹͣ,żУ��λ.
	USART1->CR1 &= 0XFDFF; 

//	USART1->CR1|=1<<6;    //��������ж�ʹ��
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	   
	NVIC->IP[37] = 0x30;
	NVIC->ISER[1] |= 1<<5;
}


void uart2_init(u32 pclk2,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB1ENR|=1<<17;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRL&=0XFFFF00FF; 
	GPIOA->CRL|=0X00008B00;//IO״̬����PA2,PA3
		  
	RCC->APB1RSTR|=1<<17;   //��λ����1
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.

	
//	USART1->CR1|=1<<8;    //PE�ж�ʹ��
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	 	
}

void USART2_NVIC_Init()
{
	SCB->AIRCR &=0x05faf8ff;
	SCB->AIRCR |=0x05fa0400;
	
	NVIC->ISER[1] |=1<<6;
  NVIC->IP[38]=0x30;
}

void USART2_IRQHandler(void)
{
	u8 getdata,a;
	
	if( USART2 -> SR & 0x20 )
	{
		if(g_URT2_RxFlag==0)
		{
			getdata = USART2 -> DR;
			//Uart1_PutChar(getdata);
			g_URT2_RxBuf[g_URT2_RxLen]=getdata;
			g_URT2_RxLen++;
			//USART2_falg=1;
				vtimer_SetTimer(VTIM2, 100, USART2_over_time);
		}else
		{
			a=USART2 -> SR;		
			
		}
	}		
}
////��ʼ��IO ����2
////pclk2:PCLK2ʱ��Ƶ��(Mhz)
////bound:������
//void uart2_init(u32 pclk2,u32 bound)
//{  	 
//	float temp;
//	u16 mantissa;
//	u16 fraction;	   
//	temp = (float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
//	mantissa = temp;				 //�õ���������
//	fraction = (temp-mantissa)*16; //�õ�С������	 
//    mantissa <<= 4;
//	mantissa += fraction; 
//	RCC->APB2ENR |= 1<<2;   //ʹ��PORTA��ʱ��  
//	RCC->APB1ENR |= 1<<17;  //ʹ�ܴ���2ʱ�� 
//	GPIOA->CRL &= 0XFFFF00FF; 
//	GPIOA->CRL |= 0X00008B00;//IO״̬����PA2,PA3
//		  
//	RCC->APB1RSTR |= 1<<17;   //��λ����2
//	RCC->APB1RSTR &= ~(1<<17);//ֹͣ��λ	   	   
//	//����������
// 	USART2->BRR=mantissa; // ����������	 
////	USART2->CR1 |= 0X200C;  //1λֹͣ,��У��λ.
//	USART2->CR1 |= 0X340C;  //1λֹͣ,żУ��λ.
//	USART2->CR1 &= 0XFDFF; 
////	USART2->CR1|=1<<6;    //��������ж�ʹ��
//	USART2->CR1 |= 1<<5;    //���ջ������ǿ��ж�ʹ��	  
//	NVIC->IP[38] = 0x30;
//	NVIC->ISER[1] |= 1<<6;
//}

void USART1_NVIC_Init()
{
	SCB->AIRCR &=0x05faf8ff;
	SCB->AIRCR |=0x05fa0400;
}



void USART1_over_time()
{
	g_URT1_RxFlag = 1;
	vtimer_KillTimer(VTIM1);
}
void USART2_over_time()
{
	g_URT2_RxFlag = 1;
	vtimer_KillTimer(VTIM2);
}

/*************
����1�жϺ���
�����ж�   �����ж�
*************/

void USART1_IRQHandler(void)
{
	u8 temp;

	if(USART1->SR & 0x20)
	{
		if(g_URT1_RxFlag == 0)
		{
			g_URT1_RxBuf[g_URT1_RxLen] = USART1->DR;
			g_URT1_RxLen++;
			vtimer_SetTimer(VTIM1, 100, USART1_over_time);
		}
		else
			temp = USART1->DR;	
			
	}
	
	if((USART1->SR & 0X40) && g_URT1_TxEn)
	{
		USART1->DR = g_URT1_TxBuf[m_URT1_TxNum];

		m_URT1_TxNum++;
		if(m_URT1_TxNum >= g_URT1_TxLen)
		{
			USART1->CR1 &= ~(1<<6);    
			m_URT1_TxNum = 1;
			g_URT1_TxEn = 0;
			g_URT1_TxLen = 0;
			
		}
	}
}

/*************
����2�жϺ���
�����ж�   �����ж�
*************/

//void USART2_IRQHandler(void)
//{
//	u8 temp;

//	if(USART2->SR & 0x20)
//	{
//		if(g_URT2_RxFlag == 0)
//		{
//			g_URT2_RxBuf[g_URT2_RxLen] = USART2->DR;
//			g_URT2_RxLen++;
//			vtimer_SetTimer(VTIM2, 100, USART2_over_time);
//		}
//		else 
//		{
//			temp = USART2->DR;	
//		}
//	}
//	
//	if((USART2->SR & 0X40) && g_URT2_TxEn)
//	{
//		USART2->DR = g_URT2_TxBuf[m_URT2_TxNum];

//		m_URT2_TxNum++;
//		if(m_URT2_TxNum >= g_URT2_TxLen)
//		{
//			USART2->CR1 &= ~(1<<6);    
//			m_URT2_TxNum = 1;
//			g_URT2_TxEn = 0;
//			g_URT2_TxLen = 0;
//			g_URT2_TxFinish = 1;
////			memset(g_URT1_TxBuf, 0, sizeof(g_URT1_TxBuf));
//		}
//	}
//}

//void Pack_Data(u8* data,u16 len,u8 cmd)
//{
//	u8 packdata[350], x, y=10, i;
//	u16 crc;
//	
//	packdata[0] = 0x5a;
//	packdata[1] = 0x3c;
//	if(cmd==0x00)
//	{
//		for(i=0; i<6; i++)
//		{
//			packdata[i+2] = 0xff;
//		}
//	}else
//	{
//		for(i=0; i<6; i++)
//		{
//			packdata[i+2] = g_CntrMAC[i];
//		}
//	}
//	packdata[8] = cmd;
//	packdata[9] = len;
//	
//	for(x=0; x<len; x++)
//	{
//		packdata[y] = data[x];
//		y++;
//	}
//	crc = Get_Crc16(packdata,y);

//	packdata[y] = crc & 0xff;
//	y++;
//	packdata[y] = (crc>>8) & 0xff;
//	y++;
//	
//	i = USART1->SR;         
//	for(x=0; x<y; x++)
//	{
//		g_URT1_TxBuf[x] = packdata[x];
//	}
//	g_URT1_TxLen = y;
//	Uart1_PutChar(g_URT1_TxBuf[0]);

//}
