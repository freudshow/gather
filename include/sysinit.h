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




//定义表基础信息结构体。
typedef struct
{
	uint16 netType; 	 //网络类型，0-GPRS网络，1-485网络。
	uint16 collectMode;  //自动抄表方式，0-按照collectCycle设定周期自动抄表，1-按定时间节点抄表.
	uint16 collectCycle; //自动抄表周期设置,单位分钟.
	uint16 reportMode;   //抄表数据上报方式，0-抄完表自动上推，1-上位机主动取数。
	uint16 heartBeatCycle;//心跳周期，单位分钟，范围1-10分钟可设置。
}SysConfig_ASC2HEX_T;



extern int32 g_uiIoControl;
extern int32 g_uiRS4851Fd;
extern int32 g_uiRS4852Fd;
extern int32 g_uiGprsFd;
extern int32 g_uiMbusFd;

extern SysConfig_ASC2HEX_T g_sysConfigHex;





extern void sysinit(void);
extern uint8 QueuesInit(void);
extern uint8 open_IOControl(void);
extern void sem_Init(void);
extern void sqldb_init(void);
extern void sysConfig_Ascii2hex(void);






#endif  //_SYSINIT_H_

