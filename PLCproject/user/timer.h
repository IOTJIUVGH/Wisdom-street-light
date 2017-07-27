#ifndef __TIMER_H
#define __TIMER_H
#include "stdio.h"	
#include "stm32f10x.h"
extern u8 TIMER3_flag,TIMER4_flag;
extern u8 USART_falg,USART2_falg;

void TIM3_Int_Init(u16 arr,u16 psc);
void TIMER3_UP__NVIC_Init(void);

void TIM4_Int_Init(u16 arr,u16 psc);
void TIMER4_UP__NVIC_Init(void);
#endif
