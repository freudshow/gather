/*
  *******************************************************************************
  * @file    sysinit.h
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

#ifndef _SYSINIT_H_
#define _SYSINIT_H_


extern int32 g_uiIoControl;
extern int32 g_uiRS4851Fd;
extern int32 g_uiRS4852Fd;
extern int32 g_uiGprsFd;

extern void sysinit(void);
extern uint8 QueuesInit(void);
extern uint8 open_IOControl(void);
extern void sem_Init(void);






#endif  //_SYSINIT_H_

