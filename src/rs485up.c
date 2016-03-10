/*
  *******************************************************************************
  * @file    rs485up.c
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
#include "rs485up.h"
#include "uart_set.h"
#include "commap.h"
#include "sysinit.h"
#include "xml.h"



/*
  ******************************************************************************
  * �������ƣ� int pthread_RS485up_Rec()
  * ˵    ���� ����485�ӿڽ������ݴ����̡߳�
  * ��    ���� ��
  ******************************************************************************
*/

int pthread_RS485up_Rec(void)
{
	
	//����485�ڳ�ʼ��
	g_uiRS4851Fd = open_com_dev(DEVICE4851);
	if (set_com_para(g_uiRS4851Fd,RS485UP_COM_SPEED,8,1,'N') == FALSE) { 
 
	}
	else
		printf("RS485Up com parameter set OK.\n");	
  	

	ioctl(g_uiIoControl,RS4851_C30,RS485_RECV);  //485����Ϊ�������ݡ�
	usleep(1000);

	//�������ݡ�
 	 while(1){
    		RS485up_Rec();
    	 }
	 
  	return(CONTROL_RET_SUC);
	
}



/*
******************************************************************************
* �������ƣ� INT32 RS485up_Rec(void)
* ˵	���� 
* ��	���� ��
******************************************************************************
*/

INT32 RS485up_Rec(void)
{
    uint8 lu8data = 0;
    fd_set ReadSetFD;
    struct timeval stTimeVal;
    uint8 ret;
    
    while(1)
    {
        FD_ZERO(&ReadSetFD);
        FD_SET(g_uiRS4851Fd, &ReadSetFD);
        /* ���õȴ��ĳ�ʱʱ�� */
        stTimeVal.tv_sec = RS485UP_REC_TIMEOUT_SEC;
        stTimeVal.tv_usec = RS485UP_REC_TIMEOUT_USEC;
        
        if (select(g_uiRS4851Fd+1, &ReadSetFD, NULL, NULL,&stTimeVal) > 0){
		   if (FD_ISSET(g_uiRS4851Fd, &ReadSetFD)){
				if(read(g_uiRS4851Fd, &lu8data, 1) == 1){
					ret = QueueWrite((void*)UpRecQueue_RS485UP, lu8data);
					if(ret != QUEUE_OK){
						//printf("[%s][%s][%d] ret=%d queue full\n",FILE_LINE,ret);
					}
		   
					OSSemPost(UP_COMMU_DEV_485);
		   
				}
		    }

        }
	   

    }
    
  	return(CONTROL_RET_FAIL);
	
}


/*
******************************************************************************
* �������ƣ�void RS485Up_FileSend(uint8 Dev, FILE *fp)
* ˵	���� ͨ��485���ж˿ڣ�����һ���ļ����ݡ�
* ��	���� 
******************************************************************************
*/
void RS485Up_FileSend(uint8 Dev, FILE *fp)
{
	int	i = 0;
	uint8 err = 0;
	int32 l32FileSize = 0;
	uint32 lu32SendNum = 0;

	sem_wait(&RS485Up_sem);

	//���Ƚ�485��������Ϊsend.
	ioctl(g_uiIoControl,RS4851_C30,RS485_SED);
	usleep(1000);
	
	//�����ļ���С��
	fseek(fp,0,2);			//λ��ָ�뵽�ļ�ĩβ
	l32FileSize = ftell(fp);	//�����ļ���С
	fseek(fp,0,0);			//λ��ָ�뵽�ļ���ʼ
	lu32SendNum = SEND_TEMPBUF_SIZE;

	for(i=0;i<(l32FileSize/SEND_TEMPBUF_SIZE + 1);i++){
		if(i == l32FileSize/SEND_TEMPBUF_SIZE){ //���һ��
			lu32SendNum = l32FileSize % SEND_TEMPBUF_SIZE;
		}
		
		memset(pSendTempBuf[Dev],0,sizeof(pSendTempBuf));
		fread(pSendTempBuf[Dev],1,lu32SendNum,fp);
                 //debug_info(gDebugModule[GPRS_MODULE], "send xml:  %s\n",gGPRSSendBuf);
		       // debug_info(gDebugModule[GPRS_MODULE], "INFO: [TaskUpSend] SendNum=%d\n",SendNum);
  		err = RS485Up_DataSend(pSendTempBuf[Dev],lu32SendNum);
		if(err != NO_ERR){

		}

	}

	//���485��������Ϊrecv.
	ioctl(g_uiIoControl,RS4851_C30,RS485_RECV);
	usleep(1000);

	sem_post(&RS485Up_sem);


}



/*
******************************************************************************
* �������ƣ� uint8 RS485Up_DataSend(uint8 *Data,uint32 n)
* ˵	���� ����485�ӿڷ������ݡ�
* ��	���� 
******************************************************************************
*/
uint8 RS485Up_DataSend(uint8 *Data,uint32 n)
{
	uint32 lu32delaytimes = 0;

	write(g_uiRS4851Fd, (uint8 *)Data, n);

	lu32delaytimes = 12000000 / RS485UP_COM_SPEED;  //1000000*12/RS485UP_COM_SPEED;
	lu32delaytimes = lu32delaytimes*n;  //�����������ΪRS485UP_COM_SPEEDʱ������n���ֽڴ����Ҫʱ�䡣

	usleep(lu32delaytimes); //��֤�������ݷ��ꡣ
	

	return NO_ERR; 

}







/*
******************************************************************************
* �������ƣ� void pthread_RS485UpDeal(void)
* ˵	���� ����485ͨ�������̡߳�
* ��	���� 
******************************************************************************
*/

void pthread_RS485UpDeal(void)
{
	uint8 err = 0;
	//uint8 lu8data = 0;
	uint16 lu16outtime = 1000;  //�ȴ� ��������
	uint8 lu8xmlIndex = 0;
	FILE *fp;

	while(1){	
		do{
			lu8xmlIndex = Get_XMLBuf();  //��ȡһ��xml�ݴ�ռ�,���һ��Ҫ�ͷŸÿռ䣬��ȡ-ʹ��-�ͷš�
		}while(lu8xmlIndex == ERR_FF);

		//printf("Get_XMLBuf %d.\n",lu8xmlIndex);
		
		err = UpGetXMLStart(lu8xmlIndex,UP_COMMU_DEV_485,lu16outtime);
		if(err == NO_ERR){
			printf("UpGetXMLStart OK.\n");
			err = UpGetXMLEnd(lu8xmlIndex,UP_COMMU_DEV_485,lu16outtime);
			if(err == NO_ERR){//˵�����յ�һ֡������xml���ݡ�
				printf("UpGetXMLEnd OK.\n");
				//��Ӧ����
				fp = fopen(gXML_File[lu8xmlIndex].pXMLFile,"r");
				RS485Up_FileSend(UP_COMMU_DEV_485,fp);

				fclose(fp);
				
				Put_XMLBuf(lu8xmlIndex);  //�ͷű�ռ�õ�xml�ݴ档

			}
			else{
				Put_XMLBuf(lu8xmlIndex);  //�ͷű�ռ�õ�xml�ݴ档
			}

		}
		else{
			Put_XMLBuf(lu8xmlIndex);  //�ͷű�ռ�õ�xml�ݴ档
		}		


	}


}







  

