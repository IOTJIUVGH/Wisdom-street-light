#include "stm32f10x.h"

void Init_SyStemClock_inside(void)
{
	RCC->CR |= 0x0001;								// HSION 内部8MHz时钟开启
	while(!(RCC->CR & (0x0001<<1)));
	RCC->CFGR |=  0x00000000;					// set PLLSRC HSE作为时钟源
	RCC->CFGR |= 0x0007<<18;					// PLLMUL: PLL倍频系数 9
	RCC->CR |= 0x0001<<24;						//PLLON: PLL使能
	while(!(RCC->CR & (0x0001<<25)));	
	RCC->CFGR |= 0x0001<<1;						//SW:系统时钟切换 PLL输出作为系统时钟源
	while(!(RCC->CFGR & (0x0001<<3)));
	
}

void Init_SystemClock_outside(void)
{
	RCC->CR |=0x1<<16;//HSEON 外部时钟开启8HZ
	while(!(RCC->CR & (0x0001<<17)));
	RCC->CFGR |=  0x0001<<16;					// set PLLSRC HSE作为时钟源
	RCC->CFGR |= 0x0007<<18;					// PLLMUL: PLL倍频系数 9
	RCC->CR |= 0x0001<<24;						//PLLON: PLL使能
	while(!(RCC->CR & (0x0001<<25)));	
	RCC->CFGR |= 0x0001<<1;						//SW:系统时钟切换 PLL输出作为系统时钟源
	while(!(RCC->CFGR & (0x0001<<3)));
}
