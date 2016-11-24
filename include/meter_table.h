/*
  *******************************************************************************
  * @file    meter_table.h
  * @author  zjjin
  * @version V0.0.0
  * @date    03-08-2016
  * @brief
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/
#ifndef        _METER_TABLE_H_
#define		_METER_TABLE_H_


#define	HEATMETER_PROTO_SUM		39	//������Э��������
#define	ELECMETER_PROTO_SUM		4	//���Э��������


#define     COMSET_1		0
#define	  COMSET_2		1
#define	  COMSET_3		2
#define     COMSET_4        	3
#define     COMSET_5        	4
#define     COMSET_6        	5
#define     COMSET_7        	6

#define     ELEC_COMSET_1        	0


#define DE_LU			COMSET_1
#define TIAN_GANG		COMSET_1
#define LI_CHUANG		COMSET_1
#define DAN_FUSEN		COMSET_1
#define WAN_HUA		COMSET_2

#define WANHUA_VER		    1	//����򻪡�
#define HYDROMETER775_VER    6
#define ZENNER_VER           7    	//�����ȱ�
#define LANDISGYR_VER        8   	//������
#define ENGELMANN_VER        9   	//������


#define NUANLIU_VER		    	17	//ů��
#define LIANQIANG_VER485    	19   //��ǿ485 �ν���485

#define HAIWEICI_VER         	20   //������
#define JINGWEI_VER          	22   //��γ
#define LIANQIANG_VERMBUS   	23   //��ǿmbus
#define LI_CHUANG_2			24 	//�����ڶ��ֱ�
#define JICHENG_VER			25 	//����������
#define ZHENYU_VER_2		26 	//ZHENYU2������
#define JINGWEIFE_VER  		27 	//��γ����ʶFE�汾Э��


#define YITONGDA_VER			28				//�����ͨ��
#define WANHUA_TO_DELU_VER      	30		//�����ת�ɵ�³Э���ϴ�
#define ZHENYU_S_VER			31      //����������S�Ͷ�һ���ۼ�����5�ֽڡ�
#define MITE_VER				32		//��������������
#define TIANGANG_EURO_VER01		33		//�������ŷ��1Э��(�����ú�С��).
#define CHAOYI_WATERMETER_VER		34		//34���㽭����ˮ������ɵ�³�ȱ��ʽ�ϴ����ݣ������ֶ�д0.
#define MITE_21XX_VER02			35		//��������21XX������
#define LANDISGYR_F4_VER			36		//������F4������
#define LANDISGYR_T230_VER		37	     //������T230������
#define BEITE_BEFORE2014			38		//����2014�꼰��ǰ�Ĵ�ھ��ȱ�Э��(���չ����ְ칫¥)

extern uint8 (*METER_ComParaSetArray[])(void);
extern uint16 gMETER_Table[HEATMETER_PROTO_SUM][4];

#endif  //_METER_TABLE_H_

