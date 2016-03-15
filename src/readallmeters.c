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



struct tm *p_gTimeNode;  //��¼��ǰ����ʱ��ڵ���Ϣ��




/*
  ******************************************************************************
  * �������ƣ�static int CallBack_ReadAllMeters(void *p_Para, int argc, char **argv, char **azColName)
  * ˵    ������ѯ���ݿⳭ��ص���������ͨ�����������������������ص������д���
  		     ��̬������ֻ�����ڱ��ļ����á�
  * ��    ���� 
  ******************************************************************************
*/

static int CallBack_ReadAllMeters(pMeter_info pMeterInfo)
{
	uint8 i = 0;
	MeterFileType mf;  //��ֹ�۸ı������Ϣ�����������ֲ�����ʹ�á�

	//begin:��ӡ����pMeter_info����
	printf("MeterID = %d  .",pMeterInfo->f_device_id);
	printf("MeterAddr = ");
	for(i=0;i<7;i++)
		printf("%x",pMeterInfo->f_meter_address[i]);
	
	printf("  MeterType = %x  .",pMeterInfo->f_meter_type);
	printf("Protocol = %x  .",pMeterInfo->f_meter_proto_type);
	printf("Channel = %x  .",pMeterInfo->f_meter_channel);
	printf("Position is %s  .\n",pMeterInfo->f_install_pos);	

	//end:��ӡ����pMeter_info����

	mf.u16MeterID = pMeterInfo->f_device_id;
	memcpy(mf.u8MeterAddr,pMeterInfo->f_meter_address,LENGTH_B_METER_ADDRESS);
	mf.u8MeterType = pMeterInfo->f_meter_type;
	mf.u8ProtocolType = pMeterInfo->f_meter_proto_type;
	mf.u8Channel = pMeterInfo->f_meter_channel;
	memcpy(mf.install_pos,pMeterInfo->f_install_pos,LENGTH_F_INSTALL_POS);


	ReaOneMeter(&mf);



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
uint8 ReaOneMeter(MeterFileType *pmf)
{
	uint8 err = 0;
	uint8 i = 0;
	uint8 lu8retrytimes = 0;
	uint8 lu8Channel = 0;	//�����������ͨ����

	
	lu8Channel = pmf->u8Channel;
	if(lu8Channel == RS485_DOWN_CHANNEL)  //����һ���豸���������ź���,������ͻ��
		sem_wait(&Opetate485Down_sem);
	else
		sem_wait(&OperateMBUS_sem);
		
	METER_ChangeChannel(lu8Channel);  //��ȷ���ڶ�Ӧͨ���ϡ�
	
	
	switch(pmf->u8MeterType){
		case HEATMETER:
			lu8retrytimes = 1 + 1;  //������������ǰ�ù̶�1�β���������Ҫ�������á�
			for(i=0;i<lu8retrytimes;i++){
				err = Read_HeatMeter(pmf);
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


	if(lu8Channel == RS485_DOWN_CHANNEL)  //������һ���豸���ͷŶ�Ӧ�ź�����
		sem_post(&Opetate485Down_sem);
	else
		sem_post(&OperateMBUS_sem);
	

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
		time_t timep;


	while(1){
		//��Ⳮ���źţ���������ź���Ч����ʼȫ�������򣬵ȴ�30s�ɣ���Щ�Ժ�Ҫ���䣬������������á�

		time(&timep); 
		p_gTimeNode = localtime(&timep);  
		p_gTimeNode->tm_year += 1900;
		p_gTimeNode->tm_mon += 1;  //ת���ɵ�ǰ����¡�
		p_gTimeNode->tm_sec = 0;   //��ʱ����ڵ㣬�����̶�д0.
		printf("%d %d %d \n",p_gTimeNode->tm_year, p_gTimeNode->tm_mon,p_gTimeNode->tm_mday); 
		printf("%d:%d:%d\n", p_gTimeNode->tm_hour, p_gTimeNode->tm_min, p_gTimeNode->tm_sec); 


		retrieve_meter_info_list(CallBack_ReadAllMeters);  //������ȫ��


		OSTimeDly(10000);

		
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
**	�� ��  �� ��: �����������л�MBUSͨ�� 1--6,0ͨ����ر�����MBUSͨ����
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
            		
        	case 7:  //��7ͨ����485����ͨ�������ﲻ���κβ�����
			
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






