/*
  *******************************************************************************
  * @file    read_heatmeters.c 
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

#include "includes.h"
#include "readallmeters.h"
#include "read_heatmeter.h"
#include "meter_table.h"
#include "commap.h"






/*
  ******************************************************************************
  * 函数名称：void CreateFrame_CJ188(MeterFileType *pmf,uint8 *pDataTemp,uint8 *plen)
  * 说    明：
  * 参    数： 
  			
  ******************************************************************************
*/
static uint8 gMETER_FrameSer = 0x00;

void CreateFrame_CJ188(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
	//uint16 lu8BufSize = *plen;   //pDataTemp指针指向空间的大小，以防超限,当前因256肯定够用，暂没做处理，以后完善。
	DELU_Protocol	ProtocoalInfo;
	uint8 *pTemp = pDataTemp;
	uint8 *pCSStart; //记录校验计算起始位置。
	uint8 lu8FrameLen = 0;
	uint8 lu8CheckLen = 0;
	uint8 lu8CS = 0;
	uint8 lu8TempLen = 0;

	ProtocoalInfo.PreSmybolNum  = gMETER_Table[pmf->u8ProtocolType][2];
	ProtocoalInfo.MeterType = pmf->u8MeterType;
	memcpy(ProtocoalInfo.MeterAddr,  pmf->u8MeterAddr, 7);
	ProtocoalInfo.ControlCode 	= 0x01;
	ProtocoalInfo.Length		= 0x03;
	ProtocoalInfo.DataIdentifier= gMETER_Table[pmf->u8ProtocolType][1];

	pTemp = pDataTemp;
	
	memset(pTemp, 0xFE, ProtocoalInfo.PreSmybolNum);		//填入前导字符0xFE, 个数视情况而定
	pTemp += ProtocoalInfo.PreSmybolNum;
	lu8FrameLen += ProtocoalInfo.PreSmybolNum;
	
	pCSStart = pTemp;							//预留计算校验位
	*pTemp++ = DELU_FRAME_START;					//组建 帧头 0x68
	*pTemp++ = ProtocoalInfo.MeterType;					//仪表类型
	memcpy(pTemp, ProtocoalInfo.MeterAddr, 7);				//组建 地址域
	pTemp += 7;
	lu8FrameLen += 9;
	lu8CheckLen += 9;
	
	*pTemp++ = ProtocoalInfo.ControlCode;					//组建 控制码
	*pTemp++ = ProtocoalInfo.Length;						//组建 数据域长度
	lu8FrameLen += 2;
	lu8CheckLen += 2;
	
	*pTemp++ = ProtocoalInfo.DataIdentifier;
	*pTemp++ = (ProtocoalInfo.DataIdentifier) >> 8;		//组建 数据标识符
	*pTemp++ = gMETER_FrameSer++;
	lu8FrameLen += 3;
	lu8CheckLen += 3;
	
	lu8TempLen =ProtocoalInfo.Length - 3;
	memcpy(pTemp, ProtocoalInfo.DataBuf, lu8TempLen);
	pTemp += lu8TempLen;
	lu8FrameLen += lu8TempLen;
	lu8CheckLen += lu8TempLen;
	
	lu8CS = PUBLIC_CountCS(pCSStart, lu8CheckLen);			//计算 校验字节
	*pTemp++ = lu8CS;									//组建 校验位
	*pTemp++ = 0x16;									//组建 结束位
	lu8FrameLen += 2;									//组建 数据帧长度

	*pTemp++ = 0x16;	//联强485总线1200bps调,最后一字节总是不对，组帧多放一字节。
	lu8FrameLen += 1;	
	
	*plen = lu8FrameLen;							//返回数据帧长度



}






/****************************************************************************************************
**	函 数  名 称: METER_ReceiveFrame
**	函 数  功 能: 集中器中的计量模块接收上位机下发的对其操作的命令
**	输 入  参 数: uint8 dev -- 下行通道设备; 
**                uint16 Out_Time -- 接收命令超时等待时间
**	输 出  参 数: uint8 *buf -- 输出数据指针; 
**                uint8 *datalen -- 输出接收到数据帧长度
**  返   回   值: 0 -- 接收到有效帧;		1 -- 数据帧头不正确; 		2 -- 接收到的数据帧不是本电能表地址
**				  4 -- 数据帧长度不够		5 -- 数据帧校验不正确		6 -- 数据帧尾不正确
**	备		  注: 此处输出参数最好用联合体或宏定义，错误的返回值
*****************************************************************************************************/

uint8 CJ188_ReceiveFrame(uint8 dev,uint16 Out_Time,uint8 *buf,uint16 *datalen)
{
	uint8 data 	= 0x00;
	uint8 len	= 0x00;
	uint8 Cs	= 0x00;
	uint32 i,j;
	uint8 lu8FEnum = 0;//记录数据帧头FE的数量。

	
	*datalen = 0;
	   	
	i = 500;
	lu8FEnum = 0;
	while(i--){	//找帧头
		if(DownDevGetch(dev, &data, Out_Time)) 
			return 1;

		if(data == 0xfe)
			lu8FEnum += 1;
		else	if(data == 0x68){	
			if(lu8FEnum >= 2)
				break; 
			else
				lu8FEnum = 0;
		}
		else
			lu8FEnum = 0;
		
	}
	

	if(i == 0)   //说明没找到头，直接返回1.
		return 1;
	
	
	Cs 		= data;
	*buf++ 	= data;
	
	if(DownDevGetch(dev, &data, Out_Time))  		
		return 2;		//表类型
	Cs 		+= data;
	*buf++	 = data;
	
    for(i=0; i<7; i++)												//地址
    {
		if(DownDevGetch(dev, &data, Out_Time))  	
			return 3;
		Cs 		+= data;
		*buf++	 = data;
	}
	
	if(DownDevGetch(dev, &data, Out_Time))  		
		return 4;		//控制码
	Cs 		+= data;
	*buf++	 = data;
	
	if(DownDevGetch(dev, &len, Out_Time))  		
		return 5;		//数据长度
	if(len > METER_FRAME_LEN_MAX)             		
		return 10;
	Cs 		+= len;
	*buf++	 = len;

	for(j=0; j<len; j++)											//数据域
	{
	    if(DownDevGetch(dev, &data, Out_Time))  	
			return 6;
		*buf++  = data;
		Cs     += data;
    }  
     
	if(DownDevGetch(dev, &data, Out_Time))  		
		return 7;		//校验字节
	if(data != Cs)         							
		return 11; 
	
	*buf++	= data; 
	
	if(DownDevGetch(dev, &data, Out_Time))  		
		return 8;		//结束符
	if(data != 0x16)         						
		return 12;
	   
  	*buf++		= data; 
  	*datalen 	= len + 16;
  	  
    return NO_ERR;
    
}



/*
  ******************************************************************************
  * 函数名称：HeatMeter_DataSendAndRec(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
  * 说    明：发送抄表数据帧，等待返回数据，并返回解析是否正确。
  * 参    数： *pmf 表信息;
  			*pDataTemp 抄表返回数据存放位置指针；
  			*plen 抄表返回数据有效长度。
  			返回 成功 or 失败。  			
  ******************************************************************************
*/

uint8 HeatMeter_DataSendAndRec(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
	uint8 err = 0;
	uint32 lu32len = *plen;
	uint8 lu8protocoltype = 0;
	uint16 lu16OutTime = 2000;//毫秒。
	uint8 lu8DownComDev = DOWN_COMMU_DEV_485;

	if(pmf->u8Channel == RS485_DOWN_CHANNEL)
		lu8DownComDev = DOWN_COMMU_DEV_485;
	else
		lu8DownComDev = DOWN_COMMU_DEV_MBUS;

	DownDevSend(lu8DownComDev,pDataTemp,lu32len);  //发送数据。

	lu8protocoltype = pmf->u8ProtocolType;

	switch(lu8protocoltype){
		case HYDROMETER775_VER:
		case ZENNER_VER:

			break;
		

		default://默认抄CJ188格式。
			err = CJ188_ReceiveFrame(lu8DownComDev,lu16OutTime,pDataTemp,plen);
			//printf("CJ188_ReceiveFrame err = %d.\n",err);  //测试用
			break;



	}	


	return err;
	
}





/****************************************************************************************************
**	函 数  名 称: uint8 Meter_AnalDataFrm_CJ188(uint8 *pRecFrame,uint8 *pLen)
**	函 数  功 能: 将符合CJ188的设备返回数据处理，取出有效数据，并返回。
**	输 入  参 数: 
**	输 出  参 数: 
**   返   回   值: 
**	备		注: 
*****************************************************************************************************/
uint8 Meter_AnalDataFrm_CJ188(uint8 *pRecFrame,uint8 *pLen)
{
	uint8 *pTemp		= pRecFrame;
	DELU_Protocol ProtoclData;
		
	
	if(0x68 != *pTemp++)	return 1; 
			
	ProtoclData.MeterType = *pTemp++;						//仪表类型
	memcpy(ProtoclData.MeterAddr, pTemp, 7);					//仪表地址
	pTemp += 7;
			
	ProtoclData.ControlCode	= *pTemp++;					//控制码
	ProtoclData.Length		= *pTemp++;					//长度
	ProtoclData.DataIdentifier = *pTemp++;
	ProtoclData.DataIdentifier |= (*pTemp++ << 8);
	ProtoclData.SER			= *pTemp++;
	memcpy(ProtoclData.DataBuf, pTemp, (ProtoclData.Length-3));	//数据域数据
	pTemp += ProtoclData.Length - 3;
	
	pRecFrame = ProtoclData.DataBuf;
	*pLen = ProtoclData.Length - 3;
	
	return NO_ERR;
	
}



/*
  ******************************************************************************
  * 函数名称：uint8 HeatMeter_DataDeal(MeterFileType *pmf,uint8 *pDataBuf,uint16 *pLen,CJ188_Format *pCJ188Data)
  * 说    明：处理抄表返回的数据，并处理成统一CJ188格式。
  * 参    数：
  			
  ******************************************************************************
*/

uint8 HeatMeter_DataDeal(MeterFileType *pmf,uint8 *pDataBuf,uint16 *pLen,CJ188_Format *pCJ188Data)
{
	uint8 err = 0;
	uint8 lu8len = 0;
	uint8 lu8ProtocolType = 0;
	//CJ188_Format  MeterData={0,0x05,0,0x05,0,0x17,0,0x35,0,0x2c};

	lu8ProtocolType = pmf->u8ProtocolType;

	switch(lu8ProtocolType){



		default:{  //符合德鲁CJ188协议的，默认。
			err = Meter_AnalDataFrm_CJ188(pDataBuf,&lu8len);
			memcpy(pCJ188Data,pDataBuf,sizeof(CJ188_Format));
		}
		break;

	}

	
	return err;

}






/*
  ******************************************************************************
  * 函数名称：Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo)
  * 说    明：组建抄表数据帧，抄热表,并将不同厂家的热表返回数据处理成固定统一格式。
  * 参    数： *pmf 表信息。
  			
  ******************************************************************************
*/

uint8 HeatMeterCommunicate(MeterFileType *pmf,CJ188_Format *pCJ188Data)
{
	uint8 lu8DataTemp[256] = {0};  //欧标比较长，这里放256.
	uint16 lu16len = 0;
	uint8 err = 0;

	if(pmf->u8ProtocolType == HYDROMETER775_VER || pmf->u8ProtocolType == ZENNER_VER){  //一些欧标的表

	}
	else{	//CJ188协议的表，都在这里处理。	
		lu16len = sizeof(lu8DataTemp);  //CreateFrame_CJ188()中传入存储空间大小，以免超限。
		CreateFrame_CJ188(pmf,lu8DataTemp,&lu16len);  //创建抄表帧。
		err = HeatMeter_DataSendAndRec(pmf,lu8DataTemp,&lu16len);
		if(err == NO_ERR){
			err = HeatMeter_DataDeal(pmf,lu8DataTemp,&lu16len,pCJ188Data);//解析接收到的数据，并处理成统一CJ188格式。
		}

		
	}
	

	return err;

}





/*
  ******************************************************************************
  * 函数名称：Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo)
  * 说    明：抄热表处理函数
  * 参    数： *pmf 表信息。
  			*pPositionInfo 安装位置信息。
  ******************************************************************************
*/

uint8 Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo)
{
	uint8 err = 0;
	uint8 lu8retrytimes = 0;
	//DELU_Protocol	ProtocoalInfo;
	CJ188_Format CJ188_Data;
	

	if(pmf->u8ProtocolType >= HEATMETER_PROTO_SUM){  //防止协议版本号超限。
		//协议版本号超限处理。
	}		

	//根据协议版本号，设置对应端口参数.
	lu8retrytimes = 3;
	do{
		err = (*METER_ComParaSetArray[gMETER_Table[pmf->u8ProtocolType][0]])();  //按照抄表协议，设置抄表串口参数。
		lu8retrytimes --;

	}while((err != TRUE) && (lu8retrytimes > 0));
	
	//组建抄表数据帧，抄热表,并将不同厂家的热表返回数据处理成固定统一格式。
	err = HeatMeterCommunicate(pmf,&CJ188_Data);

	//根据t_request_data参数设置，将抄表数据存入对应数据库表格。
	if(err == NO_ERR){


	}

	



	return err;

}










