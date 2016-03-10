/*
  *******************************************************************************
  * @file    rs485up.h
  * @author  zjjin
  * @version V0.0.0
  * @date    02-24-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

#ifndef _RS485UP_H_
#define _RS485UP_H_

#include "includes.h"




extern int pthread_RS485up_Rec(void);
extern INT32 RS485up_Rec(void);
extern void RS485Up_FileSend(uint8 Dev, FILE *fp);
extern uint8 RS485Up_DataSend(uint8 *Data,uint32 n);



extern void pthread_RS485UpDeal(void);










#endif  //_RS485UP_H_

