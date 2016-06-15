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
#define HEATMETER	0x20//热表
#define WATERMETER	0x10//水表
#define ELECTMETER	0x40//电表
#define GASMETER	0x30//燃气表
#define SENSORDEV  0x50//采集各种传感器的Rtu模块

#define METER_TYPE_CNT 5//一共有多少种表

//字段长度定义
#define LENGTH_B_METER_ADDRESS	7//在程序中处理过的仪表地址长度, 最大7字节, B代表Byte
#define LENGTH_F_INSTALL_POS		50//仪表安装位置长度
#define LENGTH_SYS_ANSWER       16384//系统命令返回的信息长度

#define LENGTH_F_CONFIG_NAME		16//配置名长度
#define LENGTH_F_CONFIG_VALUE		50//配置值长度
#define LENGTH_F_METER_ADDRESS	    15//在数据表中存储的仪表地址长度, 最大14个字符, 加上结束符'\0', 就是15个字符
#define LENGTH_F_METER_TYPE		2//在数据表中存储的仪表类型长度, 最大2个字符

/********************************************************************************
 ** 字符长度相关
 ********************************************************************************/
#define LEN_BYTE	8//一个字节的比特数
#define LEN_HALF_BYTE	4//一半字节的比特数
#define BYTE_BCD_CNT	2//一个字节由多少个BCD码表示

#define LENGTH_MD5              50//MD5值的最大长度, 一般为32, 这里定的大一些


/*上位机查询下位机缺少的帧索引列表长度. 假设上位机下发程序的总帧数为1024帧,
 * 因为1~9, 9个1字符; 10~99, 90个2字符; 100~999, 900个3字符; 1000~1024, 25个4字符
 * 加起来共2989个字符, 再加上每个索引后面的一个分隔符(','或者' '), 共1023个
 * 这样总字符长度就是4012, 凑个整数, 4096.
 * 如果升级程序的总帧数大于1024, 那么就用总帧数除以1024, 求出其整数部分, 再加1,
 * 用这个数乘以4096即可, 就怕上位机不做这么大的暂存区来缓存不完整的xml帧
 */
#define LENGHTH_LACK_STRING     4096
/********************************************************************************
 ** MODBUS相关
 ********************************************************************************/
#define MODBUS_READ    0x03//读多路寄存器
#define LC_ELEC_WORK_START  0x000C//力创电表电量参数的起始寄存器地址
#define LC_ELEC_WORK_LEN    0x000C//力创电表电量参数的寄存器个数



//定义设备文件名称
#define DEVICE4851		"/dev/ttyS1"
#define DEVICE4852   	"/dev/ttyS2"
#define DEVICEGPRS  	"/dev/ttyS3"
#define DEVICEMBUS  	"/dev/ttyS6"           

#define IO_CONTROL  "/dev/CONTROL_IO"

#define QMSG_FILE	"./"  //消息队列用


//数据库名称
#define SQLITE_NAME  "gatherdb.db"
#define APP_NAME    "gather_V0"
#define APP_TMPNAME "tmp_gather"

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

#define READ_METER_TIMES 2//重复读表次数






//COM口参数使用结构体
typedef struct
{
    INT32 ByteBits;
    INT32 Speed;
    UINT8 Name[15];
}COM_INFO_T;

//消息队列用消息体
typedef struct{
	long int mtype;
	uint8 dev;
	uint8 functype;
    char timenode[30];
}QmsgType;



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

typedef enum{//写入历史数据协议的当前状态
    stat_his_init=0,//初始状态, 文件空白
    stat_his_trans,//传输相关数据状态
    stat_his_data,//row data状态
    stat_his_end//结束状态
}wr_his_stat;

/*****************************************
 ** 仪表种类
 *****************************************/
#define MTYPE_CNT	4//挂载的仪表类型数量
enum meter_type_idx;
typedef enum meter_type_idx mtype_idx;
enum meter_type_idx{
	em_heat=0,//热表索引
	em_water,//水表索引
	em_elect,//电表索引
	em_gas//燃气表索引
};


#define LENGTH_ADDR	50//集中器号或者上位机ip的地址长度
#pragma pack(1)
typedef struct{
    uint8 sadd[LENGTH_ADDR];
    uint8 oadd[LENGTH_ADDR];
    uint8 func_type;
    uint8 oper_type;
    char timenode[30];
    xmlDocPtr xmldoc_rd;//从上位机下发的xmlDoc
    xmlDocPtr xmldoc_wr;//需要向上位机发送的xmlDoc
    uint8 xml_rd_file_idx;//上位机下发的xml文件索引
    uint8 xml_wr_file_idx;//要写入历史数据的xml文件索引

    

    int total_row[MTYPE_CNT];//当前表类型共有多少行数据
    int mod[MTYPE_CNT];//总共的行数 mod ROW_PER_FRAME的余数, 用于计算总共的帧数和最后一帧的行数, 仪表类型相关的
    int total_frame[MTYPE_CNT];//总共需要发多少帧, 仪表类型相关的
    int cur_frame[MTYPE_CNT];//当前第几帧, 从1开始, 仪表类型相关的

    int cur_cnt;//当前帧有几行数据, 与表类型无关的
    int total_rows;//总共的行数, 与表类型无关的
    int cur_frame_indep;//与表类型无关的帧索引
    int cur_rows;//已经发送了多少行数据 if(read answer OK) cur_rows += cur_cnt;
    int cur_row_idx;//每一帧发送的行的索引
    wr_his_stat cur_wr_state;//当前xml所处的状态

    int meter_info_row_idx;

    int up_total_bytes;//压缩包的总字节数
    int up_total_frm;//压缩包的总帧数
    int up_cur_bytes;//当前帧包含的字节数
    int up_cur_frm_idx;//远程升级时的当前帧索引
    char up_md5[LENGTH_F_CONFIG_VALUE];//上位机下发的新程序的MD5值
    uint8 **pDataList;//暂存空间的指针
    int *pDataLen;//每一帧暂存空间的大小
} xml_info_str;
typedef xml_info_str* pXml_info;
#pragma pack()




typedef enum{//上,下位机直接下发function的类型编号
    em_FUNC_ID = 0,     //func_index -0-, 登录
    em_FUNC_HEATBT,     //func_index -1-, 心跳
    em_FUNC_SYSCONF,    //func_index -2-, 系统参数配置
    em_FUNC_RQDATA,     //func_index -3-, 仪表的数据项配置
    em_FUNC_TNODE,      //func_index -4-, 抄表与上报时间点配置
    em_FUNC_MINFO,      //func_index -5-, 表地址配置
    em_FUNC_RPTUP,      //func_index -6-, 上传历史数据
    em_FUNC_RDSTAT,     //func_index -7-, 读取集中器状态
    em_FUNC_SWIP,       //func_index -8-, 切换ip
    em_FUNC_DBMANI,     //func_index -9-, 数据库透传
    em_FUNC_SYSCMD,     //func_index -10-, 本地shell命令透传
    em_FUNC_CODEUP,     //func_index -11-, 程序更新
    em_FUNC_PROTOTRS,   //func_index -12-, 协议透传
    em_FUNC_CLOCK_SET,  //func_index -13-, 集中器校时
    em_FUNC_COLLECT_SET //func_index -14-, 集中器抄表模式和抄表周期设置。
}func_type_idx;



typedef enum{//上下位机操作类型编号
	em_OPER_RD=0,//读取
	em_OPER_WR,//写入
	em_OPER_DO,//操作
	em_OPER_ASW//应答
}oper_type_idx;

#define RPT_ACTIVE  0
#define RPT_PASSIVE 1

#define NETMOD_GPRS 0
#define NETMOD_RS485 0
#define NETMOD_ETH 0
#define NETMOD_GPRS 0

typedef enum em_net_mod{//组网方式
    em_net_gprs   = 0,  //GPRS
    em_net_rs485,       //RS485
    em_net_eth,         //以太网
    em_net_zigbee       //ZigBee
}net_mod;

typedef enum T_System_Config {
    CONFIG_PRIMARY_SERVER = 0,  //-0-, 主服务器IP
    CONFIG_PRIMARY_DNS,         //-1-, 主DNS IP
    CONFIG_PRIMARY_PORT,        //-2-, 主服务器端口
    CONFIG_SECOND_SERVER,       //-3-, 副服务器IP
    CONFIG_SECOND_DNS,          //-4-, 副DNS IP
    CONFIG_SECOND_PORT,         //-5-, 副服务器端口
    CONFIG_GATEWAY_ID,          //-6-, 集中器编号
    CONFIG_NET_TYPE,            //-7-, 网络类型，0-GPRS,1-485等。
    CONFIG_MD5_KEY,             //-8-, MD5秘钥
    CONFIG_AES_KEY,             //-9-, AES秘钥
    CONFIG_COLLECT_MODE,        //-10-, 自动抄表方式，0-根据collect_cycle周期设置抄表，1-定点抄表。
    CONFIG_COLLECT_CYCLE,       //-11-, 自动采集周期，当COLLECT_MODE为0时有效。
    CONFIG_REPORT_MODE,         //-12-, 数据上报模式,0-主动上报，1-被动请求。
    CONFIG_BEAT_CYCLE,          //-13-, 心跳周期，范围1-10分钟。
    CONFIG_SVR_NUM,             //-14-, 主服务器的编号
    CONFIG_APP_MD5              //-15-, 程序文件的md5值
}sys_config_idx;
#define SYS_CONFIG_COUNT	16//基本配置项的数量

typedef enum item_idx_heat {//按照CJ188协议规定的字段域
    em_HColdE=1,        //冷量, 实际用于结算日热量
    em_HHeatE,          //累积热量
    em_HPower,          //热功率
    em_HFlowrate,       //流速
    em_HAccumFlow,      //累积流量
    em_HInTemp,         //供水温度
    em_HOutTemp,        //回水温度
    em_HAccumWorkTime,  //累积工作时间
    em_HCurTime,        //实时时间
    em_HState           //状态ST
}hItem_idx;

typedef enum item_idx_water {//水表数据项
    em_WCurAccumFlow=1, //当前累积流量
    em_WFlowrate,       //流速
    em_WSettleAccumFlow,//结算日累积流量
    em_WSettleSate,     //结算日
    em_WAccumWorkTime   //累积工作时间
}wItem_idx;

typedef enum item_idx_elec {//电表数据项
    em_EPActTotElec=1,	    //正向有功总电能
    em_ENActTotElec,	    //反向有功总电能
    em_EPReactTotElec,	    //正向无功总电能
    em_ENReactTotElec,	    //反向无功总电能
    em_EActTotElec,	    //有功总电能
    em_EReactTotElec,	    //无功总电能
    em_EOvrPowFac,	    //总功率因数
    em_EPMaxDem	        //正向有功总最大需量
}eItem_idx;

#define PROTO_RES_LEN 512
typedef struct {//透传命令相关信息
    uint8 channel;      //通道号
    uint32 baud;        //波特率
    uint8 databits;     //数据位
    uint32 stop;        //停止位
    uint8 parity;      //校验位
    uint8 cmd[256];     //透传命令
    uint16 cmdlen;       //透传命令的长度
    uint8 meter_type;    //仪表类型
    uint8 proto_type;    //协议类型
    uint8 res[PROTO_RES_LEN];     //透传的结果
    uint16 resLen;
} proto_trans_str;
typedef proto_trans_str* pProto_trans;


#endif  //_GLOBALDEFINE_H_
