#include "stm32f10x.h"
#include "rcc.h"
#include "usart.h"
#include "dog.h"
#include "crc.h"
#include "delay.h"
#include "string.h"
#include <stdlib.h>
#include "V_Timer.h"
#include "USART_data_handle.h"
#include "led.h"
#include "flash.h"
#include "i2c.h"
#include "24c32.h"
#include "timer.h"

int main()
{
	SystemInit();
	Delayms(500);
	
	vtimer_init();
	
	uart1_init(72,9600);
	uart2_init(72,9600*2);	
	USART1_NVIC_Init();
	USART2_NVIC_Init();
	
//TIM3_Int_Init(29999,7199);//��ʾ3��
//TIMER3_UP__NVIC_Init();	
//IWDG_Init(4,1000); 
	printf("GO");
	
	init_i2c1();
	en_24c32();//��ʼ��EPPROM
//write_byte(0x03fe,0xff);
//Delayms(10);
//write_byte(0x03ff,0xff);
//Uart2_PutChar('A');
//write_byte(0x3fd,0x00);
//Storage_MACAddr(g_MACAddr,240);
	vtimer_SetTimer(VTIM0, 5000, En_GetEdit);//ע�Ἧ����
	vtimer_SetTimer(VTIM3, 10000, En_SendDat);
	
	while(1)
	{			
		RegisterMasterDevice();//ע�����豸
		
		RotationEquipment();//��ѵ�ն��豸����
			
		Uart1RxDatahandle();//����1�������ݴ���
			
		Uart2RxDatahandle();//����2�������ݴ���		
	}
}	

