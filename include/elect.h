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

#define ACREL_DTSF1352_PARA_START		0x0000//ACREL_DTSF1352�������������ʼ�Ĵ�����ַ
#define ACREL_DTSF1352_PARA_LENGTH		0x0050//ACREL_DTSF1352����������ļĴ�������

#define ACREL_PZ80_E4C_PARA_START		0x0000//ACREL_PZ80-E4C�������������ʼ�Ĵ�����ַ
#define ACREL_PZ80_E4C_PARA_LENGTH		0x004F//ACREL_PZ80-E4C����������ļĴ�������

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
	uint8 address;//ģ���ַ��
	uint8 func;//������
	uint8 dataLen;//���ݳ���
	uint32 activeWork;// �й��ܵ���, 0.01kWh
	uint32 activePeakWork;//�й����ܵ���, 0.01kWh
	uint32 activeFlatWork;//�й�ƽ�ܵ���, 0.01kWh
	uint32 activeValleyWork;//�й����ܵ���, 0.01kWh
	uint32 reverseActiveWork;//�����й��ܵ���, 0.01kWh
	uint16 passwd;//����
	uint16 voltage;//��ѹ, 0.1V
	uint16 current;//����, 0.01A
	int16 activePower;//�й�����, 0.001kW, -32767~32767
	int16 reactivePower;//�޹�����, 0.001kvar, -32767~32767
	uint16 apparentPower;//���ڹ���, 0.001kVA
	int16 powerFactor;//��������, 0.001, -1000~1000
	int16 frequency;//Ƶ��, 0.01Hz
	uint8  year;
	uint8  month;
	uint8  day;
	uint8  hour;
	uint8  minute;
	uint8  second;
	uint8  addr;//��ַ
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
	uint16 crcsum;
} acrelDDSD1352DataStr;
typedef acrelDDSD1352DataStr* acrelDDSD1352DataPtr;

typedef struct {//ACREL_DTSF1352��������ֵ�ṹ
	uint8 address;//ģ���ַ��
	uint8 func;//������
	uint8 dataLen;//���ݳ���
	uint32 activeWork;				//��ǰ���й�����
	uint32 activeSharpWork;			//��ǰ���й������
	uint32 activePeakWork;			//��ǰ���й������
	uint32 activeFlatWork;			//��ǰ���й�ƽ����
	uint32 activeValleyWork;		//��ǰ���й��ȵ���
	uint8  second;
	uint8  minute;
	uint8  hour;
	uint8  day;
	uint8  month;
	uint8  year;
	uint8  addr;					//��ַ
	uint8  baud;					//������, 00-9600, 01-4800, 02-2400, 03-1200
	uint8  firstOneRateNo;			//��1��ʱ��α�, ��1ʱ�η��ʺ�
	uint8  firstOneTimeEndMin;		//��1��ʱ��α�, ��1ʱ����ֹ: ��
	uint8  firstOneTimeEndHour;		//��1��ʱ��α�, ��1ʱ����ֹ: ʱ
	uint8  firstTwoRateNo;			//��1��ʱ��α�, ��2ʱ�η��ʺ�
	uint8  firstTwoTimeEndMin;		//��1��ʱ��α�, ��2ʱ����ֹ: ��
	uint8  firstTwoTimeEndHour;		//��1��ʱ��α�, ��2ʱ����ֹ: ʱ
	uint8  firstThreeRateNo;		//��1��ʱ��α�, ��3ʱ�η��ʺ�
	uint8  firstThreeTimeEndMin;	//��1��ʱ��α�, ��3ʱ����ֹ: ��
	uint8  firstThreeTimeEndHour;	//��1��ʱ��α�, ��3ʱ����ֹ: ʱ
	uint8  firstFourRateNo;			//��1��ʱ��α�, ��4ʱ�η��ʺ�
	uint8  firstFourTimeEndMin;		//��1��ʱ��α�, ��4ʱ����ֹ: ��
	uint8  firstFourTimeEndHour;	//��1��ʱ��α�, ��4ʱ����ֹ: ʱ
	uint8  firstFiveRateNo;			//��1��ʱ��α�, ��5ʱ�η��ʺ�
	uint8  firstFiveTimeEndMin;		//��1��ʱ��α�, ��5ʱ����ֹ: ��
	uint8  firstFiveTimeEndHour;	//��1��ʱ��α�, ��5ʱ����ֹ: ʱ
	uint8  firstSixRateNo;			//��1��ʱ��α�, ��6ʱ�η��ʺ�
	uint8  firstSixTimeEndMin;		//��1��ʱ��α�, ��6ʱ����ֹ: ��
	uint8  firstSixTimeEndHour;		//��1��ʱ��α�, ��6ʱ����ֹ: ʱ
	uint8  firstSevenRateNo;		//��1��ʱ��α�, ��7ʱ�η��ʺ�
	uint8  firstSevenTimeEndMin;	//��1��ʱ��α�, ��7ʱ����ֹ: ��
	uint8  firstSevenTimeEndHour;	//��1��ʱ��α�, ��7ʱ����ֹ: ʱ
	uint8  firstEigthtRateNo;		//��1��ʱ��α�, ��8ʱ�η��ʺ�
	uint8  firstEigthtTimeEndMin;	//��1��ʱ��α�, ��8ʱ����ֹ: ��
	uint8  firstEigthtTimeEndHour;	//��1��ʱ��α�, ��8ʱ����ֹ: ʱ
	uint8  secondOneRateNo;			//��2��ʱ��α�, ��1ʱ�η��ʺ�
	uint8  secondOneTimeEndMin;		//��2��ʱ��α�, ��1ʱ����ֹ: ��
	uint8  secondOneTimeEndHour;	//��2��ʱ��α�, ��1ʱ����ֹ: ʱ
	uint8  secondTwoRateNo;			//��2��ʱ��α�, ��2ʱ�η��ʺ�
	uint8  secondTwoTimeEndMin;		//��2��ʱ��α�, ��2ʱ����ֹ: ��
	uint8  secondTwoTimeEndHour;	//��2��ʱ��α�, ��2ʱ����ֹ: ʱ
	uint8  secondThreeRateNo;		//��2��ʱ��α�, ��3ʱ�η��ʺ�
	uint8  secondThreeTimeEndMin;	//��2��ʱ��α�, ��3ʱ����ֹ: ��
	uint8  secondThreeTimeEndHour;	//��2��ʱ��α�, ��3ʱ����ֹ: ʱ
	uint8  secondFourRateNo;		//��2��ʱ��α�, ��4ʱ�η��ʺ�
	uint8  secondFourTimeEndMin;	//��2��ʱ��α�, ��4ʱ����ֹ: ��
	uint8  secondFourTimeEndHour;	//��2��ʱ��α�, ��4ʱ����ֹ: ʱ
	uint8  secondFiveRateNo;		//��2��ʱ��α�, ��5ʱ�η��ʺ�
	uint8  secondFiveTimeEndMin;	//��2��ʱ��α�, ��5ʱ����ֹ: ��
	uint8  secondFiveTimeEndHour;	//��2��ʱ��α�, ��5ʱ����ֹ: ʱ
	uint8  secondSixRateNo;			//��2��ʱ��α�, ��6ʱ�η��ʺ�
	uint8  secondSixTimeEndMin;		//��2��ʱ��α�, ��6ʱ����ֹ: ��
	uint8  secondSixTimeEndHour;	//��2��ʱ��α�, ��6ʱ����ֹ: ʱ
	uint8  secondSevenRateNo;		//��2��ʱ��α�, ��7ʱ�η��ʺ�
	uint8  secondSevenTimeEndMin;	//��2��ʱ��α�, ��7ʱ����ֹ: ��
	uint8  secondSevenTimeEndHour;	//��2��ʱ��α�, ��7ʱ����ֹ: ʱ
	uint8  secondEigthtRateNo;		//��2��ʱ��α�, ��8ʱ�η��ʺ�
	uint8  secondEigthtTimeEndMin;	//��2��ʱ��α�, ��8ʱ����ֹ: ��
	uint8  secondEigthtTimeEndHour;	//��2��ʱ��α�, ��8ʱ����ֹ: ʱ
	uint8  secondNineRateNo;		//��2��ʱ��α�, ��9ʱ�η��ʺ�
	uint8  secondNineTimeEndMin;	//��2��ʱ��α�, ��9ʱ����ֹ: ��
	uint8  secondNineTimeEndHour;	//��2��ʱ��α�, ��9ʱ����ֹ: ʱ
	uint8  unused; 					//�Ĵ���0x0027��û���õ����ֽ�
	uint8  oneSegNo;				//��1ʱ��ʱ�α��
	uint8  oneSegStartDay;			//��1ʱ����ʼ����: ��
	uint8  oneSegStartMon;			//��1ʱ����ʼ����: ��
	uint8  twoSegNo;				//��2ʱ��ʱ�α��
	uint8  twoSegStartDay;			//��2ʱ����ʼ����: ��
	uint8  twoSegStartMon;			//��2ʱ����ʼ����: ��
	uint8  threeSegNo;				//��3ʱ��ʱ�α��
	uint8  threeSegStartDay;		//��3ʱ����ʼ����: ��
	uint8  threeSegStartMon;		//��3ʱ����ʼ����: ��
	uint8  fourSegNo;				//��4ʱ��ʱ�α��
	uint8  fourSegStartDay;			//��4ʱ����ʼ����: ��
	uint8  fourSegStartMon;			//��4ʱ����ʼ����: ��
	uint32 positiveActiveWork;		//��ǰ�������й�����, 0.01kWh
	uint32 positiveActiveSharpWork;	//��ǰ�����й������, 0.01kWh
	uint32 positiveActivePeakWork;	//��ǰ�����й������, 0.01kWh
	uint32 positiveActiveFlatWork;	//��ǰ�����й�ƽ����, 0.01kWh
	uint32 positiveActiveValleyWork;//��ǰ�����й��ȵ���, 0.01kWh
	uint32 negtiveActiveWork;		//��ǰ�����й��ܵ���, 0.01kWh
	uint32 negtiveActiveSharpWork;	//��ǰ�����й������, 0.01kWh
	uint32 negtiveActivePeakWork;	//��ǰ�����й������, 0.01kWh
	uint32 negtiveActiveFlatWork;	//��ǰ�����й�ƽ����, 0.01kWh
	uint32 negtiveActiveValleyWork;	//��ǰ�����й��ȵ���, 0.01kWh
	uint16  voltageA;				//A���ѹ, 0.1V
	uint16  voltageB;				//B���ѹ, 0.1V
	uint16  voltageC;				//C���ѹ, 0.1V
	uint16  currentA;				//A�����, 0.01A
	uint16  currentB;				//B�����, 0.01A
	uint16  currentC;				//C�����, 0.01A
	uint16  voltageAB;				//A-B�ߵ�ѹ
	uint16  voltageCB;				//C-B�ߵ�ѹ
	uint16  voltageAC;				//A-C�ߵ�ѹ
	uint16  voltageRatio;			//��ѹ���PT
	uint16  currentRatio;			//�������CT
	uint8   pressLossThreshold;		//ʧѹ��ֵ
	uint8   pressLossState;			//ʧѹ״̬
	uint16  pulseConst;				//���峣��
	uint16  runState;				//����״̬
	uint16 crcsum;
} acrelDTSF1352DataStr;
typedef acrelDTSF1352DataStr* acrelDTSF1352DataPtr;

typedef struct {//ACREL_PZ80_E4C��������ֵ�ṹ
	uint8 address;	//ģ���ַ��
	uint8 func;		//������
	uint8 dataLen;	//���ݳ���
	uint16 passwd;	//����
	uint8  addr;	//��ַ
	uint8  baud;	//������, 00-38400, 01-19200, 02-9600, 03-4800
	uint16 ctlWord;	//������
	uint16 PTRatio;	//PT���
	uint16 CTRatio;	//CT���
	uint8  oneOutSelect;	//��1·�������ѡ��
	uint16 oneOutScale;		//��1·�������
	uint8  twoOutSelect;	//��2·�������ѡ��
	uint16 twoOutScale;		//��2·�������
	uint8  threeOutSelect;	//��3·�������ѡ��
	uint16 threeOutScale;	//��3·�������
	uint8  fourOutSelect;	//��4·�������ѡ��
	uint16 fourOutScale;	//��4·�������
	uint16 unused1[6];		//����
	uint8  lightCtl;		//�������
	uint8  unused3;			//����
	uint16 unused2[16];		//����
	uint16 inputState;		//����������״̬
	uint8  DPT;				//С����U
	uint8  DCT;				//С����I
	uint8  DPQ;				//С����PQ
	uint8  PQ;				//����PQ
	uint16  voltageA;		//A���ѹ
	uint16  voltageB;		//B���ѹ
	uint16  voltageC;		//C���ѹ
	uint16  voltageAB;		//A-B�ߵ�ѹ
	uint16  voltageCB;		//C-B�ߵ�ѹ
	uint16  voltageAC;		//A-C�ߵ�ѹ
	uint16  currentA;		//A�����
	uint16  currentB;		//B�����
	uint16  currentC;		//C�����
	uint16  powerA;			//A���й���
	uint16  powerB;			//B���й���
	uint16  powerC;			//C���й���
	uint16  powertotal;		//���й���
	uint16  powerQA;		//A���޹���
	uint16  powerQB;		//B���޹���
	uint16  powerQC;		//C���޹���
	uint16  powerQtotal;	//���޹���
	uint16  pfA;			//A����������
	uint16  pfB;			//B����������
	uint16  pfC;			//C����������
	uint16  pftotal;		//����������
	uint16  sA;				//A�����ڹ���
	uint16  sB;				//B�����ڹ���
	uint16  sC;				//C�����ڹ���
	uint16  stotal;			//�����ڹ���
	uint16  frequency;		//Ƶ��
	uint32  absorbActive2;	//�����й����ܶ��β�
	uint32  freeActive2;	//�ͷ��й����ܶ��β�
	uint32  induceReactive2;//�����޹����ܶ��β�
	uint32	capReactive2;	//�����޹����ܶ��β�
	float32 absorbActive1;	//�����й�����һ�β�
	float32 freeActive1;	//�ͷ��й�����һ�β�
	float32 induceReactive1;//�����޹�����һ�β�
	float32 capReactive1;	//�����޹�����һ�β�
	uint16  crcsum;
} acrelPZ80_E4CDataStr;
typedef acrelPZ80_E4CDataStr* acrelPZ80_E4CDataPtr;

typedef struct {//�����ֵ��ʽ
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
