#ifndef WATER_H
#define	WATER_H


typedef struct {//ˮ�����ݵĽṹ
	uint32 accumFlow;		//�ۻ�����, BCD, xxxxxx.xx, С��
	uint8  accumFlowUnit;	//�ۻ�������λ, BCD, 0x2C-������
	uint32 flowRate;		//����, BCD, xxxxxx.xx, С��
	uint8  flowRateUnit;	//���ٵ�λ, BCD, 0x35-������/Сʱ
	uint32 settleFlow;		//�������ۻ�����, BCD, 0x2C-������
	uint32 accumWorkTime;	//�ۻ�����ʱ��, BCD
} waterDataStr;
typedef waterDataStr* waterDataPtr;

#endif//WATER_H