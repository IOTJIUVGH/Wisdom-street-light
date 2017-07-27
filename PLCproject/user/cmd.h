#ifndef __CMD_H
#define __CMD_H

#include "stm32f10x.h"
#include "usart.h"
#include "flash.h"

extern u8 g_GetEdit_Flag;
extern u8 NextFindFlag;

void Get_MAC(void);
void Get_MACList(u8 First);
void Get_Edition(void);
void Poll_Heartdata(u8 dat);
void En_GetEdit(void);
void En_SendDat(void);
void Set_CurXS(void);
void Find_CurXS(void);
void RegisterMasterDevice(void);
void RotationEquipment(void);//ÂÖÑµÖÕ¶ËÐÄÌø
void Uart1RxDatahandle(void);
void Uart2RxDatahandle(void);

#endif
