#ifndef ELECT_H
#define ELECT_H

#define	ELECMETER_PROTO_SUM		4	//���Э��������


#define     ELEC_COMSET_1        	0
#define		ELEC_COMSET_2			1
#define		ELEC_COMSET_3			2
#define		ELEC_COMSET_4			3


#define ELEC_PROTO_MODBUS	0//MODBUSЭ����
#define ELEC_PROTO_645		1//645Э����

#define ELEC_MBUS			0//MBUS���ߵ��
#define ELEC_RS485			1//485���ߵ��

#define LC_ELEC_WORK_START  0x000C//������������������ʼ�Ĵ�����ַ
#define LC_ELEC_WORK_LEN    0x000C//���������������ļĴ�������
#define ELEC_LCMOD_RANGEV_REG  0x0001//����Modbus���, ��ѹ���̵ļĴ�����ַ. 1��1000V ��Ӧ 1��1000
#define ELEC_LCMOD_RANGEI_REG  0x0002//����Modbus���, �������̵ļĴ�����ַ. 0.1��1000 ��ֵ 1��10000 Ĭ��5AֵΪ50
#define ELEC_LCMOD_REG_BYTES       2//����DTSD106������, ÿ���Ĵ������ֽ���
#define ELEC_LCMOD_WORK_BYTES      4//����DTSD106������, ÿ�����ܲ�����Ӧ���ֽ���
#define ELEC_LCMOD_ALL_PARAM_REGS  0x0017//����Modbus���, �ӵ�ѹ���̼Ĵ������޹��ܵ��ܼĴ����ļĴ�������

#define ELEC_LCMOD_DEF_V    250//����Modbus���, Ĭ�ϵ�ѹ����
#define ELEC_LCMOD_DEF_I    5//����Modbus���, Ĭ�ϵ�������

#define ACREL_DDSD1352_PARA_START		0x0000//ACREL_DDSD1352�������������ʼ�Ĵ�����ַ
#define ACREL_DDSD1352_PARA_LENGTH		0x003A//ACREL_DDSD1352����������ļĴ�������

#pragma pack(push)
#pragma pack(1)

typedef struct {//modbus��ȡ����֡
	uint8 address;//ģ���ַ��
	uint8 func;//������
	uint16 start;//��ʼ�Ĵ�����ַ
	uint16 regCount;//�Ĵ�������
	uint16 crcSum;//crcУ��
}modSendStr;

typedef struct {//ACREL_DDSD1352��������ֵ�ṹ
	uint32 activeWork;// �й��ܵ���, 0.01kWh
	uint32 activePeakWork;//�й����ܵ���, 0.01kWh
	uint32 activeFlatWork;//�й�ƽ�ܵ���, 0.01kWh
	uint32 activeValleyWork;//�й����ܵ���, 0.01kWh
	uint32 reverseActiveWork;//�����й��ܵ���, 0.01kWh
	uint16 passwd;//����
	uint16 voltage;//��ѹ, 0.1V
	uint16 current;//����, 0.01A
	uint16 activePower;//�й�����, 0.001kW, -32767~32767
	uint16 reactivePower;//�޹�����, 0.001kvar, -32767~32767
	uint16 apparentPower;//���ڹ���, 0.001kVA
	uint16 powerFactor;//��������, 0.001, -1000~1000
	uint8  year;
	uint8  month;
	uint8  day;
	uint8  hour;
	uint8  minute;
	uint8  second;
	uint8  address;//��ַ
	uint8  baud;//������, 00-9600, 01-4800, 02-2400, 03-1200
	uint8  oneRateNo;//��1ʱ�η��ʺ�, 00-��, 01-ƽ, 02-��, ��ͬ
	uint8  oneTimeEndMin;//��1ʱ����ֹ: ��
	uint8  oneTimeEndHour;//��1ʱ����ֹ: ʱ
	uint8  twoRateNo;//��2ʱ�η��ʺ�
	uint8  twoTimeEndMin;//��2ʱ����ֹ: ��
	uint8  twoTimeEndHour;//��2ʱ����ֹ: ʱ
	uint8  threeRateNo;//��3ʱ�η��ʺ�
	uint8  threeTimeEndMin;//��3ʱ����ֹ: ��
	uint8  threeTimeEndHour;//��3ʱ����ֹ: ʱ
	uint8  fourRateNo;//��4ʱ�η��ʺ�
	uint8  fourTimeEndMin;//��4ʱ����ֹ: ��
	uint8  fourTimeEndHour;//��4ʱ����ֹ: ʱ
	uint8  fiveRateNo;//��5ʱ�η��ʺ�
	uint8  fiveTimeEndMin;//��5ʱ����ֹ: ��
	uint8  fiveTimeEndHour;//��5ʱ����ֹ: ʱ
	uint8  sixRateNo;//��6ʱ�η��ʺ�
	uint8  sixTimeEndMin;//��6ʱ����ֹ: ��
	uint8  sixTimeEndHour;//��6ʱ����ֹ: ʱ
	uint8  sevenRateNo;//��7ʱ�η��ʺ�
	uint8  sevenTimeEndMin;//��7ʱ����ֹ: ��
	uint8  sevenTimeEndHour;//��7ʱ����ֹ: ʱ
	uint8  eigthtRateNo;//��8ʱ�η��ʺ�
	uint8  eigthtTimeEndMin;//��8ʱ����ֹ: ��
	uint8  eigthtTimeEndHour;//��8ʱ����ֹ: ʱ
	uint32 last1ActiveWork;//��1���й��ܵ���
	uint32 last1ActivePeakWork;//��1���й������
	uint32 last1ActiveFlatWork;//��1���й�ƽ����
	uint32 last1ActiveValleyWork;//��1���й��ȵ���
	uint32 last2ActiveWork;//��2���й��ܵ���
	uint32 last2ActivePeakWork;//��2���й������
	uint32 last2ActiveFlatWork;//��2���й�ƽ����
	uint32 last2ActiveValleyWork;//��2���й��ȵ���
	uint32 last3ActiveWork;//��3���й��ܵ���
	uint32 last3ActivePeakWork;//��3���й������
	uint32 last3ActiveFlatWork;//��3���й�ƽ����
	uint32 last3ActiveValleyWork;//��3���й��ȵ���
} acrelDDSD1352DataStr;
typedef acrelDDSD1352DataStr* acrelDDSD1352DataPtr;

typedef struct{//�����ֵ��ʽ
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
}elecMeterDataStr;
typedef elecMeterDataStr* elecMeterDataPtr;

#pragma pack(pop)

typedef enum {//index of gELEC_METER_Table's column
	em_elec_table_comset = 0,
	em_elec_table_dataID,
	em_elec_table_prefixCnt,
	em_elec_table_busType,
	em_elec_table_protoType
}elecMeterTableIdx;

typedef enum {//������ҵ�����
	em_lcModbus = 0,//����Modbus���
	em_acrel_DDSD1352,//acrel DDSD1352������
	em_acrel_DTSF1352,//acrel DTSF1352�������ʽ���ܱ�
	em_acrel_PZ80_E4C//acrel PZ80-E4C�ɱ�����ܵ������๦�ʡ�������ܲ��֣�
}electMeterIdx;

typedef enum lcElecAnswerState{//�������Ӧ��֡�Ľ���״̬
	em_init_state = 0,
	em_address_state,
	em_func_state,
	em_length_state,
	em_data_state,
	em_crc_state,
	em_end_state
}lcElecAnsSt;

extern uint8 (*ELEC_METER_ComParaSetArray[])(void);
extern uint16 gELEC_METER_Table[ELECMETER_PROTO_SUM][5];
extern uint8 Read_ElecMeter(MeterFileType *pmf, elecMeterDataStr* pElecData);

#endif //ELECT_H
