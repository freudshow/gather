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
  * 函数名称： int pthread_RS485up_Rec()
  * 说    明： 上行485接口接收数据处理线程。
  * 参    数： 无
  ******************************************************************************
*/

int pthread_RS485up_Rec(void)
{
	
	//上行485口初始化
	g_uiRS4851Fd = open_com_dev(DEVICE4851);
	if (set_com_para(g_uiRS4851Fd,RS485UP_COM_SPEED,8,1,'N') == FALSE) { 
 
	}
	else
		printf("RS485Up com parameter set OK.\n");	
  	

	ioctl(g_uiIoControl,RS4851_C30,RS485_RECV);  //485设置为接收数据。
	usleep(1000);

	//接收数据。
 	 while(1){
    		RS485up_Rec();
    	 }
	 
  	return(CONTROL_RET_SUC);
	
}



/*
******************************************************************************
* 函数名称： INT32 RS485up_Rec(void)
* 说	明： 
* 参	数： 无
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
        /* 设置等待的超时时间 */
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
* 函数名称：void RS485Up_FileSend(uint8 Dev, FILE *fp)
* 说	明： 通过485上行端口，发送一个文件内容。
* 参	数： 
******************************************************************************
*/
void RS485Up_FileSend(uint8 Dev, FILE *fp)
{
	int	i = 0;
	uint8 err = 0;
	int32 l32FileSize = 0;
	uint32 lu32SendNum = 0;

	sem_wait(&RS485Up_sem);

	//首先将485方向设置为send.
	ioctl(g_uiIoControl,RS4851_C30,RS485_SED);
	usleep(1000);
	
	//计算文件大小。
	fseek(fp,0,2);			//位置指针到文件末尾
	l32FileSize = ftell(fp);	//计算文件大小
	fseek(fp,0,0);			//位置指针到文件开始
	lu32SendNum = SEND_TEMPBUF_SIZE;

	for(i=0;i<(l32FileSize/SEND_TEMPBUF_SIZE + 1);i++){
		if(i == l32FileSize/SEND_TEMPBUF_SIZE){ //最后一包
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

	//最后将485方向设置为recv.
	ioctl(g_uiIoControl,RS4851_C30,RS485_RECV);
	usleep(1000);

	sem_post(&RS485Up_sem);


}



/*
******************************************************************************
* 函数名称： uint8 RS485Up_DataSend(uint8 *Data,uint32 n)
* 说	明： 上行485接口发送数据。
* 参	数： 
******************************************************************************
*/
uint8 RS485Up_DataSend(uint8 *Data,uint32 n)
{
	uint32 lu32delaytimes = 0;

	write(g_uiRS4851Fd, (uint8 *)Data, n);

	lu32delaytimes = 12000000 / RS485UP_COM_SPEED;  //1000000*12/RS485UP_COM_SPEED;
	lu32delaytimes = lu32delaytimes*n;  //计算出在速率为RS485UP_COM_SPEED时，发送n个字节大概需要时间。

	usleep(lu32delaytimes); //保证本次数据发完。
	

	return NO_ERR; 

}







/*
******************************************************************************
* 函数名称： void pthread_RS485UpDeal(void)
* 说	明： 上行485通道处理线程。
* 参	数： 
******************************************************************************
*/

void pthread_RS485UpDeal(void)
{
	uint8 err = 0;
	//uint8 lu8data = 0;
	uint16 lu16outtime = 1000;  //等待 毫秒数。
	uint8 lu8xmlIndex = 0;
	FILE *fp;

	while(1){	
		do{
			lu8xmlIndex = Get_XMLBuf();  //获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
		}while(lu8xmlIndex == ERR_FF);

		//printf("Get_XMLBuf %d.\n",lu8xmlIndex);
		
		err = UpGetXMLStart(lu8xmlIndex,UP_COMMU_DEV_485,lu16outtime);
		if(err == NO_ERR){
			printf("UpGetXMLStart OK.\n");
			err = UpGetXMLEnd(lu8xmlIndex,UP_COMMU_DEV_485,lu16outtime);
			if(err == NO_ERR){//说明接收到一帧完整的xml数据。
				printf("UpGetXMLEnd OK.\n");
				//相应处理。
				fp = fopen(gXML_File[lu8xmlIndex].pXMLFile,"r");
				RS485Up_FileSend(UP_COMMU_DEV_485,fp);

				fclose(fp);
				
				Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存。

			}
			else{
				Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存。
			}

		}
		else{
			Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存。
		}		


	}


}







  

