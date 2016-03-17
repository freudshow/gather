/*
  *******************************************************************************
  * @file    sysfuctions.c 
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
#include "sysfuctions.h"
#include "commap.h"
#include "rs485up.h"





uint8 gDebugLevel = 1;
uint8 gDebugModule[20];
sem_t  CRITICAL_sem;
sem_t  GprsIPDTask_sem;//����GprsIPD�߳��Ƿ����е��ź���(��ûʹ�������̹߳���ķ�����)��
sem_t  Gprs_Sem;
sem_t  RS485Up_sem;  //����485�˿��ź���
sem_t  RS485Down_sem;//����485�˿��ź���
sem_t  GPRSPort_sem; //GPRS�˿��ź���
sem_t  OperateDB_sem; //�������ݿ���ź���
sem_t  OperateMBUS_sem;	//����MBUS�˿��ź���
sem_t  Opetate485Down_sem;	//����485���ж˿��ź�����

sem_t Validate_sem;  //GPRS��¼�ź�����





static XmlInfoRecord gXmlInfo[UP_COMMU_DEV_ARRAY];  //���ڷֱ��¼ÿ���豸�µ�xml��Ϣ��





void OSTimeDly(uint32 ms)
{
    usleep(ms*1000);
}


void OS_ENTER_CRITICAL(void)
{
    int ret,value=0;
    ret = sem_getvalue(&CRITICAL_sem,&value);
    if(ret)  {
      	debug_err(gDebugModule[MSIC_MODULE],"[%s][%s][%d]value=%d \n",FILE_LINE,ret);  
    }
    debug_debug(gDebugModule[MSIC_MODULE],"[%s][%s][%d]value=%d \n",FILE_LINE,value);    
    
    sem_wait(&CRITICAL_sem);	/*��һ*/
}


void OS_EXIT_CRITICAL(void)
{
    int ret=0,value=0;
    ret = sem_getvalue(&CRITICAL_sem,&value);
    if(ret){
      	printf("[%s][%s][%d]value=%d \n",FILE_LINE,ret);  
    }
    debug_debug(gDebugModule[MSIC_MODULE],"[%s][%s][%d]value=%d \n",FILE_LINE,value); 
    
    sem_post(&CRITICAL_sem);  /*��һ*/
}



void  OSSemPend (uint8 dev,uint32  timeout,uint8 *perr)
{
    int ret=0;
    struct timespec outtime;
    struct timeval now;

    gettimeofday(&now,NULL);
    
    outtime.tv_sec = now.tv_sec + timeout/1000;
    outtime.tv_nsec = now.tv_usec + (timeout%1000)*1000*1000;
    
    outtime.tv_sec += outtime.tv_nsec/(1000*1000*1000);
    outtime.tv_nsec %= (1000*1000*1000);
    
    if(timeout == (uint32)NULL){
      	sem_wait(&QueueSems[dev]);
    }
    else{
    		ret = sem_timedwait(&QueueSems[dev],&outtime);
		if(ret != 0){
          	debug_err(gDebugModule[MSIC_MODULE],"[%s][%s][%d]sem_timedwait %s\n",FILE_LINE,strerror(errno));
        	}
    }
    
  	*perr = (uint8)ret;
	
}


void OSSemPost(uint8 dev)
{
    sem_post(&QueueSems[dev]);  /*��һ*/
}


/*
  ******************************************************************************
  * �������ƣ� int OSSem_timedwait (sem_t Sem,uint32  timeout)
  * ˵    ���� �ɶ�ʱ�ȴ�һ���ź����ĵ�����
  * ��    ���� �ɹ��ȴ����ź���������0����0˵����ʱ��
  			sem_t Sem Ҫ�ȴ����ź�����
  			uint32  timeout ��ʱʱ�䣬��λ���롣
  ******************************************************************************
*/

int OSSem_timedwait(sem_t *pSem,uint32  timeout)
{
    int ret=0;
    struct timespec outtime;
    struct timeval now;

    gettimeofday(&now,NULL);
    
    outtime.tv_sec = now.tv_sec + timeout/1000;
    outtime.tv_nsec = now.tv_usec + (timeout%1000)*1000*1000;
    
    outtime.tv_sec += outtime.tv_nsec/(1000*1000*1000);
    outtime.tv_nsec %= (1000*1000*1000);
    
    if(timeout == (uint32)NULL){
      	sem_wait(pSem);
    }
    else{
    		ret = sem_timedwait(pSem,&outtime);
		if(ret != 0){
          	debug_err(gDebugModule[MSIC_MODULE],"[%s][%s][%d]sem_timedwait %s\n",FILE_LINE,strerror(errno));
        	}
    }

    return ret;
    
	
}





/*
  ******************************************************************************
  * �������ƣ� uint8 Str2Bin(char *str, uint8 *array,uint16 lens)
  * ˵    ���� ���ַ���ת��Ϊʮ����������
  * ��    ���� lensΪarrayָ��ռ���ֽ������Է�str�ϳ���array�ռ䲻����������⡣
  ******************************************************************************
*/
uint8 Str2Bin(char *str, uint8 *array,uint16 lens)
{
	char temp;
	char data;
	uint16 lu16counter = 0;

	int number = 0;
	char tempLow, tempHigh;

	number = strlen(str);
	//printf("[%s][%s][%d] number = %d\n",FILE_LINE,number);	
	if(number%2)
	{	
		number++;
		data = *str++;
		tempHigh = 0x00;
		tempLow  = Ascii2Hex(data);
		//printf("templow data is %x\n", tempLow);
		*array++ = (tempHigh << 4) + tempLow;
		//printf("creat array data is %x\n", (tempHigh << 4) + tempLow);
		lu16counter += 1;
	}
	
	//printf("The string 2 Hex data is: ");
	while(*str != '\0')
	{
		if(lu16counter < lens){
			data = *str++;
			temp = Ascii2Hex(data)<<4;
			data = *str++;
			temp += Ascii2Hex(data);

			*array++ = temp;
			//printf(" %x ,", temp);
		
			lu16counter += 1;
		}
		else{
			break;
		}
		
	}
	//printf("\n");
	return 0;

}


/*
  ******************************************************************************
  * �������ƣ� uint8 PUBLIC_CountCS(uint8* _data, uint16 _len)
  * ˵    ���� ����һ�����ֵ��ۼӺ�У�飬�������������
  * ��    ���� 
  ******************************************************************************
*/

uint8 PUBLIC_CountCS(uint8* _data, uint16 _len)
{
    uint8 cs = 0;
    uint16 i;
    
    for(i=0;i<_len;i++)
    {
       cs += *_data++;
    }
    
    return cs;    
}





/*
  ******************************************************************************
  * �������ƣ� uint8 setXmlInfo()
  * ˵    ���� �����豸�ţ�������Ӧ��gXmlInfo��Ϣ��
  * ��    ���� 
  ******************************************************************************
*/

uint8 setXmlInfo(uint8 dev,uint8 functype,uint8 opertype,uint8 app1,uint8 app2,uint8 app3,uint8 app4)
{

	if(dev >= UP_COMMU_DEV_ARRAY){
		printf("dev num error.\n");
		return ERR_1;
	}

	gXmlInfo[dev].FuncType = functype;
	gXmlInfo[dev].OperType = opertype;
	gXmlInfo[dev].appendix1 = app1;
	gXmlInfo[dev].appendix2 = app2;
	gXmlInfo[dev].appendix3 = app3;
	gXmlInfo[dev].appendix4 = app4;

	return NO_ERR;

}

/*
  ******************************************************************************
  * �������ƣ� uint8 getXmlInfo(uint8 dev,XmlInfoRecord *xmlInfo)
  * ˵    ���� �����豸��ţ���ȡ��Ӧ��xmlInfo��Ϣ��
  * ��    ���� 
  ******************************************************************************
*/

uint8 getXmlInfo(uint8 dev,XmlInfoRecord *xmlInfo)
{

	if(dev >= UP_COMMU_DEV_ARRAY){
		printf("dev num error.\n");
		return ERR_1;
	}

	memcpy((uint8 *)xmlInfo,(uint8 *)&gXmlInfo[dev].FuncType,sizeof(XmlInfoRecord));

	return NO_ERR;

}








/*
******************************************************************************
* �������ƣ�void FileSend(uint8 Dev, FILE *fp)
* ˵	���� ����һ���ļ����ݡ�
* ��	���� Dev����ѡ��ͨ��485����GPRS���͡�
******************************************************************************
*/
void FileSend(uint8 Dev, FILE *fp)
{	
	switch(Dev){
		case UP_COMMU_DEV_AT:
		case UP_COMMU_DEV_GPRS:
			GPRS_FileSend(UP_COMMU_DEV_GPRS,fp);
			break;

		case UP_COMMU_DEV_485:
			RS485Up_FileSend(UP_COMMU_DEV_485,fp);
			break;

		default:
			break;


	}

}


