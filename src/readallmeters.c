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



struct tm gTimeNode;  //��¼��ǰ����ʱ��ڵ���Ϣ��




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
	printf("[%s][%s][%d]MeterID = %d  .", FILE_LINE, pMeterInfo->f_device_id);
	printf("MeterAddr = ");
	for(i=0;i<7;i++)
		printf("%02x",pMeterInfo->f_meter_address[i]);
	
	printf("  MeterType = %02x  .",pMeterInfo->f_meter_type);
	printf("Protocol = %02x  .",pMeterInfo->f_meter_proto_type);
	printf("Channel = %02x  .",pMeterInfo->f_meter_channel);
	printf("Position is %s  .\n",pMeterInfo->f_install_pos);	

	//end:��ӡ����pMeter_info����

	mf.u16MeterID = pMeterInfo->f_device_id;
	memcpy(mf.u8MeterAddr,pMeterInfo->f_meter_address,LENGTH_B_METER_ADDRESS);
	mf.u8MeterType = pMeterInfo->f_meter_type;
	mf.u8ProtocolType = pMeterInfo->f_meter_proto_type;
	mf.u8Channel = pMeterInfo->f_meter_channel;
	memcpy(mf.install_pos,pMeterInfo->f_install_pos,LENGTH_F_INSTALL_POS);

	printf("[%s][%s][%d]\n", FILE_LINE);
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
	uint8 lu8retrytimes = READ_METER_TIMES;
	uint8 lu8Channel = pmf->u8Channel;//�����������ͨ����
	CJ188_Format CJ188_Data;
    lcModbusElec_str lcModStr;
	struct tm NowTime;
	time_t timep;

	if(lu8Channel == RS485_DOWN_CHANNEL)  //����һ���豸���������ź���,������ͻ��
		sem_wait(&Opetate485Down_sem);
	else
		sem_wait(&OperateMBUS_sem);
	printf("[%s][%s][%d]lu8Channel: %d\n", FILE_LINE, lu8Channel);
	METER_ChangeChannel(lu8Channel);  //��ȷ���ڶ�Ӧͨ���ϡ�
	
    //��ȡ��ǰʱ��		
    time(&timep);
    localtime_r(&timep, &NowTime);
    switch(pmf->u8MeterType){
    case HEATMETER:
        for(i=0;i<lu8retrytimes;i++){
            err = Read_HeatMeter(pmf, &CJ188_Data);
            OSTimeDly(200); //��ֹ����̫�죬�����Ժ���Ըĳ���ʱ�����á�
            if(err == NO_ERR)
            break;
        }

        if(err == NO_ERR){
            err = insert_his_data(pmf, &CJ188_Data, &NowTime, &gTimeNode);
            printf("[%s][%s][%d]insert_his_data over.\n", FILE_LINE);
        } else {
            err = insert_his_data(pmf, NULL, &NowTime, &gTimeNode);
            printf("[%s][%s][%d]critical: not read response from meter!\n", FILE_LINE);
        }
        break;
    case WATERMETER:
			break;
    case ELECTMETER:
        for(i=0;i<lu8retrytimes;i++){
            err = Read_ElecMeter(pmf, &lcModStr);
            OSTimeDly(200); //��ֹ����̫�죬�����Ժ���Ըĳ���ʱ�����á�
            if(err == NO_ERR)
                break;
        }

        if(err == NO_ERR){
            err = insert_his_data(pmf, &lcModStr, &NowTime, &gTimeNode);
            printf("[%s][%s][%d]insert_his_data over.\n", FILE_LINE);
        } else {
            err = insert_his_data(pmf, NULL, &NowTime, &gTimeNode);
            printf("[%s][%s][%d]critical: not read response from meter!\n", FILE_LINE);
        }
        break;
    case GASMETER:
        break;
    default:
        err = ERR_1;
		printf("[%s][%s][%d]meter_type error: %d", FILE_LINE, pmf->u8MeterType);
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
  * �������ƣ�void ReadAllMeters(void)
  * ˵    ���� �����б�
  * ��    ���� ��
  ******************************************************************************
*/

void ReadAllMeters(void)
{
    time_t timep;
    struct tm nowTime;
    QmsgType Qmsg;
    memset(&Qmsg, 0, sizeof(QmsgType));
    net_mod lnetMod;
    sys_config_str sysconfig;

    time(&timep);
    localtime_r(&timep, &gTimeNode);
    gTimeNode.tm_sec = 0;   //¶¨Ê±³­±í½Úµã£¬ÃëÊý¹Ì¶¨Ð´0.
    memcpy((uint8 *)&nowTime,(uint8 *)&gTimeNode,sizeof(struct tm));
    nowTime.tm_year +=      1900;
    nowTime.tm_mon += 1;  //×ª»»³Éµ±Ç°ÄêºÍÔÂ¡£

    printf("%d %d %d ",nowTime.tm_year, nowTime.tm_mon,nowTime.tm_mday);
    printf("- %d:%d:%d\n", nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);

	printf("[%s][%s][%d]\n", FILE_LINE);
    
    retrieve_meter_info_list(CallBack_ReadAllMeters);  //±éÀú³­È«±í¡£
	printf("[%s][%s][%d]\n", FILE_LINE);
    get_sys_config(CONFIG_REPORT_MODE, &sysconfig);
    if(RPT_ACTIVE == atoi(sysconfig.f_config_value)) {//����������ϱ�, �����ϱ���Ϣ
    lnetMod = g_sysConfigHex.netType;

	if(lnetMod != em_net_rs485){  //ʹ��485�������ǲ�������Ϣ�������Ƶģ�����ȴ���λҪ��, ����������������485������������, ��������ͻ
	  	Qmsg.dev = UP_COMMU_DEV_GPRS;
		Qmsg.mtype = 1;  //��Ҫд0�����������ԡ�  �����֮���Զ�����
		Qmsg.functype = em_FUNC_RPTUP;
		char tmpstr[30];
		asctime_r(&gTimeNode, tmpstr);
		asc_to_datestr(tmpstr, Qmsg.timenode);
  		msgsnd(g_uiQmsgFd,&Qmsg,sizeof(QmsgType),0);
	}

    /*
  	Qmsg.mtype = 1;  //��Ҫд0�����������ԡ�  �����֮���Զ�����
  
  	lnetMod = g_sysConfigHex.netType;
  	if(lnetMod == 0)
	  	Qmsg.dev = UP_COMMU_DEV_GPRS;
  	else
	  	Qmsg.dev = UP_COMMU_DEV_485;    //ʹ��485�������ǲ�������Ϣ�������Ƶģ�����ȴ���λҪ������������á�
  
        Qmsg.functype = em_FUNC_RPTUP;
        char tmpstr[30];
        asctime_r(&gTimeNode, tmpstr);
        asc_to_datestr(tmpstr, Qmsg.timenode);
        printf("[%s][%s][%d] Qmsg.timenode: %s\n", FILE_LINE, Qmsg.timenode );
        msgsnd(g_uiQmsgFd,&Qmsg,sizeof(QmsgType),0);
        printf("[%s][%s][%d] Qmsg have been sent\n", FILE_LINE);
    */
    }
}



/*
  ******************************************************************************
  * �������ƣ� pthread_ReadAllMeters(void)
  * ˵    ���� �����б��̡߳�
  * ��    ���� ��
  ******************************************************************************
*/

uint32 gu32lastChkMin;//�ϴμ�Ⳮ��ʱ���ķ��ӵ�
void pthread_ReadAllMeters(void)
{
    uint16 lu16ReadmeterMode = 0;
    uint16 lu16ReadmeterCycle = 0;
    uint32 lu32CheckCyc_S = 30;  //������ڣ���λ�롣
    uint32 lu32CheckCnt = 0;  //������ڼ�����
    uint32 lu32CurMin = 0;//��ǰʱ��������00:00ʱ�������ķ�����          	
    time_t rawtime;
    struct tm timeinfo;

	while(1){
        //��Ⳮ���źţ���������ź���Ч����ʼȫ�������򣬵ȴ�30s�ɣ���Щ�Ժ�Ҫ���䣬������������á�
        lu16ReadmeterMode = g_sysConfigHex.collectMode;
        lu16ReadmeterCycle = g_sysConfigHex.collectCycle;
        if(lu16ReadmeterMode == 0){  //�������Զ�����
            //lu32CheckCnt = lu32CheckCnt % ((lu16ReadmeterCycle*60)/lu32CheckCyc_S);  //��ʼ������������һ�Ρ�
            time (&rawtime);
            localtime_r(&rawtime, &timeinfo);
            lu32CurMin = timeinfo.tm_hour*60 + timeinfo.tm_min;
            //printf("[%s][%s][%d]current minutes: %d\n" , FILE_LINE, lu32CurMin);
            if((lu32CurMin%lu16ReadmeterCycle) == 0){
                if(timeinfo.tm_min != gu32lastChkMin) {
                    //������μ��ķ��ӵ㲻���ϴμ��ķ��ӵ�, �򳭱�
                    //�����������ǰ���Ǽ�����ڱ�һ��Сʱ��
                    printf("[%s][%s][%d] now read all meters", FILE_LINE);
                    ReadAllMeters();  //����
                }
            }
            gu32lastChkMin = timeinfo.tm_min;
            lu32CheckCnt += 1;
        }
        else{  //������ʱ��ڵ㳭��


        }
        OSTimeDly(lu32CheckCyc_S*1000);//�̶���ʱ30����һ�μ�⡣
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






