#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "stdio.h"	
#include "V_Timer.h"
#include <string.h>
#include "crc.h"

#define FLASH_SAVE_CURXS   0x0801FC00


extern u8 g_URT1_TxEn; 
extern u8 g_URT1_RxFlag;
extern u16 g_URT1_RxLen;
extern u16 g_URT1_TxLen;
extern u8 g_URT1_RxBuf[200];
extern u8 g_URT1_TxBuf[350];

extern u8 g_URT2_RxFlag;
extern u8 g_URT2_RxBuf[200];
extern u16 g_URT2_RxLen;
extern u8 g_URT2_TxEn;
extern u8 g_URT2_TxBuf[200];
extern u16 g_URT2_TxLen;

extern u8 g_CntrMAC[6];
extern u8 g_URT1_RxDat[100];
extern u8 g_URT1_RxDtLng;
extern u8 g_MACList[350] ;
extern u8 g_MAC_num;
extern u8 g_SendEditFlag;
extern u8 g_SendDatFlga;
extern u8 g_URT2_TxFinish;
extern u8 g_MAC_Anum;

extern u16 g_MAC_Snum; 

void uart1_init(u32 pclk2,u32 bound);
void uart2_init(u32 pclk2,u32 bound);
void Uart1_PutChar(u8 ch);
void Uart2_PutChar(u8 ch);
void USART1_NVIC_Init(void);
void Pack_Data(u8* data,u16 len,u8 cmd);
void USART2_NVIC_Init(void);
void USART2_over_time(void);

#endif	   
















