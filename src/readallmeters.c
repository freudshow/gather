/*
  *******************************************************************************
  * @file    readallmeters.c 
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

#include "readallmeters.h"
#include "read_heatmeter.h"





sqlite3 *p_sqlitedb;	//���ݿ���
uint8 gu8ReadAllMeterFlag = 0;  //���ڱ���Ƿ����ڳ�ȫ��1-���ڳ�ȫ��0-û���ڳ�ȫ��
uint8 gu8DownComDev = 0; //��¼ʹ����һ�������豸,ֻ������ȫ��������У�����������ָ�����/����





/*
  ******************************************************************************
  * �������ƣ�static int CallBack_ReadAllMeters(void *p_Para, int argc, char **argv, char **azColName)
  * ˵    ������ѯ���ݿⳭ��ص���������ͨ�����������������������ص������д���
  		     ��̬������ֻ�����ڱ��ļ����á�
  * ��    ���� 
  ******************************************************************************
*/

static int CallBack_ReadAllMeters(void *p_Para, int argc, char **argv, char **azColName)
{
	uint8 i = 0;
	//uint8 err = 0;
	MeterFileType mf;

	
	for(i=0; i<argc; i++)
	{
		debug_info(gDebugModule[METER],"%s", argv[i]);
		debug_info(gDebugModule[METER],"    %s\n",azColName[i]);
	}
	memset(&mf, 0x00, sizeof(mf));

/*
	Str2Bin(argv[geMI_MeterID], (uint8 *)&mf.u16MeterID,sizeof(mf.u16MeterID));
	Str2Bin(argv[geMI_MeterAddr], (uint8 *)mf.u8MeterAddr,sizeof(mf.u8MeterAddr));
	Str2Bin(argv[geMI_MeterType], (uint8 *)&mf.u8MeterType,sizeof(mf.u8MeterType));
	Str2Bin(argv[geMI_ProtocolType], (uint8 *)&mf.u8ProtocolType,sizeof(mf.u8ProtocolType));
	Str2Bin(argv[geMI_ChannelNum], (uint8 *)&mf.u8Channel,sizeof(mf.u8Channel));

*/

	ReadAllMeters(&mf,NULL);	



	return NO_ERR;

}



/*
  ******************************************************************************
  * �������ƣ�ReadAllMeters(MeterFileType *pmf,char *pPositionInfo);
  * ˵    ������������������ʧ�������Ҫ�ظ�����Ҳ����������д���
  * ��    ���� *pmf ����Ϣ��
  			*pPositionInfo ��װλ����Ϣ��
  ******************************************************************************
*/
uint8 ReadAllMeters(MeterFileType *pmf,char *pPositionInfo)
{
	uint8 err = 0;
	uint8 i = 0;
	uint8 lu8retrytimes = 0;
	uint8 lu8Channel = 0;	//�����������ͨ����

	
	lu8Channel = pmf->u8Channel;
	METER_ChangeChannel(lu8Channel);  //��ȷ���ڶ�Ӧͨ���ϡ�
	
	
	switch(pmf->u8MeterType){
		case HEATMETER:
			lu8retrytimes = 1 + 1;  //������������ǰ�ù̶�1�β���������Ҫ�������á�
			for(i=0;i<lu8retrytimes;i++){
				err = Read_HeatMeter(pmf,pPositionInfo);
				OSTimeDly(200); //��ֹ����̫�죬�����Ժ���Ըĳ���ʱ�����á�
				if(err == NO_ERR)
					break;
			}

			break;
		case WATERMETER:

			break;
		case ELECTMETER:

			break;

		case GASMETER:

			break;

		default:
			err = ERR_1;
			break;		


	}

	return err;


}








/*
  ******************************************************************************
  * �������ƣ� pthread_ReadAllMeters(void)
  * ˵    ���� �����б��̡߳�
  * ��    ���� ��
  ******************************************************************************
*/

void pthread_ReadAllMeters(void)
{
	int rec = 0;
	char SQ_Str[100] = "";
	char *zErrMsg = 0;


	uint8 i = 0;
	MeterFileType mf;
	char position[] = "3#-1-102";

	while(1){
		//��Ⳮ���źţ���������ź���Ч����ʼȫ�������򣬵ȴ�30s�ɣ���Щ�Ժ�Ҫ���䣬������������á�

		//������ͨ�������ͨ������

		/*
		if(pmf->u8Channel == RS485_DOWN_CHANNEL){
			sem_wait(&Opetate485Down_sem);
			gu8DownComDev = DOWN_COMM_DEV_485;
		}
		else{
			sem_wait(&OperateMBUS_sem);
			gu8DownComDev = DOWN_COMM_DEV_MBUS;
		}
		*/

		gu8ReadAllMeterFlag = 1;


		
		//rec = sqlite3_exec(p_sqlitedb, SQ_Str,CallBack_ReadAllMeters, NULL, &zErrMsg);
		//if( rec != SQLITE_OK ){
          //          debug_err(gDebugModule[METER],"[%s][%s][%d] \n",FILE_LINE);
          //          fprintf(stderr, "SQL error: %s\n", zErrMsg);
		//	     sqlite3_free(zErrMsg);
          //}

		/*
		if(pmf->u8Channel == RS485_DOWN_CHANNEL){
			post(&Opetate485Down_sem);
		}
		else{
			post(&OperateMBUS_sem);
		}
		*/


		gu8ReadAllMeterFlag = 0;


		//begin������
		for(i=1;i<=7;i++){
			mf.u16MeterID =0x01;
			mf.u8Channel = i;
			mf.u8MeterType = 0x20;
			mf.u8MeterAddr[0] = 0xaa;
			mf.u8MeterAddr[1] = 0xaa;
			mf.u8MeterAddr[2] = 0xaa;
			mf.u8MeterAddr[3] = 0xaa;
			mf.u8MeterAddr[4] = 0xaa;
			mf.u8MeterAddr[5] = 0xaa;
			mf.u8MeterAddr[6] = 0xaa;
			mf.u8ProtocolType = 0;
			
			ReadAllMeters(&mf,position);	

			//usleep(3000000);

		}



		//end������
		
	}


}




/*
  ******************************************************************************
  * �������ƣ� void Mbus_ChangeChannel(uint8 channel)
  * ˵    ���� �л�MBUSͨ��
  * ��    ���� 
  ******************************************************************************
*/
static void Mbus_ChangeChannel(uint8 channel)
{
	//uint8 i=0;
	//for(i=0; i<6; i++){  //�ȹص�����ͨ��
	//	 ioctl(g_uiIoControl,MBUS_C0+i,0); 
	//}

	ioctl(g_uiIoControl,MBUS_C0,0); //�ȹص�����ͨ��
	ioctl(g_uiIoControl,MBUS_C1,0); 
	ioctl(g_uiIoControl,MBUS_C2,0); 
	ioctl(g_uiIoControl,MBUS_C3,0); 
	ioctl(g_uiIoControl,MBUS_C4,0); 
	ioctl(g_uiIoControl,MBUS_C5,0); 
	
	//if((channel>0) && (channel<7)){  //ֻ��1-6ͨ����MBUS,����ͨ��Ĭ�Ϲص�����MBUSͨ����
	//	OSTimeDly(10);
	//	ioctl(g_uiIoControl,MBUS_C0+channel-1,1); 
	//}

	OSTimeDly(10);

	switch(channel){
		case 1:
			ioctl(g_uiIoControl,MBUS_C1,1);  //�����ԣ��ϰ�ARM9������ͨ��1��2���Ʒ���
			break;
		case 2:
			ioctl(g_uiIoControl,MBUS_C0,1);
			break;
		case 3:
			ioctl(g_uiIoControl,MBUS_C2,1);
			break;
		case 4:
			ioctl(g_uiIoControl,MBUS_C3,1);
			break;
		case 5:
			ioctl(g_uiIoControl,MBUS_C4,1);
			break;
		case 6:
			ioctl(g_uiIoControl,MBUS_C5,1);
			break;
		default:
			break;

	}

	
}



/****************************************************************************************************
**	�� ��  �� ��: METER_ChangeChannel
**	�� ��  �� ��: �����������л�MBUSͨ�� 1--6
**	�� ��  �� ��: uint8 Channel -- ͨ����־
**	�� ��  �� ��: 
**   ��   ��   ֵ: NO_ERR
**	��		  ע: 
*****************************************************************************************************/
static uint8 gsu8Current_Channel = 0;//��ǰMBUS���ڵڼ�ͨ����0��ʾͨ��ȫ�ء�
uint8 METER_ChangeChannel(uint8 Channel)
{
	uint8 lu8ChannelChangeFlag = 0; //���ͨ���Ƿ�仯��0-û�䣬1-�仯��

    	switch(Channel){
		case 1:			
			if(gsu8Current_Channel != 1){
               	Mbus_ChangeChannel(1);
				gsu8Current_Channel = 1;
				lu8ChannelChangeFlag = 1;
			}
						
        	 break;
        
        	case 2:	
			if(gsu8Current_Channel != 2){
				Mbus_ChangeChannel(2);
				gsu8Current_Channel = 2;
				lu8ChannelChangeFlag = 1;
			}
						
        	break;
		
       	case 3:
			if(gsu8Current_Channel != 3){
            		Mbus_ChangeChannel(3);
				gsu8Current_Channel = 3;
				lu8ChannelChangeFlag = 1;
			}
						
        	break;
		
        	case 4:	
			if(gsu8Current_Channel != 4){
	        		Mbus_ChangeChannel(4);
				gsu8Current_Channel = 4;
				lu8ChannelChangeFlag = 1;
			}
						
        	break;
		
        	case 5:
			if(gsu8Current_Channel != 5){
            		Mbus_ChangeChannel(5);
				gsu8Current_Channel = 5;
				lu8ChannelChangeFlag = 1;

			}
						
        	break;
		
        	case 6:
			if(gsu8Current_Channel != 6){
            		Mbus_ChangeChannel(6);
				gsu8Current_Channel = 6;
				lu8ChannelChangeFlag = 1;
			}
			
       	break;
            		
        	case 7:
		if(gsu8Current_Channel != 7){
           	Mbus_ChangeChannel(7);
			gsu8Current_Channel = 7;
		 	lu8ChannelChangeFlag = 1;
			
        	}
			
        	break;
            	
        	default:
			Mbus_ChangeChannel(0);
			gsu8Current_Channel = 0;
          break;
			   
	}	
	
     //LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ChangeChannel> Change MBUS Channel Successful! Channel is %d", Channel);
	if(lu8ChannelChangeFlag == 1){  //ͨ�������仯ʱ��ʱ�������ȶ���
		OSTimeDly(OS_TICKS_PER_SEC);/* ͨ���л���ʱ̫�̵��±��޷��ɹ�*/
	}

    
	return NO_ERR; 
	
}






