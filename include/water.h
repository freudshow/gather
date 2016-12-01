#ifndef WATER_H
#define	WATER_H

#define WATER_PROTO_SUM			1//ˮ�����Э������

#define WATER_COMSET_1			0

#define WATER_MBUS			0//MBUS����
#define WATER_RS485			1//485����

#define WATER_PROTO_MODBUS	0//MODBUSЭ��
#define WATER_PROTO_CJ188	1//CJ188Э��

#define WATER_XINGYUAN_DATAID	0x1F90//��Դˮ������ݱ�ʶ��
#define WATER_XINGYUAN_CTLCODE	0x01//��Դˮ��Ŀ�����

#pragma	pack(push)
#pragma pack(1)

typedef struct {//��Դˮ��ķ������ݵĽṹ
	uint32 accumFlow;		//�ۻ�����, BCD, xxxxxx.xx, С��
	uint8  accumFlowUnit;	//�ۻ�������λ, 0x2C-������
	uint32 settleFlow;		//�������ۻ�����, BCD
	uint8  settleFlowUnit;	//�������ۻ�������λ, 0x2C-������
	uint8  time[7];			//ʱ��, δ����, ������λ��
	uint8  unused[2];		//����
} xingYuanStr;
typedef xingYuanStr* xingYuanPtr;

typedef struct {//ˮ�����ݵĽṹ
	uint32 accumFlow;		//�ۻ�����, BCD, xxxxxx.xx, С��
	uint8  accumFlowUnit;	//�ۻ�������λ, 0x2C-������
	uint32 flowRate;		//����, BCD, xxxxxx.xx, С��
	uint8  flowRateUnit;	//���ٵ�λ, BCD, 0x35-������/Сʱ
	uint32 settleFlow;		//�������ۻ�����, BCD, 0x2C-������
	uint8  settleFlowUnit;	//�ۻ�������λ, BCD, 0x2C-������
	uint32 accumWorkTime;	//�ۻ�����ʱ��, Сʱ, BCD, С��
} waterDataStr;
typedef waterDataStr* waterDataPtr;

#pragma	pack(pop)

typedef enum {//ˮ��Э���������
	em_water_xingYuan = 0
} em_waterProtoIdx;

typedef enum {//index of gELEC_WATER_Table's column
	em_elec_table_comset = 0,
	em_elec_table_dataID,
	em_elec_table_prefixCnt,
	em_elec_table_busType,
	em_elec_table_protoType
}elecMeterTableIdx;



extern uint8 ReadWaterMeter(MeterFileType *pmf, waterDataPtr pWaterData);
#endif//WATER_H
