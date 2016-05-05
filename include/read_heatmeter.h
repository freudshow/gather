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


#pragma pack(1)
typedef struct{
	uint32   DailyHeat;		         /*结算日热量*/	
   	uint8    DailyHeatUnit;
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


#pragma pack()

#pragma pack(1)

typedef struct lcModSendCmd{//力创modbus电表读取命令帧
	uint8 address;//模块地址号
	uint8 func;//功能码
	uint16 start;//起始寄存器地址
	uint16 regCount;//寄存器数量
}lcModSendStr;
#pragma pack()

#pragma pack(1)
typedef struct{//力创电表返回值格式
    float   pact_tot_elec;              //正向有功总电能
    uint8   pact_tot_elec_unit[5];      //+KWh
    float   nact_tot_elec;              //反向有功总电能
    uint8   nact_tot_elec_unit[5];      //-KWh
    float   preact_tot_elec;            //正向无功总电能
    uint8   preact_tot_elec_unit[7];    //+KVarh
    float   nreact_tot_elec;            //反向无功总电能
    uint8   nreact_tot_elec_unit[7];    //-KVarh
    float   act_tot_elec;               //有功总电能
    uint8   act_tot_elec_unit[4];       //KWh
    float   react_tot_elec;             //无功总电能
    uint8   react_tot_elec_unit[6];     //KVarh    
}lcModbusElec_str;
#pragma pack()

typedef enum lcElecAnswerState{//力创电表应答帧的解析状态
    em_init_state = 0,
    em_address_state,
    em_func_state,
    em_length_state,
    em_data_state,
    em_crc_state,
    em_end_state
}lcElecAnsSt;







extern uint8 Read_HeatMeter(MeterFileType *pmf, CJ188_Format* pCj188Data);
extern uint8 Read_ElecMeter(MeterFileType *pmf, lcModbusElec_str* pElecData);


#endif	//_READ_HEATMETER_

