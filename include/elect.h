#ifndef ELECT_H
#define ELECT_H


#define ELEC_PROTO_MODBUS	0//MODBUSЭ����
#define ELEC_PROTO_645		1//645Э����

#define ELEC_LCMODBUS		0//����Modbus���
#define ELEC_ACREL_DDSD1352	1//acrel DDSD1352������
#define ELEC_ACREL_DTSF1352	2//acrel DTSF1352�������ʽ���ܱ�
#define ELEC_ACREL_PZ80-E4C	3//acrel PZ80-E4C�ɱ�����ܵ������๦�ʡ�������ܲ��֣�

#define ELEC_LCMOD_RANGEV_REG  0x0001//����Modbus���, ��ѹ���̵ļĴ�����ַ. 1��1000V ��Ӧ 1��1000
#define ELEC_LCMOD_RANGEI_REG  0x0002//����Modbus���, �������̵ļĴ�����ַ. 0.1��1000 ��ֵ 1��10000 Ĭ��5AֵΪ50
#define ELEC_LCMOD_REG_BYTES       2//����DTSD106������, ÿ���Ĵ������ֽ���
#define ELEC_LCMOD_WORK_BYTES      4//����DTSD106������, ÿ�����ܲ�����Ӧ���ֽ���
#define ELEC_LCMOD_ALL_PARAM_REGS  0x0017//����Modbus���, �ӵ�ѹ���̼Ĵ������޹��ܵ��ܼĴ����ļĴ�������

#define ELEC_LCMOD_DEF_V    250//����Modbus���, Ĭ�ϵ�ѹ����
#define ELEC_LCMOD_DEF_I    5//����Modbus���, Ĭ�ϵ�������

#pragma pack(1)

typedef struct lcModSendCmd{//����modbus����ȡ����֡
	uint8 address;//ģ���ַ��
	uint8 func;//������
	uint16 start;//��ʼ�Ĵ�����ַ
	uint16 regCount;//�Ĵ�������
}lcModSendStr;

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

extern uint8 (*ELEC_METER_ComParaSetArray[])(void);
extern uint16 gELEC_METER_Table[ELECMETER_PROTO_SUM][5];
extern uint8 Read_ElecMeter(MeterFileType *pmf, lcModbusElec_str* pElecData);

#endif //ELECT_H
