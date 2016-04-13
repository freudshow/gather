/*
  *******************************************************************************
  * @file    bsp.h
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

#ifndef _BSP_H_
#define _BSP_H_


extern uint8 gu8MbusComSetIndex;
extern uint8 gu8485DownComSetIndex;
extern uint32 gu32MbusBaudRate;  //Mbus通道波特率记录.
extern uint32 gu32Down485BaudRate;  //下行485通道波特率记录.



extern uint8 METER_ComSet1(void);
extern uint8 METER_ComSet2(void);
extern uint8 METER_ComSet3(void);
extern uint8 METER_ComSet4(void);
extern uint8 METER_ComSet5(void);
extern uint8 METER_ComSet6(void);
extern uint8 METER_ComSet7(void);
extern uint8 METER_ComSet8(void);








#endif  //_BSP_H_


