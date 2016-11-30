#ifndef WATER_H
#define	WATER_H

#define WATER_PROTO_SUM			1//水表的总协议数量


#define WATER_XINGYUAN_DATAID	0x1F90//兴源水表的数据标识符
#define WATER_XINGYUAN_CTLCODE	0x01//兴源水表的控制码

#pragma	pack(push)
#pragma pack(1)

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

#endif//WATER_H