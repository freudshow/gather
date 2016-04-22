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
#include "readallmeters.h"
#include "read_heatmeter.h"

/*添加链表节点的通用模式, 前提是链表结构体内的元素指针必须命名为pNext和pPrev
 *首先, 被添加节点的后继指针肯定要指向当前的链表头.
 *如果当前的链表头为空, 那么被添加的节点的前序指针就指向自己, 即自己就是表尾.
 *如果当前的链表头不空, 那么当前链表的前序指针必指向表尾, 就把这个表尾的地址
 *赋给被添加节点的前序指针, 然后再把被添加节点的地址赋给当前链表头结点的前序指针.
 *最后, 把当前链表头指向被添加节点的地址.
 *这样操作下来, 链表的头和尾可以很方便的找到
 */
#define add_node(pListHead, pNode)  pNode->pNext = pListHead;\
                    if (pListHead) {\
                        pNode->pPrev = pListHead->pPrev;/*链表尾部*/\
                        pListHead->pPrev = pNode;\
                    }\
                    else {\
                            pNode->pPrev = pNode;\
                    }\
                    pListHead = pNode;

#define empty_list(listType, list)   listType tmpNode;\
                                    while(list) {\
                                        tmpNode = list;\
                                        list = list->pNext;\
                                        free(tmpNode);\
                                    }
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
 ** SQL常用语句
 ********************************************************************************/
#define SQL_MTYPE_CNT "select distinct f_meter_type from t_request_data"//获取仪表种类数量的SQL语句
#define SQL_MTYPE	  "select count(distinct f_meter_type) from t_request_data"//获取仪表种类数量的SQL语句


/********************************************************************************
 ** SQL数据表名称
 ********************************************************************************/
#define TABLE_BASE_DEF		"t_base_define"//基本配置表
#define TABLE_REQUEST_DATA	"t_request_data"//各类型仪表需要的数据项表
#define TABLE_ELEC			"t_elect_data"//电表历史数据
#define TABLE_WATER			"t_water_data"//水表历史数据
#define TABLE_HEAT			"t_heat_data"//热量表历史数据
#define TABLE_GAS			"t_air_data"//天然气表历史数据
#define TABLE_METER_INFO	"t_meter_info"//仪表基本信息表
#define TABLE_TIME_NODE		"t_time_node"//时间配置表



/********************************************************************************
 ** 字符相关
 ********************************************************************************/
#define ZERO_CHAR	0x30//char '0'

/********************************************************************************
 ** 各字符型字段的长度 F 代表Field
 ********************************************************************************/
#define LENGTH_F_CONFIG_NAME		16//配置名长度
#define LENGTH_F_CONFIG_VALUE		50//配置值长度
#define LENGTH_F_METER_ADDRESS	    15//在数据表中存储的仪表地址长度, 最大14个字符, 加上结束符'\0', 就是15个字符
#define LENGTH_F_METER_TYPE		2//在数据表中存储的仪表类型长度, 最大2个字符

#define LENGTH_F_ID                 6//行索引号的最大长度
#define LENGTH_F_MTYPE              2//仪表类型编号的长度
#define LENGTH_F_DEVID              3//设备编号的最大长度
#define LENGTH_F_TIMESTAMP			50//时间戳长度
#define LENGTH_F_TIME				50//抄表时间点长度
#define LENGTH_F_COL_NAME			20//抄表项列名长度
#define LENGTH_F_VALUE				100//抄表历史数据最大长度
#define LENGTH_F_COL_TYPE			20//抄表项类型名长度
#define LENGTH_F_TIME_NAME			20//时间点名称长度
#define LENGTH_F_TIME_NODE			100//时间点配置长度

#define LENGTH_SQLBUF		4096//sql语句的最大长度; 有的语句很长, 所以长度定的长些
#define LENGTH_SQLCONS		50//一个条件语句的最大长度, 如"col_number_id=23"
#define LENGTH_SQLCON		1024//一条条件从句的最大长度, 如"col_number_id=23 and col_number2_id=24"
#define LENGTH_SQLINSERT	4096//一条insert语句(不计where从句)的最大长度
#define LENGTH_SQLVALUE		50//一个数据域值的最大长度
#define LENGTH_SQLSET		200//一条set从句的最大长度
#define LENGTH_SQLORDER		200//一条order by从句的最大长度

/********************************************************************************
 ** t_base_define
 ** 基础配置信息表, 用t_base_define.f_id来索引对应的配置
 ********************************************************************************/

#define FIELD_BASE_DEF_ID		"f_id"
#define FIELD_BASE_DEF_NAME	"f_config_name"
#define FIELD_BASE_DEF_VALUE	"f_config_value"

struct sys_config_structure;
typedef struct sys_config_structure sys_config_str;
typedef sys_config_str *pSys_config;
typedef pSys_config sys_config_list;

struct sys_config_structure{
	char f_config_name[LENGTH_F_CONFIG_NAME];
	char f_config_value[LENGTH_F_CONFIG_VALUE];
	int f_id;
	pSys_config pNext;
	pSys_config pPrev;
};


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

struct meter_info_str;
typedef struct meter_info_str *pMeter_info;
typedef pMeter_info meter_info_List;

struct meter_info_str{
	uint32 f_id;//行索引, 声明放到前面, 以免值被uint8[]类型的溢出改写HEX
	uint8 f_meter_address[LENGTH_B_METER_ADDRESS];//表地址7字节BCD
	uint8 f_meter_type;//1字节BCD
	uint8 f_meter_channel;//1字节HEX
	uint16 f_device_id;//2字节HEX
	uint8 f_meter_proto_type;//1字节HEX

	//安装位置, 50字节; 应该放到值声明的最后, 
	//以免因为"安装位置"的长度超过LENGTH_F_INSTALL_POS, 
	//对后面的成员变量进行改写.
	//通常指针成员的操作都是在值成员的操作完成后进行,
	//所以"安装位置"溢出后一般不会引起指针类型成员的值变化
	char f_install_pos[LENGTH_F_INSTALL_POS];
	
	pMeter_info pNext;//下个元素
	pMeter_info pPrev;//上个元素
};


/********************************************************************************
 **	 t_request_data
 ** 仪表需要返回的数据项配置表
 ********************************************************************************/
#define FIELD_REQUEST_ID			"f_id"
#define FIELD_REQUEST_MTYPE		"f_meter_type"
#define FIELD_REQUEST_ITEMIDX		"f_item_index"
#define FIELD_REQUEST_COLNAME		"f_col_name"
#define FIELD_REQUEST_COLTYPE		"f_col_type"

typedef enum Type_His_Data{//历史数据的存储数据类型
    STRING = 0,
    INT,
    FLOAT
}EM_DType;

struct request_data_str;
typedef struct request_data_str* pRequest_data;
typedef pRequest_data request_data_list;

struct request_data_str{
	int32 f_id;
	uint8 f_meter_type;
	uint8 f_item_index;
	char f_col_name[LENGTH_F_COL_NAME];
	char f_col_type[LENGTH_F_COL_TYPE];
	pRequest_data pNext;
	pRequest_data pPrev;
};

/********************************************************************************
 ** 仪表历史数据项
 ********************************************************************************/
#define FIELD_HIS_ID			"f_id"//行索引号
#define FIELD_HIS_ADDRESS		"f_meter_address"//表地址
#define FIELD_HIS_MTYPE		"f_meter_type"//表类型
#define FIELD_HIS_DEVID		"f_device_id"//设备编号
#define FIELD_HIS_TSTAMP		"f_timestamp"//时间戳
#define FIELD_HIS_TNODE		"f_time"//抄表时间点

struct meter_item;
typedef struct meter_item *pMeter_item;
typedef pMeter_item meter_item_list;
struct meter_item{
    uint8 item_index;//数据项索引
    char field_value[LENGTH_F_VALUE];//历史数据
    char field_name[LENGTH_F_COL_NAME];//历史数据对应的列名
    pMeter_item pNext;
    pMeter_item pPrev;
};

struct his_data_str;
typedef struct his_data_str *pHis_data;
typedef pHis_data his_data_list;

struct his_data_str{//历史数据一行数据的格式, 每个数据域的长度都要比约定长度至少大1, 用于存储strcpy添加的'\0'
    char   f_id[LENGTH_F_ID];//索引值
    char   f_device_id[5];//仪表的设备编号, 程序里有可能添加"NULL", 故设置为5个字符
    char   f_meter_type[5];//仪表类型
    char   f_meter_address[LENGTH_F_METER_ADDRESS];//仪表地址
    char   f_timestamp[LENGTH_F_TIMESTAMP];//时间戳
    char   f_time[LENGTH_F_TIME];//抄表时间点
    int  value_cnt;//数据项的数量
    //使用数组, 每次开辟固定大小(value_cnt, 可以从get_request_data_cnt()获取)的空间
    //避免了每次操作一行数据, 就重新生成一个链表, 比较费时
    pMeter_item value_list;//数据项数组, 需要动态开辟, 
    pHis_data   pNext;//下个元素
    pHis_data   pPrev;//上个元素
};

/********************************************************************************
 **	 t_time_node
 ** 时间点配置表
 ********************************************************************************/
 struct time_node_str;
 typedef struct time_node_str *pTime_node;
 typedef pTime_node time_node_list;
 struct time_node_str{
	char f_time_name[LENGTH_F_TIME_NAME];
	char f_time_node[LENGTH_F_TIME_NODE];
	int f_id;
    pTime_node pNext;
    pTime_node pPrev;
};



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
void get_insert_sql(char *table_name, char **cols, int col_count, char **values,char *sql, int add_quote);
void get_update_sql(char *table_name, char **sets, int set_count, char **condition, int con_count, char *sql);
void get_delete_sql(char *table_name, char **condition, int con_count, char *sql);
/**********************
 ** 系统配置参数相关 **
 **********************/
void read_sys_config(char *pErr);//从数据库读取基本配置
uint8 get_sys_config(sys_config_idx idx, pSys_config pConfig);//读取单独一条配置
int get_sys_config_cnt();//读取系统配置参数的个数
uint8 insert_sysconf(pSys_config pConf);//插入单个系统配置
uint8 empty_sysconf_list();//清空配置列表
uint8 set_sysconf(char* pErr);//设置系统参数
uint8 add_one_config(pSys_config pConf, char* pErr);
void get_sys_config_name(sys_config_idx idx, char* config_name);

/**********************
 ** 仪表地址信息相关 **
 **********************/
void read_meter_info(char	*pErr);//从数据库读取仪表地址信息
void retrieve_meter_info_list(int (*read_one_meter)(pMeter_info));//顺序遍历仪表地址信息
int  get_meter_info_cnt();//读取仪表地址信息的个数
void empty_meter_info_list();
uint8 empty_meter_info_table(char*);
uint8 insert_one_meter_info(pMeter_info);
uint8 insert_into_meter_info_table(char* pErr);

/**********************
 ** 读取数据项相关 **
 **********************/
void read_all_request_data(char	*pErr);
void read_request_data(char	*pErr, mtype_idx type_idx);//按照仪表类型读取数据项
void retrieve_request_data_list(int (*read_one_item)(pRequest_data, void*), mtype_idx type_idx, void* pVar);//顺序遍历数据项信息
int  get_request_data_cnt(mtype_idx);//读取仪表数据项的个数
uint8 insert_one_request_node(pRequest_data pRqData);
uint8 set_request_data(char* pErr);
int get_request_data_setted();

/**********************
 ** 仪表历史数据相关 **
 **********************/
void insert_his_data(MeterFileType *pmf, void *pData, struct tm *pNowTime,struct tm *pTimeNode, char *pErr);
uint8 read_his_data(char* timenode, mtype_idx idx, char* pErr);
uint8 read_all_his_data(char* timenode, char* pErr);
uint8 empty_all_hisdata(void);
int get_his_cnt(mtype_idx idx);
uint8 retrieve_his_data(mtype_idx idx, int cnt, int (*read_one_his)(pHis_data, uint8), uint8 dev);
uint8 retrieve_and_del_his_data(mtype_idx idx, int cnt, int (*read_one_his)(pHis_data, uint8), uint8 dev);
uint8 asc_to_datestr(char* src, char* dest);



#endif  //_DB_H_
