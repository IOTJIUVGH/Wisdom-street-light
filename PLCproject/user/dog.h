#ifndef __DOG_H
#define __DOG_H

#include "stm32f10x.h"
extern  u8 WWDG_CNT;   
void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed(void);
void WWDG_Init(u8 tr,u8 wr,u8 fprer);
void WWDG_Set_Counter(u8 cnt);
void WWDG_NVIC_Init(void);
#endif
