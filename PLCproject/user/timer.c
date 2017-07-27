#include "stm32f10x.h"
#include "usart.h"

u8 TIMER3_flag=0,TIMER4_flag=0;
u8 USART_falg=0,USART2_falg=0;
void TIM3_IRQHandler(void)
{ 		    		  			    
	
	if(TIM3->SR&0X0001)//溢出中断
	{
		
				
				TIMER3_flag=1;
				USART_falg=0;
		//printf("DD");
		
//		TIM3->PSC=7199; 	
						   
			TIM3->SR&=~(1<<0);//清除中断标志位 	    
	}
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值 
	TIM3->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM3->DIER|=1<<0;   //允许更新中断	 
	TIM3->CR1|=0x01;    //使能定时器3
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
	if(TIM4->SR&0X0001)//溢出中断
	{
		
		if(USART2_falg==1)
		{
			//Uart1_PutChar('A');

				TIMER4_flag=1;
				USART2_falg=0;

		
//		TIM3->PSC=7199; 	
		}				   
			TIM4->SR&=~(1<<0);//清除中断标志位 	    
	}




}
void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4时钟使能    
 	TIM4->ARR=arr;  	//设定计数器自动重装值 
	TIM4->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM4->DIER|=1<<0;   //允许更新中断	 
	TIM4->CR1|=0x01;    //使能定时器4
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
