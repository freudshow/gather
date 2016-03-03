/*
  *******************************************************************************
  * @file    uart_set.h
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

#ifndef _UART_SET_
#define _UART_SET_


extern int32 open_com_dev(char *Dev);
extern void close_com_dev(int32 fd);
extern int8 set_com_para(int32 fd,int32 speed, int32 databits,int32 stopbits,int32 parity);



#endif  //_UART_SET_

