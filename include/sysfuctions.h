/*
  *******************************************************************************
  * @file    sysfuctions.h
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


#ifndef _SYSFUCTIONS_H_
#define _SYSFUCTIONS_H_


#include "includes.h"



#define  FILE_LINE   __FILE__,__FUNCTION__,__LINE__

#define       TASKUP_MODULE    0    //集中器与主站的通信数据
#define       FATFS_MODULE     1
#define       SD_MODULE   	 2
#define       GPRS_MODULE  	 3
#define       TASKDOWN_MODULE  4    //集中器与热表的通信数据
#define       UPDATE_MODULE    5
#define       ALL_MODULE    	 6
#define 	    XML_MODULE		 7
#define	    TIMING_METER 	 8	 //定时抄表
#define 	    XML_DEAL		 9	 //xml 数据处理
#define       MSIC_MODULE      10
#define       QUEUE_MODULE     11
#define	    METER			 12	 //抄表
#define       SEM     		 13


#define	KERN_EMERG	0	/* system is unusable			*/
#define	KERN_ALERT	1	/* action must be taken immediately	*/
#define	KERN_CRIT	     2	/* critical conditions			*/
#define	KERN_ERR	     3	/* error conditions			*/
#define	KERN_WARNING	4	/* warning conditions			*/
#define	KERN_NOTICE	5	/* normal but significant condition	*/
#define	KERN_INFO		6	/* informational			*/
#define	KERN_DEBUG	7	/* debug-level messages			*/


#define Ascii2Hex(data) ((data >='0' && data <='9')?(data-'0'):((data>='A'&&data<='F')?(data-'A'+10):((data>='a'&&data<='f')?(data-'a'+10):0)))




extern uint8 gDebugModule[];
extern uint8 gDebugLevel;
extern sem_t  CRITICAL_sem;
extern sem_t  GprsIPDTask_sem;
extern sem_t  Gprs_Sem;
extern sem_t  RS485Up_sem;  //上行485端口信号量
extern sem_t  RS485Down_sem;//下行485端口信号量
extern sem_t  GPRSPort_sem; //GPRS端口信号量
extern sem_t  OperateDB_sem; //操作数据库的信号量
extern sem_t  OperateMBUS_sem;	//操作MBUS端口信号量
extern sem_t  Opetate485Down_sem;	//操作485下行端口信号量。
extern sem_t  Validate_sem;






#ifdef DEBUG
#define debug(fmt,args...)   if(gDebugLevel  >= 1) printf(fmt,##args)
#define debugX(level,fmt,args...)   if(gDebugLevel >= level) printf(fmt,##args);

#define debug_emerg(dev, fmt, args...)		\
	if(dev>=KERN_EMERG) debug(fmt,##args);
#define debug_alert(dev, fmt, args...)		\
	if(dev>=KERN_ALERT) debug(fmt,##args);
#define debug_crit(dev, fmt, args...)		\
	if(dev>=KERN_CRIT) debug(fmt,##args);
#define debug_err(dev, fmt, args...)		\
	if(dev>=KERN_ERR) debug(fmt,##args);
#define debug_warn(dev, fmt, args...)		\
	if(dev>=KERN_WARNING) debug(fmt,##args);
#define debug_notice(dev, fmt, args...)		\
	if(dev>=KERN_NOTICE) debug(fmt,##args);
#define debug_info(dev, fmt, args...)		\
	if(dev>=KERN_INFO) debug(fmt,##args);
#define debug_debug(dev, fmt, args...)		\
   if(dev>=KERN_DEBUG) debug(fmt,##args);
#else
#define debug(fmt,args...)   
#define debugX(level,fmt,args...)  
#define debug_emerg(dev, fmt, args...)
#define debug_alert(dev, fmt, args...)
#define debug_crit(dev, fmt, args...)
#define debug_err(dev, fmt, args...)	
#define debug_warn(dev, fmt, args...)
#define debug_notice(dev, fmt, args...)	
#define debug_info(dev, fmt, args...)	
#define debug_debug(dev, fmt, args...)	
#endif










extern void OSTimeDly(uint32 ms);
extern void OS_ENTER_CRITICAL(void);
extern void OS_EXIT_CRITICAL(void);
extern void  OSSemPend (uint8 dev,uint32  timeout,uint8 *perr);
extern void OSSemPost(uint8 dev);
extern int OSSem_timedwait (sem_t *pSem,uint32  timeout);
extern uint8 Str2Bin(char *str, uint8 *array,uint16 lens);
extern uint8 PUBLIC_CountCS(uint8* _data, uint16 _len);
extern void FileSend(uint8 Dev, FILE *fp);



#endif //_SYSFUCTIONS_H_

