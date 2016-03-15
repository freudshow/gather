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



struct tm *p_gTimeNode;  //记录当前抄表时间节点信息。




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
	printf("MeterID = %d  .",pMeterInfo->f_device_id);
	printf("MeterAddr = ");
	for(i=0;i<7;i++)
		printf("%x",pMeterInfo->f_meter_address[i]);
	
	printf("  MeterType = %x  .",pMeterInfo->f_meter_type);
	printf("Protocol = %x  .",pMeterInfo->f_meter_proto_type);
	printf("Channel = %x  .",pMeterInfo->f_meter_channel);
	printf("Position is %s  .\n",pMeterInfo->f_install_pos);	

	//end:打印测试pMeter_info内容

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
	uint8 lu8retrytimes = 0;
	uint8 lu8Channel = 0;	//抄表对象所在通道。

	
	lu8Channel = pmf->u8Channel;
	if(lu8Channel == RS485_DOWN_CHANNEL)  //操作一个设备，先请求信号量,谨防冲突。
		sem_wait(&Opetate485Down_sem);
	else
		sem_wait(&OperateMBUS_sem);
		
	METER_ChangeChannel(lu8Channel);  //先确保在对应通道上。
	
	
	switch(pmf->u8MeterType){
		case HEATMETER:
			lu8retrytimes = 1 + 1;  //补抄次数，当前用固定1次补抄，后期要根据设置。
			for(i=0;i<lu8retrytimes;i++){
				err = Read_HeatMeter(pmf);
				OSTimeDly(200); //防止抄表太快，这里以后可以改成延时可设置。
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


	if(lu8Channel == RS485_DOWN_CHANNEL)  //操作完一个设备，释放对应信号量。
		sem_post(&Opetate485Down_sem);
	else
		sem_post(&OperateMBUS_sem);
	

	return err;


}








/*
  ******************************************************************************
  * 函数名称： pthread_ReadAllMeters(void)
  * 说    明： 抄所有表线程。
  * 参    数： 无
  ******************************************************************************
*/

void pthread_ReadAllMeters(void)
{
		time_t timep;


	while(1){
		//检测抄表信号，如果抄表信号有效，则开始全抄表，否则，等待30s吧，这些以后要补充，现在下面测试用。

		time(&timep); 
		p_gTimeNode = localtime(&timep);  
		p_gTimeNode->tm_year += 1900;
		p_gTimeNode->tm_mon += 1;  //转换成当前年和月。
		p_gTimeNode->tm_sec = 0;   //定时抄表节点，秒数固定写0.
		printf("%d %d %d \n",p_gTimeNode->tm_year, p_gTimeNode->tm_mon,p_gTimeNode->tm_mday); 
		printf("%d:%d:%d\n", p_gTimeNode->tm_hour, p_gTimeNode->tm_min, p_gTimeNode->tm_sec); 


		retrieve_meter_info_list(CallBack_ReadAllMeters);  //遍历抄全表。


		OSTimeDly(10000);

		
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






