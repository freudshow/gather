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
  * 函数名称： main(void)
  * 说    明： 主函数
  * 参    数： 无
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
  * 函数名称： create_pthread(void)
  * 说    明： 
  * 参    数： 无
  ******************************************************************************
  */
void create_pthread(void)
{
    int32 lReg = 0;
    pthread_t RS485UpDeal_pthreadID;
    pthread_t ReadAllMeters_pthreadID;
    pthread_t GPRS_UpHis_pthreadID;
	create_pthread_AllUartRec();//创建所有串口接收线程，包括gprs、mbus、485上行、485下行。
	create_pthread_GprsRelated();  //创建和GPRS有关的线程。

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
  * 函数名称： void create_pthread_AllUartRec(void)
  * 说    明： 创建所有Uart接收线程，包括gprs、mbus、485上行、485下行。
  * 参    数： 无
  ******************************************************************************
  */

void create_pthread_AllUartRec(void)
{

	int32 lReg = 0;
	pthread_t RS485Up_Rec_pthreadID;  //上行485接口线程ID.
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
  * 函数名称： void create_pthread_GprsRelated(void)
  * 说    明： 创建所有Gprs处理有关的线程。
  * 参    数： 无
  ******************************************************************************
  */

void create_pthread_GprsRelated(void)
{
    int32 lReg = 0;
    pthread_t GPRS_Mana_pthreadID;  //GPRS管理线程ID.
    pthread_t GPRS_IPD_pthreadID;  //GPRS网络传输数据处理线程ID.
    pthread_t GprsDataDeal_pthreadID;  //GPRS网络传输数据处理线程ID.
   
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












