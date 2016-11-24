#ifndef ELECT_H
#define ELECT_H


#define ELEC_PROTO_MODBUS	0//MODBUS协议电表
#define ELEC_PROTO_645		1//645协议电表

#define ELEC_LCMODBUS		0//力创Modbus电表
#define ELEC_ACREL_DDSD1352	1//acrel DDSD1352单相电表
#define ELEC_ACREL_DTSF1352	2//acrel DTSF1352三相电子式电能表
#define ELEC_ACREL_PZ80-E4C	3//acrel PZ80-E4C可编程智能电测表（三相功率、三相电能部分）

#define ELEC_LCMOD_RANGEV_REG  0x0001//力创Modbus电表, 电压量程的寄存器地址. 1～1000V 对应 1～1000
#define ELEC_LCMOD_RANGEI_REG  0x0002//力创Modbus电表, 电流量程的寄存器地址. 0.1～1000 数值 1～10000 默认5A值为50
#define ELEC_LCMOD_REG_BYTES       2//力创DTSD106三相电表, 每个寄存器的字节数
#define ELEC_LCMOD_WORK_BYTES      4//力创DTSD106三相电表, 每个电能参数对应的字节数
#define ELEC_LCMOD_ALL_PARAM_REGS  0x0017//力创Modbus电表, 从电压量程寄存器到无功总电能寄存器的寄存器个数

#define ELEC_LCMOD_DEF_V    250//力创Modbus电表, 默认电压量程
#define ELEC_LCMOD_DEF_I    5//力创Modbus电表, 默认电流量程

#pragma pack(1)

typedef struct lcModSendCmd{//力创modbus电表读取命令帧
	uint8 address;//模块地址号
	uint8 func;//功能码
	uint16 start;//起始寄存器地址
	uint16 regCount;//寄存器数量
}lcModSendStr;

typedef struct{//力创电表返回值格式
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
}lcModbusElec_str;
#pragma pack()

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
extern uint8 Read_ElecMeter(MeterFileType *pmf, lcModbusElec_str* pElecData);

#endif //ELECT_H
