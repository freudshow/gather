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

#define ACREL_DTSF1352_PARA_START		0x0000//ACREL_DTSF1352三项电表参数的起始寄存器地址
#define ACREL_DTSF1352_PARA_LENGTH		0x0050//ACREL_DTSF1352三项电表参数的寄存器长度

#define ACREL_PZ80_E4C_PARA_START		0x0000//ACREL_PZ80-E4C三项电表参数的起始寄存器地址
#define ACREL_PZ80_E4C_PARA_LENGTH		0x004F//ACREL_PZ80-E4C三项电表参数的寄存器长度

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
	uint8 address;//模块地址号
	uint8 func;//功能码
	uint8 dataLen;//数据长度
	uint32 activeWork;// 有功总电能, 0.01kWh
	uint32 activePeakWork;//有功峰总电能, 0.01kWh
	uint32 activeFlatWork;//有功平总电能, 0.01kWh
	uint32 activeValleyWork;//有功谷总电能, 0.01kWh
	uint32 reverseActiveWork;//反向有功总电能, 0.01kWh
	uint16 passwd;//密码
	uint16 voltage;//电压, 0.1V
	uint16 current;//电流, 0.01A
	int16 activePower;//有功功率, 0.001kW, -32767~32767
	int16 reactivePower;//无功功率, 0.001kvar, -32767~32767
	uint16 apparentPower;//视在功率, 0.001kVA
	int16 powerFactor;//功率因数, 0.001, -1000~1000
	int16 frequency;//频率, 0.01Hz
	uint8  year;
	uint8  month;
	uint8  day;
	uint8  hour;
	uint8  minute;
	uint8  second;
	uint8  addr;//地址
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
	uint16 crcsum;
} acrelDDSD1352DataStr;
typedef acrelDDSD1352DataStr* acrelDDSD1352DataPtr;

typedef struct {//ACREL_DTSF1352三项电表返回值结构
	uint8 address;//模块地址号
	uint8 func;//功能码
	uint8 dataLen;//数据长度
	uint32 activeWork;				//当前总有功电能
	uint32 activeSharpWork;			//当前总有功尖电能
	uint32 activePeakWork;			//当前总有功峰电能
	uint32 activeFlatWork;			//当前总有功平电能
	uint32 activeValleyWork;		//当前总有功谷电能
	uint8  second;
	uint8  minute;
	uint8  hour;
	uint8  day;
	uint8  month;
	uint8  year;
	uint8  addr;					//地址
	uint8  baud;					//波特率, 00-9600, 01-4800, 02-2400, 03-1200
	uint8  firstOneRateNo;			//第1套时间段表, 第1时段费率号
	uint8  firstOneTimeEndMin;		//第1套时间段表, 第1时段终止: 分
	uint8  firstOneTimeEndHour;		//第1套时间段表, 第1时段终止: 时
	uint8  firstTwoRateNo;			//第1套时间段表, 第2时段费率号
	uint8  firstTwoTimeEndMin;		//第1套时间段表, 第2时段终止: 分
	uint8  firstTwoTimeEndHour;		//第1套时间段表, 第2时段终止: 时
	uint8  firstThreeRateNo;		//第1套时间段表, 第3时段费率号
	uint8  firstThreeTimeEndMin;	//第1套时间段表, 第3时段终止: 分
	uint8  firstThreeTimeEndHour;	//第1套时间段表, 第3时段终止: 时
	uint8  firstFourRateNo;			//第1套时间段表, 第4时段费率号
	uint8  firstFourTimeEndMin;		//第1套时间段表, 第4时段终止: 分
	uint8  firstFourTimeEndHour;	//第1套时间段表, 第4时段终止: 时
	uint8  firstFiveRateNo;			//第1套时间段表, 第5时段费率号
	uint8  firstFiveTimeEndMin;		//第1套时间段表, 第5时段终止: 分
	uint8  firstFiveTimeEndHour;	//第1套时间段表, 第5时段终止: 时
	uint8  firstSixRateNo;			//第1套时间段表, 第6时段费率号
	uint8  firstSixTimeEndMin;		//第1套时间段表, 第6时段终止: 分
	uint8  firstSixTimeEndHour;		//第1套时间段表, 第6时段终止: 时
	uint8  firstSevenRateNo;		//第1套时间段表, 第7时段费率号
	uint8  firstSevenTimeEndMin;	//第1套时间段表, 第7时段终止: 分
	uint8  firstSevenTimeEndHour;	//第1套时间段表, 第7时段终止: 时
	uint8  firstEigthtRateNo;		//第1套时间段表, 第8时段费率号
	uint8  firstEigthtTimeEndMin;	//第1套时间段表, 第8时段终止: 分
	uint8  firstEigthtTimeEndHour;	//第1套时间段表, 第8时段终止: 时
	uint8  secondOneRateNo;			//第2套时间段表, 第1时段费率号
	uint8  secondOneTimeEndMin;		//第2套时间段表, 第1时段终止: 分
	uint8  secondOneTimeEndHour;	//第2套时间段表, 第1时段终止: 时
	uint8  secondTwoRateNo;			//第2套时间段表, 第2时段费率号
	uint8  secondTwoTimeEndMin;		//第2套时间段表, 第2时段终止: 分
	uint8  secondTwoTimeEndHour;	//第2套时间段表, 第2时段终止: 时
	uint8  secondThreeRateNo;		//第2套时间段表, 第3时段费率号
	uint8  secondThreeTimeEndMin;	//第2套时间段表, 第3时段终止: 分
	uint8  secondThreeTimeEndHour;	//第2套时间段表, 第3时段终止: 时
	uint8  secondFourRateNo;		//第2套时间段表, 第4时段费率号
	uint8  secondFourTimeEndMin;	//第2套时间段表, 第4时段终止: 分
	uint8  secondFourTimeEndHour;	//第2套时间段表, 第4时段终止: 时
	uint8  secondFiveRateNo;		//第2套时间段表, 第5时段费率号
	uint8  secondFiveTimeEndMin;	//第2套时间段表, 第5时段终止: 分
	uint8  secondFiveTimeEndHour;	//第2套时间段表, 第5时段终止: 时
	uint8  secondSixRateNo;			//第2套时间段表, 第6时段费率号
	uint8  secondSixTimeEndMin;		//第2套时间段表, 第6时段终止: 分
	uint8  secondSixTimeEndHour;	//第2套时间段表, 第6时段终止: 时
	uint8  secondSevenRateNo;		//第2套时间段表, 第7时段费率号
	uint8  secondSevenTimeEndMin;	//第2套时间段表, 第7时段终止: 分
	uint8  secondSevenTimeEndHour;	//第2套时间段表, 第7时段终止: 时
	uint8  secondEigthtRateNo;		//第2套时间段表, 第8时段费率号
	uint8  secondEigthtTimeEndMin;	//第2套时间段表, 第8时段终止: 分
	uint8  secondEigthtTimeEndHour;	//第2套时间段表, 第8时段终止: 时
	uint8  secondNineRateNo;		//第2套时间段表, 第9时段费率号
	uint8  secondNineTimeEndMin;	//第2套时间段表, 第9时段终止: 分
	uint8  secondNineTimeEndHour;	//第2套时间段表, 第9时段终止: 时
	uint8  unused; 					//寄存器0x0027中没有用到的字节
	uint8  oneSegNo;				//第1时区时段表号
	uint8  oneSegStartDay;			//第1时区起始日期: 日
	uint8  oneSegStartMon;			//第1时区起始日期: 月
	uint8  twoSegNo;				//第2时区时段表号
	uint8  twoSegStartDay;			//第2时区起始日期: 日
	uint8  twoSegStartMon;			//第2时区起始日期: 月
	uint8  threeSegNo;				//第3时区时段表号
	uint8  threeSegStartDay;		//第3时区起始日期: 日
	uint8  threeSegStartMon;		//第3时区起始日期: 月
	uint8  fourSegNo;				//第4时区时段表号
	uint8  fourSegStartDay;			//第4时区起始日期: 日
	uint8  fourSegStartMon;			//第4时区起始日期: 月
	uint32 positiveActiveWork;		//当前正向总有功电能, 0.01kWh
	uint32 positiveActiveSharpWork;	//当前正向有功尖电能, 0.01kWh
	uint32 positiveActivePeakWork;	//当前正向有功峰电能, 0.01kWh
	uint32 positiveActiveFlatWork;	//当前正向有功平电能, 0.01kWh
	uint32 positiveActiveValleyWork;//当前正向有功谷电能, 0.01kWh
	uint32 negtiveActiveWork;		//当前反向有功总电能, 0.01kWh
	uint32 negtiveActiveSharpWork;	//当前反向有功尖电能, 0.01kWh
	uint32 negtiveActivePeakWork;	//当前反向有功峰电能, 0.01kWh
	uint32 negtiveActiveFlatWork;	//当前反向有功平电能, 0.01kWh
	uint32 negtiveActiveValleyWork;	//当前反向有功谷电能, 0.01kWh
	uint16  voltageA;				//A相电压, 0.1V
	uint16  voltageB;				//B相电压, 0.1V
	uint16  voltageC;				//C相电压, 0.1V
	uint16  currentA;				//A相电流, 0.01A
	uint16  currentB;				//B相电流, 0.01A
	uint16  currentC;				//C相电流, 0.01A
	uint16  voltageAB;				//A-B线电压
	uint16  voltageCB;				//C-B线电压
	uint16  voltageAC;				//A-C线电压
	uint16  voltageRatio;			//电压变比PT
	uint16  currentRatio;			//电流变比CT
	uint8   pressLossThreshold;		//失压阈值
	uint8   pressLossState;			//失压状态
	uint16  pulseConst;				//脉冲常数
	uint16  runState;				//运行状态
	uint16 crcsum;
} acrelDTSF1352DataStr;
typedef acrelDTSF1352DataStr* acrelDTSF1352DataPtr;

typedef struct {//ACREL_PZ80_E4C三项电表返回值结构
	uint8 address;	//模块地址号
	uint8 func;		//功能码
	uint8 dataLen;	//数据长度
	uint16 passwd;	//密码
	uint8  addr;	//地址
	uint8  baud;	//波特率, 00-38400, 01-19200, 02-9600, 03-4800
	uint16 ctlWord;	//控制字
	uint16 PTRatio;	//PT变比
	uint16 CTRatio;	//CT变比
	uint8  oneOutSelect;	//第1路变送输出选择
	uint16 oneOutScale;		//第1路输出满度
	uint8  twoOutSelect;	//第2路变送输出选择
	uint16 twoOutScale;		//第2路输出满度
	uint8  threeOutSelect;	//第3路变送输出选择
	uint16 threeOutScale;	//第3路输出满度
	uint8  fourOutSelect;	//第4路变送输出选择
	uint16 fourOutScale;	//第4路输出满度
	uint16 unused1[6];		//保留
	uint8  lightCtl;		//背光控制
	uint8  unused3;			//保留
	uint16 unused2[16];		//保留
	uint16 inputState;		//开关输入量状态
	uint8  DPT;				//小数点U
	uint8  DCT;				//小数点I
	uint8  DPQ;				//小数点PQ
	uint8  PQ;				//符号PQ
	uint16  voltageA;		//A相电压
	uint16  voltageB;		//B相电压
	uint16  voltageC;		//C相电压
	uint16  voltageAB;		//A-B线电压
	uint16  voltageCB;		//C-B线电压
	uint16  voltageAC;		//A-C线电压
	uint16  currentA;		//A相电流
	uint16  currentB;		//B相电流
	uint16  currentC;		//C相电流
	uint16  powerA;			//A相有功率
	uint16  powerB;			//B相有功率
	uint16  powerC;			//C相有功率
	uint16  powertotal;		//总有功率
	uint16  powerQA;		//A相无功率
	uint16  powerQB;		//B相无功率
	uint16  powerQC;		//C相无功率
	uint16  powerQtotal;	//总无功率
	uint16  pfA;			//A相视在因数
	uint16  pfB;			//B相视在因数
	uint16  pfC;			//C相视在因数
	uint16  pftotal;		//总视在因数
	uint16  sA;				//A相视在功率
	uint16  sB;				//B相视在功率
	uint16  sC;				//C相视在功率
	uint16  stotal;			//总视在功率
	uint16  frequency;		//频率
	uint32  absorbActive2;	//吸收有功电能二次侧
	uint32  freeActive2;	//释放有功电能二次侧
	uint32  induceReactive2;//感性无功电能二次侧
	uint32	capReactive2;	//容性无功电能二次侧
	float32 absorbActive1;	//吸收有功电能一次侧
	float32 freeActive1;	//释放有功电能一次侧
	float32 induceReactive1;//感性无功电能一次侧
	float32 capReactive1;	//容性无功电能一次侧
	uint16  crcsum;
} acrelPZ80_E4CDataStr;
typedef acrelPZ80_E4CDataStr* acrelPZ80_E4CDataPtr;

typedef struct {//电表返回值格式
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
