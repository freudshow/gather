#ifndef WATER_H
#define	WATER_H

#define WATER_PROTO_SUM			1//水表的总协议数量

#define WATER_COMSET_1			0

#define WATER_MBUS			0//MBUS总线
#define WATER_RS485			1//485总线

#define WATER_PROTO_MODBUS	0//MODBUS协议
#define WATER_PROTO_CJ188	1//CJ188协议

#define WATER_XINGYUAN_DATAID	0x1F90//兴源水表的数据标识符
#define WATER_XINGYUAN_CTLCODE	0x01//兴源水表的控制码

#pragma	pack(push)
#pragma pack(1)

typedef struct{
	uint8	PreSymbolNum;	/*前导字符个数*/
	uint8   startCode;		/*起始符*/
	uint8  	MeterType;		/*仪表类型*/
	uint8 	MeterAddr[7];	/*热计量表地址*/
	uint8 	ControlCode;	/*控制码*/
	uint8 	Length;			/*数据域长度*/
	uint16	DataIdentifier;	/*数据标识符*/
	uint8 	SER;			/*序列号*/
	uint8	chkSum;			/*校验和*/
	uint8	endCode;		/*结束符*/
}waterCJ188Str;


typedef struct {//兴源水表的返回数据的结构
	uint32 accumFlow;		//累积流量, BCD, xxxxxx.xx, 小端
	uint8  accumFlowUnit;	//累积流量单位, 0x2C-立方米
	uint32 settleFlow;		//结算日累积流量, BCD
	uint8  settleFlowUnit;	//结算日累积流量单位, 0x2C-立方米
	uint8  time[7];			//时间, 未启用, 但保留位置
	uint8  unused[2];		//保留
} xingYuanStr;
typedef xingYuanStr* xingYuanPtr;

typedef struct {//水表数据的结构
	uint32 accumFlow;		//累积流量, BCD, xxxxxx.xx, 小端
	uint8  accumFlowUnit;	//累积流量单位, 0x2C-立方米
	uint32 flowRate;		//流速, BCD, xxxxxx.xx, 小端
	uint8  flowRateUnit;	//流速单位, BCD, 0x35-立方米/小时
	uint32 settleFlow;		//结算日累积流量, BCD, 0x2C-立方米
	uint8  settleFlowUnit;	//累积流量单位, BCD, 0x2C-立方米
	uint32 accumWorkTime;	//累积工作时间, 小时, BCD, 小端
} waterDataStr;
typedef waterDataStr* waterDataPtr;

#pragma	pack(pop)

typedef enum {//水表协议的索引号
	em_water_xingYuan = 0
} em_waterProtoIdx;

typedef enum {//index of gWATER_METER_Table's column
	em_water_table_comset = 0,
	em_water_table_dataID,
	em_water_table_prefixCnt,
	em_water_table_busType,
	em_water_table_protoType
}waterMeterTableIdx;

typedef enum cj188State{//cj188应答帧的解析状态
	em_cj188_init_state = 0,//初始状态
	em_cj188_startCode_state,//开始符状态
	em_cj188_meterType_state,//仪表类型状态
	em_cj188_address_state,	//仪表地址状态
	em_cj188_control_state,	//控制码状态
	em_cj188_length_state,	//数据长度状态
	em_cj188_data_state,	//数据状态
	em_cj188_chksum_state,	//校验值状态
	em_cj188_endCode_state	//结束符状态
}waterCJ188St;


extern uint8 ReadWaterMeter(MeterFileType *pmf, waterDataPtr pWaterData);
#endif//WATER_H
