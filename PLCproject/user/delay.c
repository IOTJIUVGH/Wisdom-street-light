#include "stm32f10x.h"


//∫¡√Îº∂—” ±
void Delayms(u16 ms)
{
	unsigned int i = 0;
	while (ms--)
	{
		for (i = 0; i < 8000; ++i)
		{
			
		}
	}
}

void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*9; 
	SysTick->VAL=0x00;       
	SysTick->CTRL=0x01 ;     
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16))); 
	SysTick->CTRL=0x00;      
	SysTick->VAL =0X00;    
}
