/*
  *******************************************************************************
  * @file    gprs.h
  * @author  zjjin
  * @version V0.0.0
  * @date    03-14-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/  

#ifndef _GPRS_H_
#define _GPRS_H_

#include "includes.h"





typedef struct {
	uint8  ModuId;//ģ������
	uint8  Ready;//ģ���Ƿ�׼���ã�TRUE or FALSE
	uint8  Connect;//ģ���Ƿ�ɹ�������λ����TRUE or FALSE
	uint8  SocketType;//0:TCP;1:UDP
	uint8  IpCloseNum;//TRUE or FALSE 
	uint8  IpSendFailNum;
	uint8  IpSendRemnBufNum;//��һ��IPSENDʱGPRSģ���ʣ�໺��������
	uint8  Csq;//�ź�ǿ��
	
	uint32 FrmRecTimes;//����֡���� //���Թ۲���
	uint32 FrmSndTimes;//����֡���� //���Թ۲���
	
	uint32 FrmRecBs;//�����ֽ���
	uint32 FrmSndBs;//�����ֽ���
	
	uint32  RecDog; //����GPRS���յĿ��Ź�
	uint32  SndDog; //����GPRS���͵Ŀ��Ź�
} GPRS_RUN_STA;

typedef struct {
	uint8   GprsGvc;		// TRUE OR  FALSE��ΪTRUEʱ����ʾ��GPRS�Ĺ������漸��������Ч
    uint8   Device;			// �������ݵ��豸��GPRS, Zigbee, RS485
    uint16  ByteNum;		// �����ֽ���
    uint8   buff[1024];		//��������
} UP_COMM_MSG;



#define MODU_ID_UNKNOWN		0x55
#define MODU_ID_HW			0x12
#define MODU_ID_XMZ			0x21


#define	HEAD_MAX_LEN    64

#define	SOCKET_TCP	 0
#define	SOCKET_UDP	 1

#define  GPRS_CHECK_CYCLE	 	10 //��λ�롣
#define  GPRS_SELF_CHECK_CYCLE	 5*GPRS_CHECK_CYCLE
#define  GPRS_HEART_FRM_TIME		60
#define  GPRS_HEART_FRM_REC_OUTTIME	GPRS_HEART_FRM_TIME*4







#define MAX_IPSEND_GTM900C_BYTENUM 1024  //GTM900Cһ�η���(AT%IPSEND)���Ϊ1024�ֽ�(��������2048�������ַ�)
#define MAX_IPDATA_GTM900C_BYTENUM	2048  
//�������Ƿ���900C���յ�IPDATA�󷢹�����������1072�ֽ�(���˻ᰴ1072��ɶ��IPDATA������),�������ַ��ֻ���ִ���1072�����
//�������ֵ�㲻��,�ֽڶ�����һ����ֵ,ԭ��900B�����ǰ�1400�����

#define MAX_IPSEND_MC52i_BYTENUM	1500  //MC52iһ�η���(AT^SISW)���Ϊ1500�ֽ�,�����Ƹ�ʽ����
#define MAX_REC_MC52i_BYTENUM		1500  //MC52iһ�����ɶ��������������Ҳ��1500�ֽ�,�����������Լ�һ����1500��Ķ�û�������,��CPUȥҪ







extern UP_COMM_MSG gUpCommMsg;
extern uint8 gGprsFirst;
extern GPRS_RUN_STA gGprsRunSta;



extern int pthread_GPRS_Mana(void);





#endif  //_GPRS_H_

