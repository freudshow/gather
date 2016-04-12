/*
  *******************************************************************************
  * @file    bsp.c 
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
#include "uart_set.h"
#include "bsp.h"



uint8 gu8MbusComSetIndex = 0;		//用于记录MBUS抄表端口当前的设置是什么。
uint8 gu8485DownComSetIndex = 0;	//用于记录485下行抄表端口当前的设置是什么。



/*
  ******************************************************************************
  * 函数名称： uint8 METER_ComSet1(void) 
  * 说    明： 抄表端口参数第1种设置，MBUS端口。
  * 参    数： 
  ******************************************************************************
*/

uint8 METER_ComSet1(void) 
{
	/* 配置MBUS串口参数
	    - BaudRate = 2400 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - EVEN_CHECK
	*/
	uint8 err = TRUE;

	if(gu8MbusComSetIndex != 1){
		err = set_com_para(g_uiMbusFd,2400,8,1,'E');
		if(err == TRUE)
			gu8MbusComSetIndex = 1;

		usleep(1000);
	}

	return err;
}


/*
  ******************************************************************************
  * 函数名称： uint8 METER_ComSet2(void) 
  * 说    明： 抄表端口参数第2种设置，MBUS端口。
  * 参    数： 
  ******************************************************************************
*/

uint8 METER_ComSet2(void) 
{
	/* 配置MBUS串口参数
	    - BaudRate = 1200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - NONE_CHECK
	*/

	uint8 err = TRUE;

	if(gu8MbusComSetIndex != 2){
		err = set_com_para(g_uiMbusFd,1200,8,1,'N');
		if(err == TRUE)
			gu8MbusComSetIndex = 2;

		usleep(1000);

	}

	return err;
}



/*
  ******************************************************************************
  * 函数名称： uint8 METER_ComSet3(void) 
  * 说    明： 抄表端口参数第3种设置，下行485端口。
  * 参    数： 
  ******************************************************************************
*/

uint8 METER_ComSet3(void) 
{
	/* 配置下行485串口参数
	    - BaudRate = 2400 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - EVEN_CHECK
	*/

	uint8 err = TRUE;

	if(gu8485DownComSetIndex != 3){
		err = set_com_para(g_uiRS4852Fd,2400,8,1,'E');
		if(err == TRUE)
			gu8485DownComSetIndex = 3;

		usleep(1000);
	}

	return err;
}



/*
  ******************************************************************************
  * 函数名称： uint8 METER_ComSet4(void) 
  * 说    明： 抄表端口参数第4种设置，MBUS端口。
  * 参    数： 
  ******************************************************************************
*/

uint8 METER_ComSet4(void) 
{
	/* 配置MBUS串口参数
	    - BaudRate = 1200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - EVEN_CHECK
	*/

	uint8 err = TRUE;

	if(gu8MbusComSetIndex != 4){
		err = set_com_para(g_uiMbusFd,1200,8,1,'E');
		if(err == TRUE)
			gu8MbusComSetIndex = 4;

		usleep(1000);
	}

	return err;
}




/*
  ******************************************************************************
  * 函数名称： uint8 METER_ComSet5(void) 
  * 说    明： 抄表端口参数第5种设置，MBUS端口。
  * 参    数： 
  ******************************************************************************
*/

uint8 METER_ComSet5(void) 
{
	/* 配置MBUS串口参数
	    - BaudRate = 2400 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - NONE_CHECK
	*/

	uint8 err = TRUE;

	if(gu8MbusComSetIndex != 5){
		err = set_com_para(g_uiMbusFd,2400,8,1,'N');
		if(err == TRUE)
			gu8MbusComSetIndex = 5;

		usleep(1000);
	}

	return err;
}




/*
  ******************************************************************************
  * 函数名称： uint8 METER_ComSet6(void) 
  * 说    明： 抄表端口参数第6种设置，下行485端口。
  * 参    数： 
  ******************************************************************************
*/

uint8 METER_ComSet6(void) 
{
	/* 配置下行485串口参数
	    - BaudRate = 1200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - EVEN_CHECK
	*/

	uint8 err = TRUE;

	if(gu8485DownComSetIndex != 6){
		err = set_com_para(g_uiRS4852Fd,1200,8,1,'E');
		if(err == TRUE)
			gu8485DownComSetIndex = 6;

		usleep(1000);
	}

	return err;
}



/*
  ******************************************************************************
  * 函数名称： uint8 METER_ComSet7(void) 
  * 说    明： 抄表端口参数第7种设置，下行485端口。
  * 参    数： 
  ******************************************************************************
*/

uint8 METER_ComSet7(void) 
{
	/* 配置下行485串口参数
	    - BaudRate = 1200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - EVEN_CHECK
	*/

	uint8 err = TRUE;

	if(gu8485DownComSetIndex != 7){
		err = set_com_para(g_uiRS4852Fd,1200,8,1,'E');
		if(err == TRUE)
			gu8485DownComSetIndex = 7;

		usleep(1000);
	}

	return err;
}


/*
  ******************************************************************************
  * 函数名称： uint8 METER_ComSet7(void) 
  * 说    明： 抄表端口参数第7种设置，下行485端口。
  * 参    数： 
  ******************************************************************************
*/


uint8 ELEC_METER_ComSet1(void) 
{
	/* 配置下行485串口参数
	    - BaudRate = 1200 baud
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - EVEN_CHECK
	*/

	uint8 err = TRUE;

	if(gu8485DownComSetIndex != 7){
		err = set_com_para(g_uiRS4852Fd,9600,8,1,'N');
		if(err == TRUE)
			gu8485DownComSetIndex = 7;

		usleep(1000);
	}

	return err;
}






