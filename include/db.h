/*
  *******************************************************************************
  * @file    db.h
  * @author  songbaoshan
  * @version V0.0.0
  * @date    03-03-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */


/********************************************************************************
 **			数据库读取时，callback 回调函数参数定义
 **	callback(void *NotUsed, int argc, char **argv, char **azColName)	
 **  int argc --- 共有多少个字段
 **	char **argv --- 指向各个字段指针数组的指针，*argv 则为某字段数据开始地址
 **	char **azColName --- 指向各个字段名称指针数组的指针，
 **        *azColName 则为某字段名称的开始地址
 **
 ********************************************************************************/

#ifndef _DB_H_
#define _DB_H_

#include "globaldefine.h"
/********************************************************************************
 ** SQL动词
 ********************************************************************************/
#define SQL_SELECT			"select"
#define SQL_DELETE			"delete"
#define SQL_INSERT			"insert into"
#define SQL_DROP				"drop table if exists"
#define SQL_CREATE_TABLE	"create table"
#define SQL_UPDATE			"update"
#define SQL_SET				"set"

/********************************************************************************
 ** SQL副词与介词
 ********************************************************************************/
#define SQL_FROM		"from"
#define SQL_WHERE	"where"
#define SQL_AND		"and"
#define SQL_VALUES	"values"
#define SQL_ORDER	"order by"
/********************************************************************************
 ** SQL符号
 ********************************************************************************/
#define SQL_STAR					"*"
#define SQL_LEFT_PARENTHESIS	"("
#define SQL_RIGHT_PARENTHESIS	")"
#define SQL_SINGLE_QUOTES		"'"
#define SQL_EQUAL				"="

/********************************************************************************
 ** SQL数据表名称
 ********************************************************************************/
#define TABLE_BASE_DEF		"t_base_define"//基本配置表
#define TABLE_REQUEST_DATA	"t_request_data"//各类型仪表需要的数据项表
#define TABLE_ELEC			"t_elec_data"//电表历史数据
#define TABLE_WATER			"t_water_data"//水表历史数据
#define TABLE_HEAT			"t_heat_data"//热量表历史数据
#define TABLE_AIR			"t_air_data"//天然气表历史数据
#define TABLE_METER_INFO	"t_meter_info"//仪表基本信息表
#define TABLE_TIME_NODE		"t_time_node"//时间配置表

/********************************************************************************
 ** 字符长度相关
 ********************************************************************************/
#define LEN_BYTE	8//一个字节的比特数
#define LEN_HALF_BYTE	4//一半字节的比特数
#define BYTE_BCD_CNT	2//一个字节由多少个BCD码表示

/********************************************************************************
 ** 字符相关
 ********************************************************************************/
#define ZERO_CHAR	0x30//char '0'

/********************************************************************************
 ** 各字符型字段的长度 F 代表Field
 ********************************************************************************/
#define LENGTH_F_CONFIG_NAME		16//配置名长度
#define LENGTH_F_CONFIG_VALUE		50//配置值长度
#define LENGTH_F_METER_ADDRESS	14//在数据表中存储的仪表地址长度, 最大14个字符
#define LENGTH_B_METER_ADDRESS	7//在程序中处理过的仪表地址长度, 最大7字节, B代表Byte
#define LENGTH_F_METER_TYPE		2//在数据表中存储的仪表类型长度, 最大2个字符

#define LENGTH_F_INSTALL_POS		50//仪表安装位置长度
#define LENGTH_F_TIMESTAMP			50//时间戳长度
#define LENGTH_F_TIME				50//抄表时间点长度
#define LENGTH_F_COL_NAME			20//抄表项列名长度
#define LENGTH_F_COL_TYPE			20//抄表项类型名长度
#define LENGTH_F_TIME_NAME			20//时间点名称长度
#define LENGTH_F_TIME_NODE			100//时间点配置长度

#define LENGTH_SQLBUF		4096//sql语句的最大长度; 有的语句很长, 所以长度定的长些
#define LENGTH_SQLCONS		50//一个条件语句的最大长度, 如"col_number_id=23"
#define LENGTH_SQLCON		1024//一条条件从句的最大长度, 如"col_number_id=23 and col_number2_id=24"
#define LENGTH_SQLINSERT	2048//一条insert语句(不计where从句)的最大长度
#define LENGTH_SQLVALUE		50//一个数据域值的最大长度
#define LENGTH_SQLSET		200//一条set从句的最大长度
#define LENGTH_SQLORDER		200//一条order by从句的最大长度


/********************************************************************************
 ** t_base_define
 ** 基础配置信息表, 用t_base_define.f_id来索引对应的配置
 ********************************************************************************/
#define SYS_CONFIG_COUNT	14//基本配置项的数量

#define FIELD_BASE_DEF_ID		"f_id"
#define FIELD_BASE_DEF_NAME	"f_config_name"
#define FIELD_BASE_DEF_VALUE	"f_config_value"


enum T_System_Config {
	CONFIG_PRIMARY_SERVER = 0,
	CONFIG_PRIMARY_DNS,
	CONFIG_PRIMARY_PORT,
	CONFIG_SECOND_SERVER,
	CONFIG_SECOND_DNS,
	CONFIG_SECOND_PORT,
	CONFIG_GATEWAY_ID,
	CONFIG_NET_TYPE,
	CONFIG_MD5_KEY,
	CONFIG_AES_KEY,
	CONFIG_COLLECT_MODE,
	CONFIG_COLLECT_CYCLE,
	CONFIG_REPORT_MODE,
	CONFIG_BEAT_CYCLE

};

typedef struct {
	char f_config_name[LENGTH_F_CONFIG_NAME];
	char f_config_value[LENGTH_F_CONFIG_VALUE];
	int f_id;
} sys_config_str;
typedef sys_config_str *pSys_config;
/********************************************************************************
 **	 t_meter_info
 ** 设备地址信息表
 ********************************************************************************/
#define FIELD_MINFO_ID				"f_id"
#define FIELD_MINFO_ADDRESS		"f_meter_address"
#define FIELD_MINFO_TYPE			"f_meter_type"
#define FIELD_MINFO_CHANNEL		"f_meter_channel"
#define FIELD_MINFO_POS				"f_install_pos"
#define FIELD_MINFO_DEVICE_ID		"f_device_id"
#define FIELD_MINFO_PROTO_TYPE	"f_meter_proto_type"

struct meter_info_str{
	uint32 f_id;//行索引, 声明放到前面, 以免值被uint8[]类型的溢出改写
	uint8 f_meter_address[LENGTH_B_METER_ADDRESS];//表地址7字节
	uint8 f_meter_type;//1字节
	uint8 f_meter_channel;//1字节
	uint16 f_device_id;//2字节
	uint8 f_meter_proto_type;//1字节

	//安装位置, 50字节; 应该放到值声明的最后, 
	//以免因为"安装位置"的长度超过LENGTH_F_INSTALL_POS, 
	//对后面的成员变量进行改写.
	//通常指针成员的操作都是在值成员的操作完成后进行,
	//所以"安装位置"溢出后一般不会引起指针类型成员的值变化
	char f_install_pos[LENGTH_F_INSTALL_POS];
	
	struct meter_info_str* pNext;//下个元素
	struct meter_info_str* pPrev;//上个元素
};
typedef struct meter_info_str *pMeter_info;
typedef pMeter_info meter_info_List;

/********************************************************************************
 **	 t_request_data
 ** 仪表需要返回的数据项配置表
 ********************************************************************************/
#define MTYPE_WATER	0x10//水表仪表编号
#define MTYPE_HEAT	0x20//热量表仪表编号
#define MTYPE_ELECT	0x40//电表仪表编号

#define FIELD_REQUEST_ID			"f_id"
#define FIELD_REQUEST_MTYPE		"f_meter_type"
#define FIELD_REQUEST_ITEMIDX		"f_item_index"
#define FIELD_REQUEST_COLNAME		"f_col_name"
#define FIELD_REQUEST_COLTYPE		"f_col_type"


 
struct request_data_str{
	int32 f_id;
	uint8 f_meter_type;
	uint8 f_item_index;
	char f_col_name[LENGTH_F_COL_NAME];
	char f_col_type[LENGTH_F_COL_TYPE];
	struct request_data_str *pNext;
};
typedef struct request_data_str *pRequest_data;
typedef pRequest_data request_data_list;
/********************************************************************************
 **	 t_time_node
 ** 时间点配置表
 ********************************************************************************/
 typedef struct {
	char f_time_name[LENGTH_F_TIME_NAME];
	char f_time_node[LENGTH_F_TIME_NODE];
	int f_id;
}time_node_str;
typedef time_node_str *pTime_node;
/********************************************************************************
 ** 仪表历史数据项
 ********************************************************************************/
enum T_His_Field{
	STRING = 0,
	INT,
	FLOAT
};

struct his_field_data{
	char *pData;
	char field_name[LENGTH_F_COL_NAME];
	enum T_His_Field eData_type;
	struct his_field_data *pNext;
};
typedef struct his_field_data *pHis_field_data;
typedef pHis_field_data his_data_list;

struct his_data_str{
	char f_timestamp[LENGTH_F_TIMESTAMP];//时间戳
	char f_time[LENGTH_F_TIME];//抄表时间点
	char f_meter_address[LENGTH_F_METER_ADDRESS];//仪表地址
	int  f_device_id;//仪表的设备编号
	int  f_id;//索引值
	uint8  f_meter_type;//仪表类型
	his_data_list value_list;//数据项链表
	int value_cnt;//数据项的数量
	struct his_data_str* pNext;//下个元素
	struct his_data_str* pPrev;//上个元素
};
typedef struct his_data_str *pHis_data;
typedef pHis_data his_data_List;


/********************************************************************************
 *******************          全局声明       ************************************
 ********************************************************************************/
 
/***************************************
 ** 数据库打开与关闭, sql语句组合相关 **
 ***************************************/
int open_db(void);
int close_db(void);
void get_select_sql(char *table_name, char **cols, int col_count, char *sql);
void get_where_sql(char **condition, int con_count, char *sql);
void get_orderby_sql(char **fields, int f_cnt, int asc, char *sql);
void get_query_sql(char *table_name, char **cols, int col_count, char **condition, int con_count, char *sql);
void get_insert_sql(char *table_name, char **cols, int col_count, char **values,char *sql);
void get_update_sql(char *table_name, char **sets, int set_count, char **condition, int con_count, char *sql);
void get_delete_sql(char *table_name, char **condition, int con_count, char *sql);
/**********************
 ** 系统配置参数相关 **
 **********************/
void read_sys_config(char *pErr);//从数据库读取基本配置
sys_config_str get_sys_config(enum T_System_Config);//读取单独一条配置
int get_sys_config_cnt();//读取系统配置参数的个数

/**********************
 ** 仪表地址信息相关 **
 **********************/
void read_meter_info(char	*pErr);//从数据库读取仪表地址信息
void retrieve_meter_info_list(int (*read_one_meter)(pMeter_info));//顺序遍历仪表地址信息
int  get_meter_info_cnt();//读取仪表地址信息的个数

/**********************
 ** 读取数据项相关 **
 **********************/
void read_request_data(char	*pErr, uint8);//按照仪表类型读取数据项
void retrieve_request_data_list(int (*read_one_meter)(pMeter_info));//顺序遍历数据项信息
int  get_request_data_cnt();//读取仪表数据项的个数

#endif  //_DB_H_
