#ifndef __V_Timer_H
#define __V_Timer_H

#include "stm32f10x.h"

typedef unsigned short u16;
typedef unsigned char  u8;
typedef enum
{
  FALSE_V = 0,
  TRUE_V = !FALSE_V
}
bool;

typedef enum {VTIM0,VTIM1,VTIM2,VTIM3,VTIM4,VTIM5,VTIM6,VTIM7,VTIM8,VTIM9,VTIM10,VTIMER_NUM} VtimerName_t;//VtimerName_t 是enum VtimerName的别名

typedef u32 timer_res_t;

typedef struct
{
	timer_res_t msec;
	void* pCallback;
} Vtimer_t,*PVtimer;

void 	vtimer_init(void);
void 	vtimer_SetTimer(VtimerName_t name,timer_res_t  msec,void* pCallback);
void 	vtimer_KillTimer(VtimerName_t name);
u8 		vtimer_TimerElapsed(VtimerName_t name);
void 	vtimer_UpdateHandler(void);

void Init_TIM2(void);
void TIM3_IRQHandler(void);

#endif
