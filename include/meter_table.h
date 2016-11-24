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


#define	HEATMETER_PROTO_SUM		39	//热量表协议总数。
#define	ELECMETER_PROTO_SUM		4	//电表协议总数。


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

#define WANHUA_VER		    1	//天津万华。
#define HYDROMETER775_VER    6
#define ZENNER_VER           7    	//真兰热表
#define LANDISGYR_VER        8   	//兰吉尔
#define ENGELMANN_VER        9   	//恩乐曼


#define NUANLIU_VER		    	17	//暖流
#define LIANQIANG_VER485    	19   //联强485 嘉洁能485

#define HAIWEICI_VER         	20   //海威茨
#define JINGWEI_VER          	22   //经纬
#define LIANQIANG_VERMBUS   	23   //联强mbus
#define LI_CHUANG_2			24 	//力创第二种表
#define JICHENG_VER			25 	//积成热量表
#define ZHENYU_VER_2		26 	//ZHENYU2热量表
#define JINGWEIFE_VER  		27 	//经纬带标识FE版本协议


#define YITONGDA_VER			28				//天津易通达
#define WANHUA_TO_DELU_VER      	30		//天津万华转成德鲁协议上传
#define ZHENYU_S_VER			31      //震宇热量表S型多一组累计热量5字节。
#define MITE_VER				32		//威海米特热量表。
#define TIANGANG_EURO_VER01		33		//威海天罡欧标1协议(威海颐和小区).
#define CHAOYI_WATERMETER_VER		34		//34，浙江超仪水表，处理成德鲁热表格式上传数据，无用字段写0.
#define MITE_21XX_VER02			35		//威海米特21XX热量表。
#define LANDISGYR_F4_VER			36		//兰吉尔F4热量表。
#define LANDISGYR_T230_VER		37	     //兰吉尔T230热量表。
#define BEITE_BEFORE2014			38		//贝特2014年及以前的大口径热表协议(日照公安局办公楼)

extern uint8 (*METER_ComParaSetArray[])(void);
extern uint16 gMETER_Table[HEATMETER_PROTO_SUM][4];

#endif  //_METER_TABLE_H_

