/*
  *******************************************************************************
  * @file    sysinit.c 
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
#include "uart_set.h"
#include "commap.h"
#include "sysinit.h"
#include "xml.h"
#include "bsp.h"






// ȫ���ļ�������
int32 g_uiIoControl = 0;    //IO�ܽſ���
int32 g_uiRS4851Fd = 0;
int32 g_uiRS4852Fd = 0;
int32 g_uiGprsFd = 0;
int32 g_uiMbusFd = 0;








/*
  ******************************************************************************
  * �������ƣ� sysinit(void)
  * ˵    ���� ϵͳ��ʼ������
  * ��    ���� ��
  ******************************************************************************
*/

void sysinit(void)
{
	uint8 lu8ret= 0;

	sem_Init();
	UpcommapInit();
	
	lu8ret = XMLBuf_Init();
	if(lu8ret == NO_ERR)
		printf("XMLBuf_Init OK.\n");
	
	lu8ret = QueuesInit();
	if(lu8ret == NO_ERR)
		printf("QueuesInit OK!\n");
	else
		printf("Error,in QueuesInit!\n");

	lu8ret = open_IOControl();
	if(lu8ret == NO_ERR)
		printf("open_IOControl OK!\n");
	else
		printf("Error,in open_IOControl!\n");

	
	sqldb_init();





}




/*
  ******************************************************************************
  * �������ƣ� uint8 QueuesInit(void)
  * ˵    ���� ϵͳ��ʼ������
  * ��    ���� ��
  ******************************************************************************
*/

uint8 QueuesInit(void)
{

	if (QueueCreate((void *)UpRecQueue_RS485UP,sizeof(UpRecQueue_RS485UP), NULL,NULL) == NOT_OK){
		return ERR_1;
  	}

	if (QueueCreate((void *)UpRecQueue_Gprs,sizeof(UpRecQueue_Gprs), NULL,NULL) == NOT_OK){
  		return ERR_1;
  	}
  	if (QueueCreate((void *)USART3RecQueue_At,sizeof(USART3RecQueue_At),NULL,NULL) == NOT_OK){
		return ERR_1;
    	}
    
  	if (QueueCreate((void *)USART3RecQueue_AtIPD,sizeof(USART3RecQueue_AtIPD),NULL,NULL) == NOT_OK){
		return ERR_1;
    	}

	if (QueueCreate((void *)DownRecQueue_RS485,sizeof(DownRecQueue_RS485),NULL,NULL) == NOT_OK){
		return ERR_1;
    	}

	if (QueueCreate((void *)DownRecQueue_MBUS,sizeof(DownRecQueue_MBUS),NULL,NULL) == NOT_OK){
		return ERR_1;
    	}
	

	return NO_ERR;

}

/*
  ******************************************************************************
  * �������ƣ� uint8 open_IOControl(void)
  * ˵    ���� ��IO��������
  * ��    ���� ��
  ******************************************************************************
*/

uint8 open_IOControl(void)
{

	g_uiIoControl = open(IO_CONTROL, O_RDWR, 0);
	if (g_uiIoControl < 0){
		//printf("CONTROL_IO open error ....[%s][%s][%d] \n",FILE_LINE); 
		return ERR_1;
	}

	return NO_ERR;
}



/*
  ******************************************************************************
  * �������ƣ� void sem_Init(void)
  * ˵    ���� �����ź����ĳ�ʼ����
  * ��    ���� ��
  ******************************************************************************
*/

void sem_Init(void)
{
	int ret=0;
	
	ret = sem_init(&CRITICAL_sem,0,1);
	if(ret != 0){
	   perror("sem_init\n");
	}

	ret = sem_init(&GprsIPDTask_sem,0,1);  //Ĭ��GprsIPD�̲߳����У���GPRS���罨���ú�����С�
	if(ret != 0){
	   perror("sem_init\n");
	}

	ret = sem_init(&Gprs_Sem,0,1);  //Ĭ��GprsIPD�̲߳����У���GPRS���罨���ú�����С�
	if(ret != 0){
	   perror("sem_init\n");
	}
	
	ret = sem_init(&RS485Up_sem,0,1);
	if(ret != 0){
	   perror("sem_init\n");
	}

	ret = sem_init(&RS485Down_sem,0,1);
	if(ret != 0){
	   perror("sem_init\n");
	}

	ret = sem_init(&GPRSPort_sem,0,1);
	if(ret != 0){
	   perror("sem_init\n");
	}

	ret = sem_init(&OperateDB_sem,0,1);
	if(ret != 0){
   		perror("sem_init\n");
	}

	ret = sem_init(&OperateMBUS_sem,0,1);
	if(ret != 0){
   		perror("sem_init\n");
	}

	ret = sem_init(&Opetate485Down_sem,0,1);
	if(ret != 0){
   		perror("sem_init\n");
	}



 	ret = sem_init(&UpRecQueSem_Gprs,0,0);
	if(ret != 0)
		perror("UpRecQueSem_Gprs  error\n");
	
	 ret = sem_init(&USART3RecQueSem_At,0,0);
	if(ret != 0)
		perror("UpRecQueSem_Gprs  error\n");

	 ret = sem_init(&USART3RecQueSem_AtIPD,0,0);
	if(ret != 0)
		perror("UpRecQueSem_Gprs  error\n");

	ret = sem_init(&UpRecQueSem_RS485UP,0,0);
	if(ret != 0)
		perror("UpRecQueSem_Gprs  error\n");

	ret = sem_init(&Sequence_XML_sem,0,0);
	if(ret != 0)
		perror("UpRecQueSem_Gprs  error\n");

	ret = sem_init(&Result_XML_sem,0,0);
	if(ret != 0)
		perror("UpRecQueSem_Gprs  error\n");

	ret = sem_init(&MBUSRec_sem,0,0);
	if(ret != 0)
		perror("UpRecQueSem_Gprs  error\n");

	ret = sem_init(&Down485Rec_sem,0,0);
	if(ret != 0)
		perror("UpRecQueSem_Gprs  error\n");
	


	



}


/*
  ******************************************************************************
  * �������ƣ� void sqldb_init(void)
  * ˵    ���� ���ݿ�������ݳ�ʼ��
  * ��    ���� ��
  ******************************************************************************
*/

void sqldb_init(void)
{
	char lcSqlRetArry[100];
	uint8 lu8tmp = 0;

	open_db();
	read_meter_info(lcSqlRetArry);  //�����ݿ��еı���Ϣ��ʼ�����ڴ档
	
	lu8tmp = strlen(lcSqlRetArry);
	if(lu8tmp > 0){
		printf("read_meter_info Err is %s .\n",lcSqlRetArry);
		return;
	}
	read_all_request_data(lcSqlRetArry);
	if(lu8tmp > 0){
		printf("read_meter_info Err is %s .\n",lcSqlRetArry);
		return;
	}
	printf("get_meter_info_cnt ret = %d.\n",get_meter_info_cnt());



	//���������ơ�



}







