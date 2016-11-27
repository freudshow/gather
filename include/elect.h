#ifndef ELECT_H
#define ELECT_H

#define	ELECMETER_PROTO_SUM		4	//电表协议总数。


#define     ELEC_COMSET_1        	0
#define		ELEC_COMSET_2			1
#define		ELEC_COMSET_3			2
#define		ELEC_COMSET_4			3


#define ELEC_PROTO_MODBUS	0//MODBUS协议电表
#define ELEC_PROTO_645		1//645协议电表

#define ELEC_MBUS			0//MBUS总线电表
#define ELEC_RS485			1//485总线电表

#define LC_ELEC_WORK_START  0x000C//力创电表电量参数的起始寄存器地址
#define LC_ELEC_WORK_LEN    0x000C//力创电表电量参数的寄存器个数
#define ELEC_LCMOD_RANGEV_REG  0x0001//力创Modbus电表, 电压量程的寄存器地址. 1～1000V 对应 1～1000
#define ELEC_LCMOD_RANGEI_REG  0x0002//力创Modbus电表, 电流量程的寄存器地址. 0.1～1000 数值 1～10000 默认5A值为50
#define ELEC_LCMOD_REG_BYTES       2//力创DTSD106三相电表, 每个寄存器的字节数
#define ELEC_LCMOD_WORK_BYTES      4//力创DTSD106三相电表, 每个电能参数对应的字节数
#define ELEC_LCMOD_ALL_PARAM_REGS  0x0017//力创Modbus电表, 从电压量程寄存器到无功总电能寄存器的寄存器个数

#define ELEC_LCMOD_DEF_V    250//力创Modbus电表, 默认电压量程
#define ELEC_LCMOD_DEF_I    5//力创Modbus电表, 默认电流量程

#define ACREL_DDSD1352_PARA_START		0x0000//ACREL_DDSD1352单项电表参数的起始寄存器地址
#define ACREL_DDSD1352_PARA_LENGTH		0x003A//ACREL_DDSD1352单项电表参数的寄存器长度

#pragma pack(push)
#pragma pack(1)

typedef struct {//modbus读取命令帧
	uint8 address;//模块地址号
	uint8 func;//功能码
	uint16 start;//起始寄存器地址
	uint16 regCount;//寄存器数量
	uint16 crcSum;//crc校验
}modSendStr;

typedef struct {//ACREL_DDSD1352单项电表返回值结构
	uint32 activeWork;// 有功总电能, 0.01kWh
	uint32 activePeakWork;//有功峰总电能, 0.01kWh
	uint32 activeFlatWork;//有功平总电能, 0.01kWh
	uint32 activeValleyWork;//有功谷总电能, 0.01kWh
	uint32 reverseActiveWork;//反向有功总电能, 0.01kWh
	uint16 passwd;//密码
	uint16 voltage;//电压, 0.1V
	uint16 current;//电流, 0.01A
	uint16 activePower;//有功功率, 0.001kW, -32767~32767
	uint16 reactivePower;//无功功率, 0.001kvar, -32767~32767
	uint16 apparentPower;//视在功率, 0.001kVA
	uint16 powerFactor;//功率因数, 0.001, -1000~1000
	uint8  year;
	uint8  month;
	uint8  day;
	uint8  hour;
	uint8  minute;
	uint8  second;
	uint8  address;//地址
	uint8  baud;//波特率, 00-9600, 01-4800, 02-2400, 03-1200
	uint8  oneRateNo;//第1时段费率号, 00-峰, 01-平, 02-谷, 下同
	uint8  oneTimeEndMin;//第1时段终止: 分
	uint8  oneTimeEndHour;//第1时段终止: 时
	uint8  twoRateNo;//第2时段费率号
	uint8  twoTimeEndMin;//第2时段终止: 分
	uint8  twoTimeEndHour;//第2时段终止: 时
	uint8  threeRateNo;//第3时段费率号
	uint8  threeTimeEndMin;//第3时段终止: 分
	uint8  threeTimeEndHour;//第3时段终止: 时
	uint8  fourRateNo;//第4时段费率号
	uint8  fourTimeEndMin;//第4时段终止: 分
	uint8  fourTimeEndHour;//第4时段终止: 时
	uint8  fiveRateNo;//第5时段费率号
	uint8  fiveTimeEndMin;//第5时段终止: 分
	uint8  fiveTimeEndHour;//第5时段终止: 时
	uint8  sixRateNo;//第6时段费率号
	uint8  sixTimeEndMin;//第6时段终止: 分
	uint8  sixTimeEndHour;//第6时段终止: 时
	uint8  sevenRateNo;//第7时段费率号
	uint8  sevenTimeEndMin;//第7时段终止: 分
	uint8  sevenTimeEndHour;//第7时段终止: 时
	uint8  eigthtRateNo;//第8时段费率号
	uint8  eigthtTimeEndMin;//第8时段终止: 分
	uint8  eigthtTimeEndHour;//第8时段终止: 时
	uint32 last1ActiveWork;//上1月有功总电能
	uint32 last1ActivePeakWork;//上1月有功峰电能
	uint32 last1ActiveFlatWork;//上1月有功平电能
	uint32 last1ActiveValleyWork;//上1月有功谷电能
	uint32 last2ActiveWork;//上2月有功总电能
	uint32 last2ActivePeakWork;//上2月有功峰电能
	uint32 last2ActiveFlatWork;//上2月有功平电能
	uint32 last2ActiveValleyWork;//上2月有功谷电能
	uint32 last3ActiveWork;//上3月有功总电能
	uint32 last3ActivePeakWork;//上3月有功峰电能
	uint32 last3ActiveFlatWork;//上3月有功平电能
	uint32 last3ActiveValleyWork;//上3月有功谷电能
} acrelDDSD1352DataStr;
typedef acrelDDSD1352DataStr* acrelDDSD1352DataPtr;

typedef struct{//电表返回值格式
	float   pact_tot_elec;              //正向有功总电能
	uint8   pact_tot_elec_unit[5];      //+KWh
	float   nact_tot_elec;              //反向有功总电能
	uint8   nact_tot_elec_unit[5];      //-KWh
	float   preact_tot_elec;            //正向无功总电能
	uint8   preact_tot_elec_unit[7];    //+KVarh
	float   nreact_tot_elec;            //反向无功总电能
	uint8   nreact_tot_elec_unit[7];    //-KVarh
	float   act_tot_elec;               //有功总电能
	uint8   act_tot_elec_unit[4];       //KWh
	float   react_tot_elec;             //无功总电能
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

typedef enum {//各电表厂家的类型
	em_lcModbus = 0,//力创Modbus电表
	em_acrel_DDSD1352,//acrel DDSD1352单相电表
	em_acrel_DTSF1352,//acrel DTSF1352三相电子式电能表
	em_acrel_PZ80_E4C//acrel PZ80-E4C可编程智能电测表（三相功率、三相电能部分）
}electMeterIdx;

typedef enum lcElecAnswerState{//力创电表应答帧的解析状态
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
