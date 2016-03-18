/*
  *******************************************************************************
  * @file    globaldefine.h
  * @author  zjjin
  * @version V0.0.0
  * @date    02-24-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/ 

#ifndef _GLOBALDEFINE_H_
#define _GLOBALDEFINE_H_


//变量类型别名
#define INT8     char
#define UINT8    unsigned char
#define INT16    short
#define UINT16   unsigned short
#define INT32    int
#define UINT32   unsigned int
typedef  char int8;
typedef  short int16;
typedef  int  int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


//设备类型定义
#define HEATMETER	0x20
#define WATERMETER	0x10
#define ELECTMETER	0x40
#define GASMETER	0x30

//热表需要的数据项索引
#define	HITEM_CUR_COLD_E			0x01//当前冷量
#define	HITEM_CUR_HEAT_E			0x02//当前热量
#define	HITEM_HEAT_POWER			0x03//热功率
#define	HITEM_FLOWRATE			0x04//流速
#define	HITEM_ACCUM_FLOW			0x05//累积流量
#define	HITEM_IN_TEMP				0x06//供水温度
#define	HITEM_OUT_TEMP			0x07//回水温度
#define	HITEM_ACCUM_WORK_TIME	0x08//累计工作时间
#define	HITEM_REAL_TIME			0x09//实时时间
#define	HITEM_STATE				0x0a//状态ST
//水表需要的数据项索引
#define	WITEM_CUR_ACCUM_FLOW			0x01//当前累计流量
#define	WITEM_FLOWRATE					0x02//流速
#define	WITEM_SETTLE_DATE_ACCUM_FLOW	0x03//结算日累积流量
#define	WITEM_SETTLE_DATE				0x04//结算日
#define	WITEM_ACCUM_WORK_TIME			0x05//累计工作时间
//电表需要的数据项索引
#define	EITEM_ACTIVE_TOTAL_ELECTRICITY	0x01//有功总电能
#define	EITEM_CUR_VOLTAGE					0x02//当前电压值
#define	EITEM_CUR_CURRENT					0x03//当前电流值
#define	EITEM_CUR_ACTIVE_POWER				0x04//当前有功功率值
#define	EITEM_CUR_FREQ						0x05//当前频率值
#define	EITEM_CUR_REACTIVE_POWER			0x06//当前无功功率值
#define	EITEM_CUR_ APPARENT_POWER			0x07//当前视在功率值
#define	EITEM_CUR_POWER_FACTOR				0x08//当前功率因数值

#define LENGTH_B_METER_ADDRESS	7//在程序中处理过的仪表地址长度, 最大7字节, B代表Byte
#define LENGTH_F_INSTALL_POS		50//仪表安装位置长度



#define LENGTH_B_METER_ADDRESS	7//在程序中处理过的仪表地址长度, 最大7字节, B代表Byte
#define LENGTH_F_INSTALL_POS		50//仪表安装位置长度



//定义设备文件名称
#define DEVICE4851		"/dev/ttyS1"
#define DEVICE4852   	"/dev/ttyS2"
#define DEVICEGPRS  	"/dev/ttyS3"
#define DEVICEMBUS  	"/dev/ttyS6"           

#define IO_CONTROL  "/dev/CONTROL_IO"


//数据库名称
#define SQLITE_NAME  "gatherdb.db"

//IO口控制相关
#define IOCTLTEST_MAGIC    0xA4

#define LED0_A30   _IO (IOCTLTEST_MAGIC, 0)
#define LED1_A31   _IO (IOCTLTEST_MAGIC, 1)
#define MBUS_C0    _IO (IOCTLTEST_MAGIC, 2)
#define MBUS_C1    _IO (IOCTLTEST_MAGIC, 3)
#define MBUS_C2    _IO (IOCTLTEST_MAGIC, 4)
#define MBUS_C3    _IO (IOCTLTEST_MAGIC, 5)
#define MBUS_C4    _IO (IOCTLTEST_MAGIC, 6)
#define MBUS_C5    _IO (IOCTLTEST_MAGIC, 7)

#define GPRS_SWITCH_C28   _IO (IOCTLTEST_MAGIC, 8)
#define GPRS_RESET_C29    _IO (IOCTLTEST_MAGIC, 9)

#define RS4851_C30    _IO (IOCTLTEST_MAGIC, 10)
#define RS4852_C31    _IO (IOCTLTEST_MAGIC, 11)

#define IOCTLTEST_MAXNR        12


//通讯速率宏定义
#define RS485UP_COM_SPEED	19200 
#define GPRS_COM_SPEED		115200


//RS485 发送和接收
#define RS485_RECV  0
#define RS485_SED   1



#define FALSE 0xff 
#define TRUE  0x00 
#define OS_TICKS_PER_SEC     1000u   //Set the number of ticks in one second                        */
#define OS_ERR_NONE          0u


//错误信息的定义
#define CONTROL_RET_FAIL 	-1
#define CONTROL_RET_SUC  	0
#define NO_ERR		0//成功
#define ERR_1			1//文档错误
#define ERR_FF		0xff//致命错误

//超时时间宏定义
#define REC_TIMEOUT_SEC  	2 	//超时时间 秒
#define REC_TIMEOUT_USEC  	0	//超时时间 毫秒

#define RS485UP_REC_TIMEOUT_SEC  	REC_TIMEOUT_SEC 	
#define RS485UP_REC_TIMEOUT_USEC  	REC_TIMEOUT_USEC	



#define METER_FRAME_LEN_MAX	256		//数据帧最大长度, 该数据必须保证为2的N次方

#define RS485_DOWN_CHANNEL	7		//宏定义下行485通道号。








//COM口参数使用结构体
typedef struct
{
    INT32 ByteBits;
    INT32 Speed;
    UINT8 Name[15];
}COM_INFO_T;


//定义表基础信息结构体。
typedef struct
{
	uint16 u16MeterID; //Hex
	uint8 u8MeterAddr[LENGTH_B_METER_ADDRESS];  //BCD码
	uint8 u8MeterType;	   //BCD
	uint8 u8ProtocolType;  //HEX
	uint8 u8Channel;	   //HEX
	char  install_pos[LENGTH_F_INSTALL_POS];  //字符串
}MeterFileType;

#define LENGTH_ADDR	50//集中器号或者上位机ip的地址长度
typedef struct{
	uint8 sadd[LENGTH_ADDR];
	uint8 oadd[LENGTH_ADDR];
	uint8 func_type;
	uint8 oper_type;
	xmlDocPtr xmldoc;
	uint8 appendix1; 	//附加内容1
	uint8 appendix2;  	//附加内容2
	uint8 appendix3;  	//附加内容3
	uint8 appendix4;  	//附加内容4
} xml_info_str;
typedef xml_info_str* pXml_info;






#endif  //_GLOBALDEFINE_H_

