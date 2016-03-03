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





// 全局文件描述符
int32 g_uiIoControl = 0;    //IO管脚控制
int32 g_uiRS4851Fd = 0;
int32 g_uiRS4852Fd = 0;
int32 g_uiGprsFd = 0;







/*
  ******************************************************************************
  * 函数名称： sysinit(void)
  * 说    明： 系统初始化函数
  * 参    数： 无
  ******************************************************************************
*/

void sysinit(void)
{
	uint8 lu8ret= 0;
	//COM_INFO_T  l_RS4851DeviceInfo;

	sem_Init();
	UpcomInit();
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




}




/*
  ******************************************************************************
  * 函数名称： uint8 QueuesInit(void)
  * 说    明： 系统初始化函数
  * 参    数： 无
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
	

	return NO_ERR;

}

/*
  ******************************************************************************
  * 函数名称： uint8 open_IOControl(void)
  * 说    明： 打开IO控制驱动
  * 参    数： 无
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
  * 函数名称： void sem_Init(void)
  * 说    明： 所有信号量的初始化。
  * 参    数： 无
  ******************************************************************************
*/

void sem_Init(void)
{
	int ret=0;
	
	ret = sem_init(&CRITICAL_sem,0,1);
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



}






