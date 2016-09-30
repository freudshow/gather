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



struct tm gTimeNode;  //记录当前抄表时间节点信息。




/*
  ******************************************************************************
  * 函数名称：static int CallBack_ReadAllMeters(void *p_Para, int argc, char **argv, char **azColName)
  * 说    明：查询数据库抄表回调函数，按通道抄表，抄所有种类表都在这个回调函数中处理。
  		     静态函数，只允许在本文件调用。
  * 参    数： 
  ******************************************************************************
*/

static int CallBack_ReadAllMeters(pMeter_info pMeterInfo)
{
	uint8 i = 0;
	MeterFileType mf;  //防止篡改表基础信息链表，拷贝到局部变量使用。

	//begin:打印测试pMeter_info内容
	printf("[%s][%s][%d]MeterID = %d  .", FILE_LINE, pMeterInfo->f_device_id);
	printf("MeterAddr = ");
	for(i=0;i<7;i++)
		printf("%02x",pMeterInfo->f_meter_address[i]);
	
	printf("  MeterType = %02x  .",pMeterInfo->f_meter_type);
	printf("Protocol = %02x  .",pMeterInfo->f_meter_proto_type);
	printf("Channel = %02x  .",pMeterInfo->f_meter_channel);
	printf("Position is %s  .\n",pMeterInfo->f_install_pos);	

	//end:打印测试pMeter_info内容

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
  * 函数名称：ReadAllMeters(MeterFileType *pmf,char *pPositionInfo);
  * 说    明：抄表处理函数，抄表失败如果需要重复抄表，也在这个函数中处理。
  * 参    数： *pmf 表信息。
  			*pPositionInfo 安装位置信息。
  ******************************************************************************
*/
uint8 ReaOneMeter(MeterFileType *pmf)
{
	uint8 err = 0;
	uint8 i = 0;
	uint8 lu8retrytimes = READ_METER_TIMES;
	uint8 lu8Channel = pmf->u8Channel;//抄表对象所在通道。
	CJ188_Format CJ188_Data;
    lcModbusElec_str lcModStr;
	struct tm NowTime;
	time_t timep;

	if(lu8Channel == RS485_DOWN_CHANNEL)  //操作一个设备，先请求信号量,谨防冲突。
		sem_wait(&Opetate485Down_sem);
	else
		sem_wait(&OperateMBUS_sem);
	printf("[%s][%s][%d]lu8Channel: %d\n", FILE_LINE, lu8Channel);
	METER_ChangeChannel(lu8Channel);  //先确保在对应通道上。
	
    //获取当前时间		
    time(&timep);
    localtime_r(&timep, &NowTime);
    switch(pmf->u8MeterType){
    case HEATMETER:
        for(i=0;i<lu8retrytimes;i++){
            err = Read_HeatMeter(pmf, &CJ188_Data);
            OSTimeDly(200); //防止抄表太快，这里以后可以改成延时可设置。
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
            OSTimeDly(200); //防止抄表太快，这里以后可以改成延时可设置。
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

	if(lu8Channel == RS485_DOWN_CHANNEL)  //操作完一个设备，释放对应信号量。
		sem_post(&Opetate485Down_sem);
	else
		sem_post(&OperateMBUS_sem);

	return err;
}




/*
  ******************************************************************************
  * 函数名称：void ReadAllMeters(void)
  * 说    明： 抄所有表。
  * 参    数： 无
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
    gTimeNode.tm_sec = 0;   //露篓脢卤鲁颅卤铆陆脷碌茫拢卢脙毛脢媒鹿脤露篓脨麓0.
    memcpy((uint8 *)&nowTime,(uint8 *)&gTimeNode,sizeof(struct tm));
    nowTime.tm_year +=      1900;
    nowTime.tm_mon += 1;  //脳陋禄禄鲁脡碌卤脟掳脛锚潞脥脭脗隆拢

    printf("%d %d %d ",nowTime.tm_year, nowTime.tm_mon,nowTime.tm_mday);
    printf("- %d:%d:%d\n", nowTime.tm_hour, nowTime.tm_min, nowTime.tm_sec);

	printf("[%s][%s][%d]\n", FILE_LINE);
    
    retrieve_meter_info_list(CallBack_ReadAllMeters);  //卤茅脌煤鲁颅脠芦卤铆隆拢
	printf("[%s][%s][%d]\n", FILE_LINE);
    get_sys_config(CONFIG_REPORT_MODE, &sysconfig);
    if(RPT_ACTIVE == atoi(sysconfig.f_config_value)) {//如果是主动上报, 则发送上报消息
    lnetMod = g_sysConfigHex.netType;

	if(lnetMod != em_net_rs485){  //使用485组网，是不允许信息主动上推的，必须等待上位要数, 如果多个集中器都往485总线上推数据, 会引发冲突
	  	Qmsg.dev = UP_COMMU_DEV_GPRS;
		Qmsg.mtype = 1;  //不要写0，其他都可以。  抄完表之后自动上推
		Qmsg.functype = em_FUNC_RPTUP;
		char tmpstr[30];
		asctime_r(&gTimeNode, tmpstr);
		asc_to_datestr(tmpstr, Qmsg.timenode);
  		msgsnd(g_uiQmsgFd,&Qmsg,sizeof(QmsgType),0);
	}

    /*
  	Qmsg.mtype = 1;  //不要写0，其他都可以。  抄完表之后自动上推
  
  	lnetMod = g_sysConfigHex.netType;
  	if(lnetMod == 0)
	  	Qmsg.dev = UP_COMMU_DEV_GPRS;
  	else
	  	Qmsg.dev = UP_COMMU_DEV_485;    //使用485组网，是不允许信息主动上推的，必须等待上位要数，这里测试用。
  
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
  * 函数名称： pthread_ReadAllMeters(void)
  * 说    明： 抄所有表线程。
  * 参    数： 无
  ******************************************************************************
*/

uint32 gu32lastChkMin;//上次检测抄表时间点的分钟点
void pthread_ReadAllMeters(void)
{
    uint16 lu16ReadmeterMode = 0;
    uint16 lu16ReadmeterCycle = 0;
    uint32 lu32CheckCyc_S = 30;  //检测周期，单位秒。
    uint32 lu32CheckCnt = 0;  //检测周期计数。
    uint32 lu32CurMin = 0;//当前时间点相对于00:00时所经历的分钟数          	
    time_t rawtime;
    struct tm timeinfo;

	while(1){
        //检测抄表信号，如果抄表信号有效，则开始全抄表，否则，等待30s吧，这些以后要补充，现在下面测试用。
        lu16ReadmeterMode = g_sysConfigHex.collectMode;
        lu16ReadmeterCycle = g_sysConfigHex.collectCycle;
        if(lu16ReadmeterMode == 0){  //按周期自动抄表。
            //lu32CheckCnt = lu32CheckCnt % ((lu16ReadmeterCycle*60)/lu32CheckCyc_S);  //初始化后立即抄读一次。
            time (&rawtime);
            localtime_r(&rawtime, &timeinfo);
            lu32CurMin = timeinfo.tm_hour*60 + timeinfo.tm_min;
            //printf("[%s][%s][%d]current minutes: %d\n" , FILE_LINE, lu32CurMin);
            if((lu32CurMin%lu16ReadmeterCycle) == 0){
                if(timeinfo.tm_min != gu32lastChkMin) {
                    //如果本次检测的分钟点不是上次检测的分钟点, 则抄表
                    //这个检测成立的前提是检测周期比一个小时短
                    printf("[%s][%s][%d] now read all meters", FILE_LINE);
                    ReadAllMeters();  //抄表。
                }
            }
            gu32lastChkMin = timeinfo.tm_min;
            lu32CheckCnt += 1;
        }
        else{  //按抄表时间节点抄表。


        }
        OSTimeDly(lu32CheckCyc_S*1000);//固定延时30秒钟一次检测。
    }
}


/*
  ******************************************************************************
  * 函数名称： void Mbus_ChangeChannel(uint8 channel)
  * 说    明： 切换MBUS通道
  * 参    数： 
  ******************************************************************************
*/
static void Mbus_ChangeChannel(uint8 channel)
{
	//uint8 i=0;
	//for(i=0; i<6; i++){  //先关掉所有通道
	//	 ioctl(g_uiIoControl,MBUS_C0+i,0); 
	//}

	ioctl(g_uiIoControl,MBUS_C0,0); //先关掉所有通道
	ioctl(g_uiIoControl,MBUS_C1,0); 
	ioctl(g_uiIoControl,MBUS_C2,0); 
	ioctl(g_uiIoControl,MBUS_C3,0); 
	ioctl(g_uiIoControl,MBUS_C4,0); 
	ioctl(g_uiIoControl,MBUS_C5,0); 
	
	//if((channel>0) && (channel<7)){  //只有1-6通道是MBUS,其他通道默认关掉所有MBUS通道。
	//	OSTimeDly(10);
	//	ioctl(g_uiIoControl,MBUS_C0+channel-1,1); 
	//}

	OSTimeDly(10);

	switch(channel){
		case 1:
			ioctl(g_uiIoControl,MBUS_C1,1);  //经测试，老版ARM9集中器通道1、2控制反。
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
**	函 数  名 称: METER_ChangeChannel
**	函 数  功 能: 集中器控制切换MBUS通道 1--6,0通道则关闭所有MBUS通道。
**	输 入  参 数: uint8 Channel -- 通道标志
**	输 出  参 数: 
**   返   回   值: NO_ERR
**	备		  注: 
*****************************************************************************************************/
static uint8 gsu8Current_Channel = 0;//当前MBUS处于第几通道。0表示通道全关。
uint8 METER_ChangeChannel(uint8 Channel)
{
	uint8 lu8ChannelChangeFlag = 0; //标记通道是否变化，0-没变，1-变化。

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
            		
        	case 7:  //第7通道是485下行通道，这里不做任何操作。
			
        	break;
            	
        	default:
			Mbus_ChangeChannel(0);
			gsu8Current_Channel = 0;
          break;
			   
	}	
	
     //LOG_WriteSysLog_Format(LOG_LEVEL_INFO, "INFO: <METER_ChangeChannel> Change MBUS Channel Successful! Channel is %d", Channel);
	if(lu8ChannelChangeFlag == 1){  //通道发生变化时延时待电气稳定。
		OSTimeDly(OS_TICKS_PER_SEC);/* 通道切换延时太短导致表无法成功*/
	}

    
	return NO_ERR; 
	
}






