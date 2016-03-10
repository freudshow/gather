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
	uint8	PreSmybolNum;		/*ǰ���ַ�����*/
	uint8  	MeterType;		/*�Ǳ�����*/
	uint8 	MeterAddr[7];		/*�ȼ������ַ*/
	uint8 	ControlCode;		/*������*/
	uint8 	Length;			/*�����򳤶�*/
	uint16	DataIdentifier;	/*���ݱ�ʶ��*/
	uint8 	SER;				/*���к�*/
    	uint8  	DataBuf[METER_FRAME_LEN_MAX];
	}DELU_Protocol;



typedef struct{
	uint32   DailyHeat;		         /*����������*/	
   	uint8     DailyHeatUnit;
	uint32   CurrentHeat;		    /*��ǰ����*/
    	uint8     CurrentHeatUnit;
	uint32 	 HeatPower;		    /*�ȹ���*/
    	uint8     HeatPowerUnit;
	uint32 	 Flow;			    /*��������*/
   	uint8 	 FlowUnit;
	uint32 	AccumulateFlow;	    /*�ۻ�����*/
    	uint8 	AccumulateFlowUnit;	
	uint8	WaterInTemp[3];		/*��ˮ�¶�*/	
	uint8 	WaterOutTemp[3];		/*��ˮ�¶�*/
    	uint8 	AccumulateWorkTime[3];	/*�ۼƹ���ʱ��*/
	uint8	RealTime[7];		     /*ʵʱʱ��*/	
	uint16 	ST;					/*״̬ST*/  
	}CJ188_Format;







extern uint8 Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo);


#endif	//_READ_HEATMETER_

