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





sqlite3 *p_sqlitedb;	//数据库句柄
uint8 gu8ReadAllMeterFlag = 0;  //用于标记是否正在抄全表，1-正在抄全表，0-没有在抄全表。
uint8 gu8DownComDev = 0; //记录使用哪一种下行设备,只能用于全抄表过程中，不允许用于指令控制/抄表。





/*
  ******************************************************************************
  * 函数名称：static int CallBack_ReadAllMeters(void *p_Para, int argc, char **argv, char **azColName)
  * 说    明：查询数据库抄表回调函数，按通道抄表，抄所有种类表都在这个回调函数中处理。
  		     静态函数，只允许在本文件调用。
  * 参    数： 
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
  * 函数名称：ReadAllMeters(MeterFileType *pmf,char *pPositionInfo);
  * 说    明：抄表处理函数，抄表失败如果需要重复抄表，也在这个函数中处理。
  * 参    数： *pmf 表信息。
  			*pPositionInfo 安装位置信息。
  ******************************************************************************
*/
uint8 ReadAllMeters(MeterFileType *pmf,char *pPositionInfo)
{
	uint8 err = 0;
	uint8 i = 0;
	uint8 lu8retrytimes = 0;
	uint8 lu8Channel = 0;	//抄表对象所在通道。

	
	lu8Channel = pmf->u8Channel;
	METER_ChangeChannel(lu8Channel);  //先确保在对应通道上。
	
	
	switch(pmf->u8MeterType){
		case HEATMETER:
			lu8retrytimes = 1 + 1;  //补抄次数，当前用固定1次补抄，后期要根据设置。
			for(i=0;i<lu8retrytimes;i++){
				err = Read_HeatMeter(pmf,pPositionInfo);
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
	int rec = 0;
	char SQ_Str[100] = "";
	char *zErrMsg = 0;


	uint8 i = 0;
	MeterFileType mf;
	char position[] = "3#-1-102";

	while(1){
		//检测抄表信号，如果抄表信号有效，则开始全抄表，否则，等待30s吧，这些以后要补充，现在下面测试用。

		//抄表按照通道，逐个通道抄表。

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


		//begin测试用
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



		//end测试用
		
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
**	函 数  功 能: 集中器控制切换MBUS通道 1--6
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
	if(lu8ChannelChangeFlag == 1){  //通道发生变化时延时待电气稳定。
		OSTimeDly(OS_TICKS_PER_SEC);/* 通道切换延时太短导致表无法成功*/
	}

    
	return NO_ERR; 
	
}






