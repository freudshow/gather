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



uint8 gu8MbusComSetIndex = 0;		//���ڼ�¼MBUS����˿ڵ�ǰ��������ʲô��
uint8 gu8485DownComSetIndex = 0;	//���ڼ�¼485���г���˿ڵ�ǰ��������ʲô��



/*
  ******************************************************************************
  * �������ƣ� uint8 METER_ComSet1(void) 
  * ˵    ���� ����˿ڲ�����1�����ã�MBUS�˿ڡ�
  * ��    ���� 
  ******************************************************************************
*/

uint8 METER_ComSet1(void) 
{
	/* ����MBUS���ڲ���
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
  * �������ƣ� uint8 METER_ComSet2(void) 
  * ˵    ���� ����˿ڲ�����2�����ã�MBUS�˿ڡ�
  * ��    ���� 
  ******************************************************************************
*/

uint8 METER_ComSet2(void) 
{
	/* ����MBUS���ڲ���
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
  * �������ƣ� uint8 METER_ComSet3(void) 
  * ˵    ���� ����˿ڲ�����3�����ã�����485�˿ڡ�
  * ��    ���� 
  ******************************************************************************
*/

uint8 METER_ComSet3(void) 
{
	/* ��������485���ڲ���
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
  * �������ƣ� uint8 METER_ComSet4(void) 
  * ˵    ���� ����˿ڲ�����4�����ã�MBUS�˿ڡ�
  * ��    ���� 
  ******************************************************************************
*/

uint8 METER_ComSet4(void) 
{
	/* ����MBUS���ڲ���
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
  * �������ƣ� uint8 METER_ComSet5(void) 
  * ˵    ���� ����˿ڲ�����5�����ã�MBUS�˿ڡ�
  * ��    ���� 
  ******************************************************************************
*/

uint8 METER_ComSet5(void) 
{
	/* ����MBUS���ڲ���
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
  * �������ƣ� uint8 METER_ComSet6(void) 
  * ˵    ���� ����˿ڲ�����6�����ã�����485�˿ڡ�
  * ��    ���� 
  ******************************************************************************
*/

uint8 METER_ComSet6(void) 
{
	/* ��������485���ڲ���
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
  * �������ƣ� uint8 METER_ComSet7(void) 
  * ˵    ���� ����˿ڲ�����7�����ã�����485�˿ڡ�
  * ��    ���� 
  ******************************************************************************
*/

uint8 METER_ComSet7(void) 
{
	/* ��������485���ڲ���
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
  * �������ƣ� uint8 METER_ComSet7(void) 
  * ˵    ���� ����˿ڲ�����7�����ã�����485�˿ڡ�
  * ��    ���� 
  ******************************************************************************
*/


uint8 ELEC_METER_ComSet1(void) 
{
	/* ��������485���ڲ���
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






