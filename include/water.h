#ifndef WATER_H
#define	WATER_H

#define WATER_PROTO_SUM			1//ˮ�����Э������


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

#endif//WATER_H