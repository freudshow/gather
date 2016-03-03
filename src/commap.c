/*
  *******************************************************************************
  * @file    commap.c 
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

#include "includes.h"
#include "commap.h"
#include "xml.h"
#include "sysinit.h"

//����IPDATA�����������Ϸ�����,���ڶ���û�������˵ȴ����Ƕ���,���ﶨ��ıȽϴ�

uint8 *pQueues[UP_COMMU_DEV_ARRAY];
uint8 UpRecQueue_Gprs[GPRS_REC_QUEUES_LENGTH];
uint8 USART3RecQueue_At[USART3_REC_QUEUE_AT_LENGTH];
uint8 USART3RecQueue_AtIPD[USART3_REC_QUEUE_ATIPD_LENGTH];
uint8 UpRecQueue_RS485UP[GPRS_REC_QUEUES_LENGTH];



sem_t  QueueSems[UP_COMMU_DEV_ARRAY];
sem_t  UpRecQueSem_Gprs;
sem_t  USART3RecQueSem_At;
sem_t  USART3RecQueSem_AtIPD;
sem_t  UpRecQueSem_RS485UP;
sem_t  Sequence_XML_sem;
sem_t  Result_XML_sem;


uint8 *pRecvTempBuf[UP_COMMU_DEV_ARRAY];  //������ʱ�����Ҫ��������ݡ�
uint8 gu8RecvTempBuf_GPRS[RECV_TEMPBUF_SIZE];
uint8 gu8RecvTempBuf_RS485UP[RECV_TEMPBUF_SIZE];


uint8 *pSendTempBuf[UP_COMMU_DEV_ARRAY];  //������ʱ�����Ҫ��������ݡ�
uint8 gu8SendTempBuf_GPRS[SEND_TEMPBUF_SIZE];
uint8 gu8SendTempBuf_RS485UP[SEND_TEMPBUF_SIZE];





uint8 UpcomInit(void)
{
	pQueues[UP_COMMU_DEV_GPRS] 		= UpRecQueue_Gprs;
	pQueues[UP_COMMU_DEV_AT]			= USART3RecQueue_At;
	pQueues[UP_COMMU_DEV_ATIPD]		= USART3RecQueue_AtIPD;
	pQueues[UP_COMMU_DEV_485] 		= UpRecQueue_RS485UP;
  
	QueueSems[UP_COMMU_DEV_GPRS] 		= UpRecQueSem_Gprs;
	QueueSems[UP_COMMU_DEV_AT]		= USART3RecQueSem_At;
	QueueSems[UP_COMMU_DEV_ATIPD]		= USART3RecQueSem_AtIPD;
	QueueSems[UP_COMMU_DEV_485] 		= UpRecQueSem_RS485UP;
  	QueueSems[SEQUEUE_XML]			= Sequence_XML_sem;
  	QueueSems[RESULT_XML]			= Result_XML_sem;

	pRecvTempBuf[UP_COMMU_DEV_GPRS] 	= gu8RecvTempBuf_GPRS;
	pRecvTempBuf[UP_COMMU_DEV_485] 	= gu8RecvTempBuf_RS485UP;

	pSendTempBuf[UP_COMMU_DEV_GPRS] 	= gu8SendTempBuf_GPRS;
	pSendTempBuf[UP_COMMU_DEV_485] 	= gu8SendTempBuf_RS485UP;
	
  	return 0;
}

/*
  ******************************************************************************
  * �������ƣ� UpGetch(uint8 dev, uint8* data, uint16 OutTime)
  * ˵    ���� 
  * ��    ���� OutTime,�ȴ���ʱ��������
  ******************************************************************************
*/

uint8 UpGetch(uint8 dev, uint8* data, uint16 OutTime)
{
	uint8 err = 0x00;
	uint8 ret;
	while((ret=QueueRead(data, (void*)pQueues[dev])) != QUEUE_OK){	
         //printf("[%s][%s][%d] ret = %d QUEUE_EMPTY \n",FILE_LINE,ret);
   		OSSemPend(dev, (uint32)OutTime, &err);
		if(err != OS_ERR_NONE){
			//debug_err(gDebugModule[GPRS_MODULE],"[%s][%s][%d]OSSemPend err=%d\n",FILE_LINE,err);
			return err;
		}
    		else{
    			//printf("[%s][%s][%d] err = %d\n",FILE_LINE,err);
   		}
	}
	
	//printf("%c",*data);
	
	return NO_ERR;
}



uint8 UpQueueFlush(uint8 dev)
{
	QueueFlush((void*)pQueues[dev]);
	
    	return 0;
}


/*
read n betys buf to dev
*/
//д���н��ն���,˵��: �������еĲ��������ô˺�������,����UART3���ն��е�д����û�е��ñ�����
uint8 UpRecQueueWrite(uint8 dev,uint8* buf,uint32 n) //��������
{
	uint32 i;
	
	for(i=0;i<n;i++){ 
	  	QueueWrite((void*)pQueues[dev],buf[i]);
	}
	OSSemPost(dev);
	
	return 0;
}



void UpDevSend(uint8 dev,uint8 *Data, uint32 n) 		/*��������*/
{
	
	switch (dev){
		case UP_COMMU_DEV_ZIGBEE:	//�����Zigbee�������ݣ�ÿ��100���ֽڣ�ÿ�η��ͼ��333MS
			
			break;
		case UP_COMMU_DEV_485:
						
			break;
		
		case UP_COMMU_DEV_232:
			
			break;
			
		case UP_COMMU_DEV_GPRS:
			GprsSend(g_uiGprsFd,Data,n);
			break;
		
		
		default :
			while(1);
	}
	
}



void GprsSend(int32 fd,uint8 *Data,uint32 n)
{  
	//int stat;
  	OS_ENTER_CRITICAL();
  	//stat = write(fd, Data, n);
  	write(fd, Data, n);
   	OS_EXIT_CRITICAL();
   
  //printf("[%s][%s][%d] stat = %d \n",FILE_LINE,stat);
    
}


void DownDevSend(uint8 dev,uint8* buf,uint32 n)
{
	switch (dev){
		case DOWN_COMM_DEV_MBUS:
			//USART2Send(buf, n); 							//MBUS����---��������
			break;
     
      	case DOWN_COMM_DEV_485:
			//UART5Send(buf, n); 					//485����
			break;
    
		default :
			while(1);
			
	}
	
}

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
