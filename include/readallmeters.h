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
	uint16 u16MeterID; //Hex
	uint8 u8MeterAddr[LENGTH_B_METER_ADDRESS];  //BCD码
	uint8 u8MeterType;	   //BCD
	uint8 u8ProtocolType;  //HEX
	uint8 u8Channel;	   //HEX
}MeterFileType;










extern uint8 ReaOneMeter(MeterFileType *pmf,char *pPositionInfo);
extern void pthread_ReadAllMeters(void);
extern uint8 METER_ChangeChannel(uint8 Channel);





#endif  //_READ_ALL_METERS_

