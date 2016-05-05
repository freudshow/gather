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





extern struct tm gTimeNode; 








extern uint8 ReaOneMeter(MeterFileType *pmf);
extern void pthread_ReadAllMeters(void);
extern uint8 METER_ChangeChannel(uint8 Channel);




#endif  //_READ_ALL_METERS_

