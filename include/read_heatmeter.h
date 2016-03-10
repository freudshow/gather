/*
  *******************************************************************************
  * @file    read_heatmeters.h
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
#ifndef	_READ_HEATMETER_
#define	_READ_HEATMETER_

#include "includes.h"



#define	DELU_FRAME_START			0x68



typedef struct{
	uint8	PreSmybolNum;		/*前导字符个数*/
	uint8  	MeterType;		/*仪表类型*/
	uint8 	MeterAddr[7];		/*热计量表地址*/
	uint8 	ControlCode;		/*控制码*/
	uint8 	Length;			/*数据域长度*/
	uint16	DataIdentifier;	/*数据标识符*/
	uint8 	SER;				/*序列号*/
    	uint8  	DataBuf[METER_FRAME_LEN_MAX];
	}DELU_Protocol;



typedef struct{
	uint32   DailyHeat;		         /*结算日热量*/	
   	uint8     DailyHeatUnit;
	uint32   CurrentHeat;		    /*当前热量*/
    	uint8     CurrentHeatUnit;
	uint32 	 HeatPower;		    /*热功率*/
    	uint8     HeatPowerUnit;
	uint32 	 Flow;			    /*流量流速*/
   	uint8 	 FlowUnit;
	uint32 	AccumulateFlow;	    /*累积流量*/
    	uint8 	AccumulateFlowUnit;	
	uint8	WaterInTemp[3];		/*供水温度*/	
	uint8 	WaterOutTemp[3];		/*回水温度*/
    	uint8 	AccumulateWorkTime[3];	/*累计工作时间*/
	uint8	RealTime[7];		     /*实时时间*/	
	uint16 	ST;					/*状态ST*/  
	}CJ188_Format;







extern uint8 Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo);


#endif	//_READ_HEATMETER_

