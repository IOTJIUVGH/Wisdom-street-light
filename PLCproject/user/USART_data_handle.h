#ifndef __USART_DATA_HANDLE_H
#define __USART_DATA_HANDLE_H

#include "stm32f10x.h"
#include "usart.h"
#include "crc.h"
#include "cmd.h"

void InquireFrame(void);
void AckFrame(void);
void Set_Time(void);   
void USART1_RxData(void);
void Reply_f(void);
void Data_f(void);
void Confirm_f(void);
void Cmd_f(void);
void Find_MAC(void);
void USART2_RxData(void);
void Pack_Frame(u8 *data,u8 len);
void SelfPack_Frame(u8 *data,u8 len);
void RegisterDevice(void);
void QueryDevice(void);
void RemoveDevice(void);
void InquireTimer(void);
void HeartbeatInterval(void);
void SendData(void);
void HeartDataUp(void);
u8 comp_MAC(void);
extern u8 RemoveDeviceFlag;



#endif
