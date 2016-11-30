#ifndef WATER_H
#define	WATER_H


typedef struct {//水表数据的结构
	uint32 accumFlow;		//累积流量, BCD, xxxxxx.xx, 小端
	uint8  accumFlowUnit;	//累积流量单位, BCD, 0x2C-立方米
	uint32 flowRate;		//流速, BCD, xxxxxx.xx, 小端
	uint8  flowRateUnit;	//流速单位, BCD, 0x35-立方米/小时
	uint32 settleFlow;		//结算日累积流量, BCD, 0x2C-立方米
	uint32 accumWorkTime;	//累积工作时间, BCD
} waterDataStr;
typedef waterDataStr* waterDataPtr;

#endif//WATER_H