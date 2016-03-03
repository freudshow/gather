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




uint8 gDebugLevel = 1;
uint8 gDebugModule[20];
sem_t  CRITICAL_sem;
sem_t  RS485Up_sem;  //上行485端口信号量
sem_t  RS485Down_sem;//下行485端口信号量
sem_t  GPRSPort_sem; //GPRS端口信号量







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
    
    sem_wait(&CRITICAL_sem);	/*减一*/
}


void OS_EXIT_CRITICAL(void)
{
    int ret=0,value=0;
    ret = sem_getvalue(&CRITICAL_sem,&value);
    if(ret){
      	printf("[%s][%s][%d]value=%d \n",FILE_LINE,ret);  
    }
    debug_debug(gDebugModule[MSIC_MODULE],"[%s][%s][%d]value=%d \n",FILE_LINE,value); 
    
    sem_post(&CRITICAL_sem);  /*加一*/
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
    sem_post(&QueueSems[dev]);  /*加一*/
}



/*
  ******************************************************************************
  * 函数名称： void misc_init(void)
  * 说    明： 杂项初始化处理。
  * 参    数： 无
  ******************************************************************************
  */

void misc_init(void)
{
	int ret=0;
	ret = sem_init(&CRITICAL_sem,0,1);
	if(ret != 0) {
	   	perror("sem_init CRITICAL_sem.\n");
	}




}


