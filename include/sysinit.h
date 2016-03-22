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




//����������Ϣ�ṹ�塣
typedef struct
{
	uint16 netType; 	 //�������ͣ�0-GPRS���磬1-485���硣
	uint16 collectMode;  //�Զ�����ʽ��0-����collectCycle�趨�����Զ�����1-����ʱ��ڵ㳭��.
	uint16 collectCycle; //�Զ�������������,��λ����.
	uint16 reportMode;   //���������ϱ���ʽ��0-������Զ����ƣ�1-��λ������ȡ����
	uint16 heartBeatCycle;//�������ڣ���λ���ӣ���Χ1-10���ӿ����á�
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

