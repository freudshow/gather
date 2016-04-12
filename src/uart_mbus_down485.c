/*
  *******************************************************************************
  * @file    uart_mbus_down485.c
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
#include "uart_mbus_down485.h"
#include "uart_set.h"
#include "commap.h"
#include "sysinit.h"
#include "xml.h"



/*
  ******************************************************************************
  * 函数名称： int pthread_UartMbus_Rec(void)
  * 说    明： MBUS接口接收数据处理线程。
  * 参    数： 无
  ******************************************************************************
*/

int pthread_UartMbus_Rec(void)
{
	
	//MBUS端口初始化
	g_uiMbusFd = open_com_dev(DEVICEMBUS);
	if (set_com_para(g_uiMbusFd,2400,8,1,'E') == FALSE) { 
 
	}
	else
		printf("MBUS com parameter set OK.\n");	
  	

	//接收数据。
 	 while(1){
    		UartMbus_Rec();
    	 }
	 
  	return(CONTROL_RET_SUC);
	
}



/*
******************************************************************************
* 函数名称： int32 UartMbus_Rec(void)
* 说	明： 
* 参	数： 无
******************************************************************************
*/

int32 UartMbus_Rec(void)
{
    uint8 lu8data = 0;
    fd_set ReadSetFD;
    struct timeval stTimeVal;
    uint8 ret;
    
    while(1)
    {
        FD_ZERO(&ReadSetFD);
        FD_SET(g_uiMbusFd, &ReadSetFD);
        /* 设置等待的超时时间 */
        stTimeVal.tv_sec = REC_TIMEOUT_SEC;
        stTimeVal.tv_usec = REC_TIMEOUT_USEC;
        
        if (select(g_uiMbusFd+1, &ReadSetFD, NULL, NULL,&stTimeVal) > 0){
		   if (FD_ISSET(g_uiMbusFd, &ReadSetFD)){
				if(read(g_uiMbusFd, &lu8data, 1) == 1){
					ret = QueueWrite((void*)DownRecQueue_MBUS, lu8data);
					if(ret != QUEUE_OK){
						//printf("[%s][%s][%d] ret=%d queue full\n",FILE_LINE,ret);
					}
		   
					OSSemPost(DOWN_COMMU_DEV_MBUS);

					//printf("%x \n",lu8data);  //测试用
		   
				}
		    }

        }
	   

    }
    
  	return(CONTROL_RET_FAIL);
	
}



/*
  ******************************************************************************
  * 函数名称： int pthread_UartDown485_Rec(void)
  * 说    明： 下行485接口接收数据处理线程。
  * 参    数： 无
  ******************************************************************************
*/

int pthread_UartDown485_Rec(void)
{
	

	//下行485抄表端口初始化
	g_uiRS4852Fd = open_com_dev(DEVICE4852);
	if (set_com_para(g_uiRS4852Fd,2400,8,1,'E') == FALSE) { 
 
	}
	else
		printf("RS485Down com parameter set OK.\n");	

	ioctl(g_uiIoControl,RS4852_C31,RS485_RECV);	//下行485抄表端口初始化设置为接收数据。
  	

	//接收数据。
 	 while(1){
    		UartDown485_Rec();
    	 }
	 
  	return(CONTROL_RET_SUC);
	
}



/*
******************************************************************************
* 函数名称： int32 UartDown485_Rec(void)
* 说	明： 
* 参	数： 无
******************************************************************************
*/

int32 UartDown485_Rec(void)
{
    uint8 lu8data = 0;
    fd_set ReadSetFD;
    struct timeval stTimeVal;
    uint8 ret;
    
    while(1)
    {
        FD_ZERO(&ReadSetFD);
        FD_SET(g_uiRS4852Fd, &ReadSetFD);
        /* 设置等待的超时时间 */
        stTimeVal.tv_sec = REC_TIMEOUT_SEC;
        stTimeVal.tv_usec = REC_TIMEOUT_USEC;
        
        if (select(g_uiRS4852Fd+1, &ReadSetFD, NULL, NULL,&stTimeVal) > 0){
		   if (FD_ISSET(g_uiRS4852Fd, &ReadSetFD)){
				if(read(g_uiRS4852Fd, &lu8data, 1) == 1){
					ret = QueueWrite((void*)DownRecQueue_RS485, lu8data);
					if(ret != QUEUE_OK){
						//printf("[%s][%s][%d] ret=%d queue full\n",FILE_LINE,ret);
					}
		   
					OSSemPost(DOWN_COMMU_DEV_485);

					//printf("\n[%s][%s][%d] 0x%02X \n",FILE_LINE,lu8data);  //测试用
		   
				}
		    }

        }
	   

    }
    
  	return(CONTROL_RET_FAIL);
	
}


  

