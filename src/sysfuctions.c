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
sem_t  GprsIPDTask_sem;//控制GprsIPD线程是否运行的信号量(暂没使用其他线程挂起的方法。)。
sem_t  Gprs_Sem;
sem_t  RS485Up_sem;  //上行485端口信号量
sem_t  RS485Down_sem;//下行485端口信号量
sem_t  GPRSPort_sem; //GPRS端口信号量
sem_t  OperateDB_sem; //操作数据库的信号量
sem_t  OperateMBUS_sem;	//操作MBUS端口信号量
sem_t  Opetate485Down_sem;	//操作485下行端口信号量。

sem_t Validate_sem;  //GPRS登录信号量。

sem_t His_up_sem;//历史数据上传数据量
sem_t His_asw_sem;//历史数据应答数据量



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
  * 函数名称： int OSSem_timedwait (sem_t Sem,uint32  timeout)
  * 说    明： 可定时等待一个信号量的到来。
  * 参    数： 成功等待到信号量，返回0，非0说明超时。
  			sem_t Sem 要等待的信号量。
  			uint32  timeout 超时时间，单位毫秒。
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
  * 函数名称： uint8 Str2Bin(char *str, uint8 *array,uint16 lens)
  * 说    明： 将字符串转换为十六进制数。
  * 参    数： lens为array指向空间的字节数，以防str较长，array空间不足带来的问题。
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
  * 函数名称： uint8 PUBLIC_CountCS(uint8* _data, uint16 _len)
  * 说    明： 计算一串数字的累加和校验，溢出部分舍弃。
  * 参    数： 
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
* 函数名称：void FileSend(uint8 Dev, FILE *fp)
* 说	明： 发送一个文件内容。
* 参	数： Dev可以选择通过485还是GPRS发送。
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


uint8 asc_to_datestr(char* src, char* dest)
{
    int i=0;
    char *pTimeStr[6]={NULL};  
    char *buf=src;  
    char *saveptr=NULL;
    char *monEng[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char *monDec[12]={"01","02","03","04","05","06","07","08","09","10","11","12"};

    printf("src: %s\n", src);
    i=0;
    while((pTimeStr[i] = strtok_r(buf, " ", &saveptr)))
    {   
        i++;
        buf=NULL;
    }

    strncpy(dest, pTimeStr[4], 4);//year
    strcat(dest, "-");
    for(i=0;i<12;i++)
        if(strcmp(pTimeStr[1], monEng[i])==0)
            strcat(dest, monDec[i]);//mon
    
    strcat(dest, "-");
    strcat(dest, pTimeStr[2]);//day
    strcat(dest, " ");
    strcat(dest, pTimeStr[3]);//time
    //printf("dest: %s\n", dest);
    return NO_ERR;
}
