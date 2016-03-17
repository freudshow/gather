/*
  *******************************************************************************
  * @file    uart_gprs.c
  * @author  zjjin
  * @version V0.0.0
  * @date    03-09-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/ 
#include "includes.h"
#include "uart_gprs.h"
#include "uart_set.h"
#include "commap.h"
#include "sysinit.h"
#include "xml.h"



/*
  ******************************************************************************
  * 函数名称： int pthread_UartGprs_Rec(void)
  * 说    明： GPRS接口接收数据处理线程。
  * 参    数： 无
  ******************************************************************************
*/

int pthread_UartGprs_Rec(void)
{
	
	//上行485口初始化
	g_uiGprsFd = open_com_dev(DEVICEGPRS);
	if (set_com_para(g_uiGprsFd,115200,8,1,'N') == FALSE) { 
 
	}
	else
		printf("GPRS com parameter set OK.\n");	

	

	//接收数据。
 	 while(1){
    		UartGprs_Rec();
    	 }
	 
  	return(CONTROL_RET_SUC);
	
}



/*
******************************************************************************
* 函数名称： INT32 UartGprs_Rec(void)
* 说	明： 
* 参	数： 无
******************************************************************************
*/

INT32 UartGprs_Rec(void)
{
    uint8 lu8data = 0;
    fd_set ReadSetFD;
    struct timeval stTimeVal;
    uint8 ret;
    
    while(1)
    {
        FD_ZERO(&ReadSetFD);
        FD_SET(g_uiGprsFd, &ReadSetFD);
        /* 设置等待的超时时间 */
        stTimeVal.tv_sec = RS485UP_REC_TIMEOUT_SEC;
        stTimeVal.tv_usec = RS485UP_REC_TIMEOUT_USEC;
        
        if (select(g_uiGprsFd+1, &ReadSetFD, NULL, NULL,&stTimeVal) > 0){
		   if (FD_ISSET(g_uiGprsFd, &ReadSetFD)){
				if(read(g_uiGprsFd, &lu8data, 1) == 1){
					ret = QueueWrite((void*)USART3RecQueue_At, lu8data);
					if(ret != QUEUE_OK){
						//printf("[%s][%s][%d] ret=%d queue full\n",FILE_LINE,ret);
					}
					OSSemPost(UP_COMMU_DEV_AT);
					
					ret = QueueWrite((void*)USART3RecQueue_AtIPD, lu8data);
					if(ret != QUEUE_OK){									
						//printf("[%s][%s][%d] ret=%d queue full	The Queue has data Num is %d\n",FILE_LINE,ret, QueueNData(USART3RecQueue_AtIPD));
					}				
					OSSemPost(UP_COMMU_DEV_ATIPD);

					//printf("%c \n",lu8data);  //测试用

		   
				}
		    }

        }
	   

    }
    
  	return(CONTROL_RET_FAIL);
	
}






