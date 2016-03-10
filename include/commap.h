/*
  *******************************************************************************
  * @file    commap.h
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

#ifndef	_COM_MAP_H_
#define	_COM_MAP_H_




//����һЩ���еĳ���
#define ZIGBEE_SEND_BYTES			100
#define GPRS_REC_QUEUES_LENGTH		1000000
#define USART3_REC_QUEUE_AT_LENGTH   	1000000
#define USART3_REC_QUEUE_ATIPD_LENGTH   1000000
#define MBUS_REC_QUEUES_LENGTH		1024


#define RECV_TEMPBUF_SIZE	1024  //����GPRS\����485�Ƚ�����λ����Ϣ����ʱ���������С��
#define SEND_TEMPBUF_SIZE	1024  //����GPRS\����485������λ��������Ϣ����ʱ���������С��


//����ͨ���豸ID ����Щ������ֱֵ����Ϊȫ��������±�ʹ�ã���˲�������޸�
#define    UP_COMMU_DEV_ZIGBEE	0
#define	 UP_COMMU_DEV_485		1
#define	 UP_COMMU_DEV_232		2
#define	 UP_COMMU_DEV_GPRS   	3
#define    UP_COMMU_DEV_AT		4
#define    UP_COMMU_DEV_ATIPD		5
#define    DOWN_COMMU_DEV_485	    	6
#define    DOWN_COMMU_DEV_MBUS    	7

#define	 UP_COMMU_DEV_ARRAY		8  //ָ����������ͨѶ�豸��������


//============================
extern  sem_t  QueueSems[];
extern  sem_t  UpRecQueSem_Gprs;
extern  sem_t  USART3RecQueSem_At;
extern  sem_t  USART3RecQueSem_AtIPD;
extern  sem_t  UpRecQueSem_RS485UP;
extern  sem_t  Sequence_XML_sem;
extern  sem_t  Result_XML_sem;
extern  sem_t  MBUSRec_sem;		
extern  sem_t  Down485Rec_sem;



extern uint8 *pQueues[UP_COMMU_DEV_ARRAY];
extern uint8 UpRecQueue_Gprs[GPRS_REC_QUEUES_LENGTH];
extern uint8 USART3RecQueue_At[USART3_REC_QUEUE_AT_LENGTH];
extern uint8 USART3RecQueue_AtIPD[USART3_REC_QUEUE_ATIPD_LENGTH];
extern uint8 UpRecQueue_RS485UP[GPRS_REC_QUEUES_LENGTH];
extern uint8 DownRecQueue_RS485[MBUS_REC_QUEUES_LENGTH];
extern uint8 DownRecQueue_MBUS[MBUS_REC_QUEUES_LENGTH];



extern uint8 *pRecvTempBuf[UP_COMMU_DEV_ARRAY];  //������ʱ�����Ҫ��������ݡ�
extern uint8 gu8RecvTempBuf_GPRS[RECV_TEMPBUF_SIZE];
extern uint8 gu8RecvTempBuf_RS485UP[RECV_TEMPBUF_SIZE];

extern uint8 *pSendTempBuf[UP_COMMU_DEV_ARRAY];



//============================================================================

extern uint8 UpcommapInit(void);
extern uint8 UpGetch(uint8 dev, uint8* data, uint16 OutTime);
extern uint8 UpQueueFlush(uint8 dev);
extern uint8 UpRecQueueWrite(uint8 dev,uint8* buf,uint32 n);
extern void UpDevSend(uint8 dev,uint8 *Data, uint32 n);

extern void GprsSend(int32 fd,uint8 *Data,uint32 n);

extern void DownDevSend(uint8 dev,uint8* buf,uint32 n);
extern uint8 DownDevGetch(uint8 dev,uint8* data,uint16 OutTime);


#endif  //_COM_MAP_H_
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
