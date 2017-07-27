#include "stm32f10x.h"
#include "usart.h"

u8 TIMER3_flag=0,TIMER4_flag=0;
u8 USART_falg=0,USART2_falg=0;
void TIM3_IRQHandler(void)
{ 		    		  			    
	
	if(TIM3->SR&0X0001)//����ж�
	{
		
				
				TIMER3_flag=1;
				USART_falg=0;
		//printf("DD");
		
//		TIM3->PSC=7199; 	
						   
			TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
	}
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  
	TIM3->DIER|=1<<0;   //��������ж�	 
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
}

void TIMER3_UP__NVIC_Init()
{
//	SCB->AIRCR &=0x05faf8ff;
//	SCB->AIRCR |=0x05fa0400;
//	
//	NVIC->ISER[1] |=0x19;
//  NVIC->IP[37]=0x30;
		
	 SCB->AIRCR |= 0x5 << 8; 
   NVIC->IP[29] |= 0;   
   NVIC->ISER[0] |=1<<29; 
	
}

void TIM4_IRQHandler(void)
{
	if(TIM4->SR&0X0001)//����ж�
	{
		
		if(USART2_falg==1)
		{
			//Uart1_PutChar('A');

				TIMER4_flag=1;
				USART2_falg=0;

		
//		TIM3->PSC=7199; 	
		}				   
			TIM4->SR&=~(1<<0);//����жϱ�־λ 	    
	}




}
void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4ʱ��ʹ��    
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM4->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  
	TIM4->DIER|=1<<0;   //��������ж�	 
	TIM4->CR1|=0x01;    //ʹ�ܶ�ʱ��4
}

void TIMER4_UP__NVIC_Init()
{
//	SCB->AIRCR &=0x05faf8ff;
//	SCB->AIRCR |=0x05fa0400;
//	
//	NVIC->ISER[1] |=0x19;
//  NVIC->IP[37]=0x30;
		
	 SCB->AIRCR |= 0x5 << 8; 
   NVIC->IP[30] |= 0;   
   NVIC->ISER[0] |=1<<30; 
	
}
