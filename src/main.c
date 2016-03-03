/*
  *******************************************************************************
  * @file    main.c 
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
#include "sysinit.h"
#include "rs485up.h"
#include "commap.h"
#include "xml.h"



extern void create_pthread(void);


/*
  ******************************************************************************
  * �������ƣ� main(void)
  * ˵    ���� ������
  * ��    ���� ��
  ******************************************************************************
  */
int main(int argc, char **argv)
{
	sysinit();
	create_pthread();

	while(1){
		usleep(1000000);
	}




	return 0;
	
	
}


/*
  ******************************************************************************
  * �������ƣ� create_pthread(void)
  * ˵    ���� 
  * ��    ���� ��
  ******************************************************************************
  */
void create_pthread(void)
{
	int32 lReg = 0;
	pthread_t RS485Up_Rec_pthreadID;  //����485�ӿ��߳�ID.
	pthread_t RS485UpDeal_pthreadID;

	lReg = pthread_create(&RS485Up_Rec_pthreadID,NULL,(void *)pthread_RS485up_Rec,NULL);
     if(0 != lReg){
        	printf ("Create RS485Up_Rec_pthreadID error!\n");
        	exit (1);
    	}
	else
		printf ("Create RS485Up_Rec_pthreadID OK!\n");

	lReg = pthread_create(&RS485UpDeal_pthreadID,NULL,(void *)pthread_RS485UpDeal,NULL);
     if(0 != lReg){
        	printf ("Create pthread_RS485UpDeal error!\n");
        	exit (1);
    	}
	else
		printf ("Create pthread_RS485UpDeal OK!\n");


}
















