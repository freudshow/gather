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
#include "uart_gprs.h"
#include "uart_mbus_down485.h"
#include "commap.h"
#include "xml.h"
#include "readallmeters.h"




extern void create_pthread(void);
extern void create_pthread_AllUartRec(void);
extern void create_pthread_GprsRelated(void);


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

	close_db();
	exit(0);
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
    pthread_t RS485UpDeal_pthreadID;
    pthread_t ReadAllMeters_pthreadID;
    pthread_t GPRS_UpHis_pthreadID;
	create_pthread_AllUartRec();//�������д��ڽ����̣߳�����gprs��mbus��485���С�485���С�
	create_pthread_GprsRelated();  //������GPRS�йص��̡߳�

	lReg = pthread_create(&RS485UpDeal_pthreadID,NULL,(void *)pthread_RS485UpDeal,NULL);
     if(0 != lReg){
        	printf ("Create pthread_RS485UpDeal error!\n");
        	exit (1);
    	}
	else
		printf ("Create pthread_RS485UpDeal OK!\n");

	lReg = pthread_create(&ReadAllMeters_pthreadID,NULL,(void *)pthread_ReadAllMeters,NULL);
     if(0 != lReg){
        	printf ("Create pthread_ReadAllMeters error!\n");
        	exit (1);
    	}
	else
		printf ("Create pthread_ReadAllMeters OK!\n");

    lReg = pthread_create(&GPRS_UpHis_pthreadID,NULL,(void *)pthread_up_long_data,NULL);
    if(0 != lReg){
        printf ("Create pthread_up_long_data error!\n");
        exit (1);
    }
    else
        printf ("Create pthread_up_long_data OK!\n");

    /*
    pthread_t pthReadmeter_test;
    lReg = pthread_create(&pthReadmeter_test,NULL,(void *)pthread_readmeter_test,NULL);
    if(0 != lReg){
        printf ("Create pthread_up_long_data error!\n");
        exit (1);
    }
    else
        printf ("Create pthread_up_long_data OK!\n");
    */
}



/*
  ******************************************************************************
  * �������ƣ� void create_pthread_AllUartRec(void)
  * ˵    ���� ��������Uart�����̣߳�����gprs��mbus��485���С�485���С�
  * ��    ���� ��
  ******************************************************************************
  */

void create_pthread_AllUartRec(void)
{

	int32 lReg = 0;
	pthread_t RS485Up_Rec_pthreadID;  //����485�ӿ��߳�ID.
	pthread_t UartGprs_Rec_pthreadID;
	pthread_t UartMbus_Rec_pthreadID;
	pthread_t UartDown485_Rec_pthreadID;

	lReg = pthread_create(&RS485Up_Rec_pthreadID,NULL,(void *)pthread_RS485up_Rec,NULL);
	if(0 != lReg){
		printf ("Create RS485Up_Rec_pthreadID error!\n");
		exit (1);
	}
	else
		printf ("Create RS485Up_Rec_pthreadID OK!\n");

	lReg = pthread_create(&UartGprs_Rec_pthreadID,NULL,(void *)pthread_UartGprs_Rec,NULL);
	if(0 != lReg){
		printf ("Create UartGprs_Rec_pthreadID error!\n");
		exit (1);
	}
	else
		printf ("Create UartGprs_Rec_pthreadID OK!\n");


	lReg = pthread_create(&UartMbus_Rec_pthreadID,NULL,(void *)pthread_UartMbus_Rec,NULL);
	if(0 != lReg){
		printf ("Create UartMbus_Rec_pthreadID error!\n");
		exit (1);
	}
	else
		printf ("Create UartMbus_Rec_pthreadID OK!\n");


	lReg = pthread_create(&UartDown485_Rec_pthreadID,NULL,(void *)pthread_UartDown485_Rec,NULL);
	if(0 != lReg){
		printf ("Create UartDown485_Rec_pthreadID error!\n");
		exit (1);
	}
	else
		printf ("Create UartDown485_Rec_pthreadID OK!\n");



}


/*
  ******************************************************************************
  * �������ƣ� void create_pthread_GprsRelated(void)
  * ˵    ���� ��������Gprs�����йص��̡߳�
  * ��    ���� ��
  ******************************************************************************
  */

void create_pthread_GprsRelated(void)
{
    int32 lReg = 0;
    pthread_t GPRS_Mana_pthreadID;  //GPRS�����߳�ID.
    pthread_t GPRS_IPD_pthreadID;  //GPRS���紫�����ݴ����߳�ID.
    pthread_t GprsDataDeal_pthreadID;  //GPRS���紫�����ݴ����߳�ID.
   
    lReg = pthread_create(&GPRS_Mana_pthreadID,NULL,(void *)pthread_GPRS_Mana,NULL);
    if(0 != lReg){
        	printf ("Create pthread_GPRS_Mana error!\n");
        	exit (1);
    }
    else
        printf ("Create pthread_GPRS_Mana OK!\n");


    lReg = pthread_create(&GPRS_IPD_pthreadID,NULL,(void *)pthread_GPRS_IPD,NULL);
    if(0 != lReg){
        	printf ("Create pthread_GPRS_IPD error!\n");
        	exit (1);
    }
    else
        printf ("Create pthread_GPRS_IPD OK!\n");

    lReg = pthread_create(&GprsDataDeal_pthreadID,NULL,(void *)pthread_GprsDataDeal,NULL);
    if(0 != lReg){
        printf ("Create pthread_GprsDataDeal error!\n");
        exit (1);
    }
    else
        printf ("Create pthread_GprsDataDeal OK!\n");

    
    
}












