#include "V_Timer.h"


static Vtimer_t sVtimer[VTIMER_NUM]; //�ṹ������

/*
*������:Init_TIM2
*����:TIM2��ʼ��
*����:��
*���:��
*����:�ⲿ����
*/
void Init_TIM2(void)
{
	NVIC->ISER[0] |= 0X0001 << 28; 
	NVIC->IP[28] = 0X10;  //��ռ���ȼ�Ϊ1
	
	RCC->APB1ENR |= 0X0001; 

	TIM2->CNT = 0x0000;
	TIM2->PSC = 0x0047;  		// 72��Ƶ
	TIM2->ARR = 0x03E8;		// 1KHz
//	TIM2->ARR = 0x0dc;
	TIM2->DIER |= 0x0001; 	// update interrupt enabled
	TIM2->CR1 |= 0x0001;	

}

void vtimer_init()
{
	u8 i;
	for (i = 0; i < VTIMER_NUM; i++)
	{
		sVtimer[i].msec = 0;
		sVtimer[i].pCallback = 0;
	}
	
	Init_TIM2();
}

void vtimer_SetTimer(VtimerName_t name,timer_res_t  msec,void* pCallback)  //name ��enum VtimerName_t���� msec�� u16����
{
	sVtimer[name].msec = msec;
	sVtimer[name].pCallback = pCallback;
}

void vtimer_KillTimer(VtimerName_t name)
{
	sVtimer[name].msec = 0;
	sVtimer[name].pCallback = 0;
}

u8 vtimer_TimerElapsed(VtimerName_t name)
{
	if (sVtimer[name].msec == 0)
		return TRUE_V;
	else
		return FALSE_V;
}

typedef void(*PFN_Callback_t)(void);

//ÿ�� deltat ms ����һ��callback
void vtimer_UpdateHandler(void)    		
{
	//Enter each DELTAT_MS ms
	u8 i;
	
	for (i = 0; i < VTIMER_NUM; i++)  //11
	{
		if (sVtimer[i].msec != 0)
		{
			sVtimer[i].msec--;
			if (sVtimer[i].pCallback != 0)
			{
				if (sVtimer[i].msec == 0) 
				{
					((PFN_Callback_t)sVtimer[i].pCallback)();
				}
			}
		}
	}
}

/*
*������:TIM2_IRQHandler
*����:TIM2�жϺ���
*����:��
*���:��
*����:ϵͳ�Զ�����
*/
void TIM2_IRQHandler(void)
{
	
	//Timer2Ƶ�ʼ��
//	static int i = 0;
//	TIM2->SR &= ~(0x0001);
//	if(i==0){
//		GPIOB->BSRR &= 0XFFFDFFFD;
//		GPIOB->BSRR |= 0x00000002;
//		i=1;
//	}
//	else {
//		GPIOB->BSRR &= 0XFFFDFFFD;
//		GPIOB->BSRR |= 0x00020000;
//		i=0;
//	}

	
	// timer2 interrupt code
	TIM2->SR &= ~(0x0001); // clean timer2 update interrupt flag 
	vtimer_UpdateHandler();
	
	return;
}
