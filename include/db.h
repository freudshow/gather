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
#define TABLE_ELEC			"t_elec_data"//电表
#define TABLE_WATER			"t_water_data"//水表
#define TABLE_HEAT			"t_heat_data"//热量表
#define TABLE_AIR			"t_air_data"//天然气表


/********************************************************************************
 ** 各字符型字段的长度
 ********************************************************************************/
#define LENGTH_F_CONFIG_NAME		16//配置名长度
#define LENGTH_F_CONFIG_VALUE		50//配置值长度
#define LENGTH_F_METER_ADDRESS	50//表地址长度
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
typedef struct {
	char f_install_pos[LENGTH_F_INSTALL_POS];
	char f_meter_address[LENGTH_F_METER_ADDRESS];
	int f_id;
	int f_meter_type;
	int f_meter_channel;
	int f_device_id;
	int f_meter_proto_type;
}meter_address_str;
typedef meter_address_str *pMeter_address;
/********************************************************************************
 **	 t_request_data
 ** 仪表需要返回的数据项配置表
 ********************************************************************************/
 typedef struct {
	char f_col_name[LENGTH_F_COL_NAME];
	char f_col_type[LENGTH_F_COL_TYPE];
	int f_id;
	int f_meter_type;
	int f_item_index;
}request_data_str;
typedef request_data_str *pRequest_data;
/********************************************************************************
 **	 t_time_node
 ** 仪表需要返回的数据项配置表
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
typedef struct {
	char f_timestamp[LENGTH_F_TIMESTAMP];//时间戳
	char f_time[LENGTH_F_TIME];//抄表时间点
	char f_meter_address[LENGTH_F_METER_ADDRESS];//仪表地址
	int  f_device_id;//仪表的设备编号
	int  f_id;//索引值
	void *pValues;//数据项列表
	int value_len;//数据项长度
}his_data_str;
typedef his_data_str *pHis_data;


/********************************************************************************
 ** 全局声明
 ********************************************************************************/
extern sys_config_str sys_config_array[SYS_CONFIG_COUNT];

extern void get_select_sql(char *table_name, char **cols, int col_count, char *sql);
extern void get_where_sql(char **condition, int con_count, char *sql);
extern void get_orderby_sql(char **fields, int f_cnt, char *sql);
extern void get_query_sql(char *table_name, char **cols, int col_count, char **condition, int con_count, char *sql);
extern void get_insert_sql(char *table_name, char **cols, int col_count, char **values,char *sql);
extern void get_update_sql(char *table_name, char **sets, int set_count, char **condition, int con_count, char *sql);
extern void get_delete_sql(char *table_name, char **condition, int con_count, char *sql);
extern void read_sys_config(char *pErr);
#endif  //_DB_H_
