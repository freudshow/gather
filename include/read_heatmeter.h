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


#pragma pack(1)
typedef struct{
	uint32   DailyHeat;		         /*����������*/	
   	uint8    DailyHeatUnit;
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


#pragma pack()

#pragma pack(1)

typedef struct lcModSendCmd{//����modbus����ȡ����֡
	uint8 address;//ģ���ַ��
	uint8 func;//������
	uint16 start;//��ʼ�Ĵ�����ַ
	uint16 regCount;//�Ĵ�������
}lcModSendStr;
#pragma pack()

#pragma pack(1)
typedef struct{//���������ֵ��ʽ
    float   pact_tot_elec;              //�����й��ܵ���
    uint8   pact_tot_elec_unit[5];      //+KWh
    float   nact_tot_elec;              //�����й��ܵ���
    uint8   nact_tot_elec_unit[5];      //-KWh
    float   preact_tot_elec;            //�����޹��ܵ���
    uint8   preact_tot_elec_unit[7];    //+KVarh
    float   nreact_tot_elec;            //�����޹��ܵ���
    uint8   nreact_tot_elec_unit[7];    //-KVarh
    float   act_tot_elec;               //�й��ܵ���
    uint8   act_tot_elec_unit[4];       //KWh
    float   react_tot_elec;             //�޹��ܵ���
    uint8   react_tot_elec_unit[6];     //KVarh    
}lcModbusElec_str;
#pragma pack()

typedef enum lcElecAnswerState{//�������Ӧ��֡�Ľ���״̬
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

