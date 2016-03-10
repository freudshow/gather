/*
  *******************************************************************************
  * @file    readallmeters.h
  * @author  zjjin
  * @version V0.0.0
  * @date    03-05-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/

#ifndef _READ_ALL_METERS_
#define _READ_ALL_METERS_


#include "includes.h"


//定义表基础信息结构体。
typedef struct
{
	uint16 u16MeterID;  
	uint8 u8MeterAddr[7];
	uint8 u8MeterType;
	uint8 u8ProtocolType;
	uint8 u8Channel;
}MeterFileType;






extern sqlite3 *p_sqlitedb;
extern uint8 gu8ReadAllMeterFlag;
extern uint8 gu8DownComDev;





extern uint8 ReadAllMeters(MeterFileType *pmf,char *pPositionInfo);
extern void pthread_ReadAllMeters(void);
extern uint8 METER_ChangeChannel(uint8 Channel);





#endif  //_READ_ALL_METERS_

