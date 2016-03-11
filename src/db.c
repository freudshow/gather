/*
  *******************************************************************************
  * @file    db.c
  * @author  songbaoshan
  * @version V0.0.0
  * @date    03-03-2016
  * @brief   
  * @attention: 本文件的 get_*_sql() 函数, 结构与功能类似
  * 参数列表结构: (数据表名, [(列名, 列数量); (条件, 条件数量); ...], 要得到的SQL语句)
  * 无返回值.
  * 如果给出的[列名/条件]数量小于等于0, 那么就将传入的SQL指向的内存区域全部设置为
  * 空字符'\0', sql在传入之前要提前指向有效的内存区, 在函数内不再初始化.
  *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3/sqlite3.h>
#include "globaldefine.h" 
#include "db.h"

sqlite3 *g_pDB = NULL;//数据库指针, 私有变量, 只允许通过本文件提供的函数操作, 不允许外部代码操作

/**********************
 ** 读取系统配置相关 **
 **********************/
static sys_config_str sys_config_array[SYS_CONFIG_COUNT];//基本配置列表, 私有变量
static int config_idx;//基本配置的个数索引, 私有变量
static int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name);

/**********************
 ** 读取仪表信息相关 **
 **********************/
static meter_info_List list_meter_info = NULL;//仪表信息列表, 私有变量
static int meter_info_idx;//集中器挂载的仪表数量的索引, 私有变量
static int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name);
static void empty_meter_info_list();
/**********************
 ** 读取数据项相关 **
 **********************/
static request_data_list list_request_data = NULL;//仪表信息列表, 私有变量
static int request_data_idx;//配置数据项数量的索引, 私有变量
static int each_request_data(void *NotUsed, int f_cnt, char **f_value, char **f_name);
static void empty_request_data_list();


/********************************************************************************
 **	 函数名: open_db
 ** 功能	: 打开数据库
 ********************************************************************************/
int open_db(void)
{
	 return (sqlite3_open(SQLITE_NAME, &g_pDB) == SQLITE_OK) ? SQLITE_OK : -1;
}

/********************************************************************************
 **	 函数名: close_db
 ** 功能	: 关闭数据库
 ********************************************************************************/
int close_db(void)
{
	 return (sqlite3_close(g_pDB) == SQLITE_OK) ? SQLITE_OK : -1;
}

/********************************************************************************
 ** 功能区域	: 读取数据项
 ********************************************************************************/

void read_request_data(char *pErr, uint8 meter_type)
{
	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *where_buf = malloc(LENGTH_SQLCON);

	char *m_type = malloc(LENGTH_F_METER_TYPE);
	//char *con_buf[LENGTH_SQLCONS] = {FIELD_REQUEST_MTYPE};//这样初始化后, con_buf[0]指向const char*, 不能再进行连接, copy等操作
	char *con_buf = malloc(LENGTH_SQLCONS);
	char *table_name = TABLE_REQUEST_DATA;
	char *col_buf[LENGTH_F_COL_NAME] =  {FIELD_REQUEST_ID, \
				FIELD_REQUEST_MTYPE, FIELD_REQUEST_ITEMIDX, FIELD_REQUEST_COLNAME,\
				FIELD_REQUEST_COLTYPE};
	int  col_cnt = 5;

	request_data_idx = 0;
	sprintf(m_type, "%2x", meter_type);
	strcpy(con_buf, FIELD_REQUEST_MTYPE);
	strcat(con_buf, SQL_EQUAL);
	strcat(con_buf, m_type);
	
	empty_request_data_list();//清空以前的信息, 以重新读取
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_where_sql(&con_buf, 1, where_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, where_buf);
	strcat(sql_buf, ";");
	printf("%s\n", sql_buf);
	sqlite3_exec(g_pDB, sql_buf, each_request_data, NULL, &pErr);
	free(m_type);
	free(sql_buf);
	free(where_buf);
}

static int each_request_data(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
	int i;//i, 一条记录的字段名索引

	pRequest_data tmp_request = malloc(sizeof(struct request_data_str));
	memset(tmp_request, 0, sizeof(struct request_data_str));
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_REQUEST_ID))//数据项ID
			tmp_request->f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_MTYPE))//仪表类型
			tmp_request->f_meter_type= ((f_value[i][0] - ZERO_CHAR) << LEN_HALF_BYTE | (f_value[i][1] - ZERO_CHAR));
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_ITEMIDX)) {//仪表数据项的索引号
			printf(" %s ", f_value[i]);
			tmp_request->f_item_index= ((f_value[i][0] - ZERO_CHAR) << LEN_HALF_BYTE | (f_value[i][1] - ZERO_CHAR));
		}
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_COLNAME))//仪表数据项的列名
			strcpy(tmp_request->f_col_name, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_COLTYPE))//列名的类型
			strcpy(tmp_request->f_col_type, f_value[i]);
		else {//异常情况
			
		}
	}
	tmp_request->pPrev = NULL;
	tmp_request->pNext = list_request_data;
	if (list_request_data) {
		list_request_data->pPrev = tmp_request;
	}
	list_request_data = tmp_request;
	request_data_idx++;
	return 0;
}


void retrieve_request_data_list(int (*read_one_item)(pRequest_data))
{
	if(!read_one_item)
		return;
	
	pRequest_data tmp_request = list_request_data;
	while(tmp_request) {
		read_one_item(tmp_request);
		tmp_request = tmp_request->pNext;
	}
}

static void empty_request_data_list()
{
	pRequest_data tmp_request;
	while(list_request_data) {
		tmp_request = list_request_data;
		list_request_data = list_request_data->pNext;
		free(tmp_request);
	}
}

int  get_request_data_cnt()
{
	return request_data_idx;
}

/********************************************************************************
 **	 函数名: read_meter_info
 ** 功能	: 从数据库中读取仪表地址信息, 放到list_meter_info中
 ** 重要  : 程序加载时须置list_meter_info为NULL, 否则本程序会出现段错误
 ********************************************************************************/

void read_meter_info(char	*pErr)
{
	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *order_buf = malloc(LENGTH_SQLORDER);
	char *table_name = TABLE_METER_INFO;
	char *col_buf[LENGTH_F_COL_NAME] =  {FIELD_MINFO_ID, \
				FIELD_MINFO_ADDRESS, FIELD_MINFO_TYPE, FIELD_MINFO_CHANNEL,\
				FIELD_MINFO_POS, FIELD_MINFO_DEVICE_ID, FIELD_MINFO_PROTO_TYPE};
	int  col_cnt = 7;
	
	meter_info_idx = 0;
	empty_meter_info_list();//清空以前的信息, 以重新读取
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(&col_buf[3], 1, 1, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	strcat(sql_buf, ";");
	sqlite3_exec(g_pDB, sql_buf, each_meter_info, NULL, &pErr);
	free(sql_buf);
	free(order_buf);
}

static int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
	int i, j;//i, 一条记录的字段名索引; j表地址
	int length;//从数据表中读取的表地址字符串的长度
	int low_idx;//从数据表中读取的表地址字符串, 高位(低索引)下标
	pMeter_info tmp_info = malloc(sizeof(struct meter_info_str));
	memset(tmp_info, 0, sizeof(struct meter_info_str));
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_MINFO_ID))//仪表ID
			tmp_info->f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_ADDRESS)) {//仪表地址BCD
			length = strlen(f_value[i]);
			for (j=0; j<(length+1)/BYTE_BCD_CNT;j++) {
				low_idx = length-BYTE_BCD_CNT*j-2;
				/*tmp_info->f_meter_address[LENGTH_B_METER_ADDRESS-1-j] = \
					(((low_idx < 0) ? 0: (f_value[i][low_idx]-ZERO_CHAR)) << LEN_HALF_BYTE | (f_value[i][low_idx+1]-ZERO_CHAR));*/
				tmp_info->f_meter_address[j] = \
					(((low_idx < 0) ? 0: (f_value[i][low_idx]-ZERO_CHAR)) << LEN_HALF_BYTE | (f_value[i][low_idx+1]-ZERO_CHAR));
			}
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_TYPE)) {//仪表类型编码(BCD), 固定为两个字符
			if (strlen(f_value[i]) == BYTE_BCD_CNT) {
				tmp_info->f_meter_type = (((f_value[i][0]-ZERO_CHAR)<<LEN_HALF_BYTE) | (f_value[i][1]-ZERO_CHAR));
			}
			else {//异常情况
			}
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_CHANNEL))//仪表通道
			tmp_info->f_meter_channel = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_POS))//仪表安装位置
			strcpy(tmp_info->f_install_pos, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_DEVICE_ID))//仪表的设备编号
			tmp_info->f_device_id = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_PROTO_TYPE))//仪表的协议编码
			tmp_info->f_meter_proto_type = atoi(f_value[i]);
		else {//异常情况

		}
	}
	tmp_info->pPrev = NULL;
	tmp_info->pNext = list_meter_info;
	if (list_meter_info) {
		list_meter_info->pPrev = tmp_info;
	}
	list_meter_info = tmp_info;
	meter_info_idx++;
	return 0;
}

int get_meter_info_cnt()
{
	return meter_info_idx;
}

//清空仪表信息, 以便重新读取
//此函数运行的基础是, list必须先初始化为NULL
static void empty_meter_info_list()
{
	pMeter_info tmp_info;
	while(list_meter_info) {
		tmp_info = list_meter_info;
		list_meter_info = list_meter_info->pNext;
		free(tmp_info);
	}
}

//遍历仪表信息, 对每一个表进行read_one_meter操作
void retrieve_meter_info_list(int (*read_one_meter)(pMeter_info))
{
	if(!read_one_meter)
		return;

	pMeter_info pInfo = list_meter_info;
	while(pInfo) {
		read_one_meter(pInfo);
		pInfo = pInfo->pNext;
	}
}


/********************************************************************************
 **	 函数名: read_sys_config
 ** 功能	: 从数据库中读取基本参数, 放到sys_config_array中
 ********************************************************************************/
sys_config_str get_sys_config(enum T_System_Config idx)
{
	return sys_config_array[idx];
}

void read_sys_config(char *pErr)
{
	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *order_buf = malloc(LENGTH_SQLORDER);
	char *table_name = TABLE_BASE_DEF;
	char *col_buf[LENGTH_F_COL_NAME] = {FIELD_BASE_DEF_ID, FIELD_BASE_DEF_NAME, FIELD_BASE_DEF_VALUE};
	int	col_cnt = 3;

	config_idx = 0;
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(col_buf, 1, 0, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	sqlite3_exec(g_pDB, sql_buf, each_config, NULL, &pErr);
	free(sql_buf);
	free(order_buf);
}

int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
	int i;
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_BASE_DEF_ID))
			sys_config_array[config_idx].f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_BASE_DEF_NAME))
			strcpy(sys_config_array[config_idx].f_config_name, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_BASE_DEF_VALUE))
			strcpy(sys_config_array[config_idx].f_config_value, f_value[i]);
	}
	config_idx++;
	return 0;
}

int get_sys_config_cnt()
{
	return config_idx;
}


/********************************************************************************
 **	 函数名: create_table
 ** 功能	: 创建数据表
 ********************************************************************************/
void create_table(char* table_name, char *pErr)
{
	
}

/********************************************************************************
 **	 函数名: get_delete_sql
 ** 功能	: 组合插入语句, 可以有where从句
 ********************************************************************************/
void get_delete_sql(char *table_name, char **condition, int con_cnt, char *sql)
{
	char *where_buf;
	
	strcpy(sql, SQL_DELETE);
	strcat(sql, " ");
	strcat(sql, SQL_FROM);
	strcat(sql, " ");
	strcat(sql, table_name);
	//where
	if(con_cnt > 0) {
		where_buf  = malloc(LENGTH_SQLCON);
		get_where_sql(condition, con_cnt, where_buf);
		strcat(sql, " ");
		strcat(sql, where_buf);
		free(where_buf);
	}
	else {
		//警告: 所有数据都将删除, 请谨慎操作!
		
	}
	strcat(sql, ";");
}
/********************************************************************************
 **	 函数名: get_update_sql
 ** 功能	: 组合更新语句, 可以有where从句
 ********************************************************************************/
void get_update_sql(char *table_name, char **sets, int set_cnt, char **condition, int con_cnt, char *sql)
{
	int i;
	char *where_buf;
	
	if(set_cnt <= 0) {
		memset(sql, 0, sizeof(sql));
		return;
	}

	strcpy(sql, SQL_UPDATE);
	strcat(sql, " ");
	strcat(sql, table_name);
	strcat(sql, " ");
	strcat(sql, SQL_SET);	
	//sets
	for(i=0;i<set_cnt-1;i++) {
		strcat(sql, " ");
		strcat(sql, sets[i]);
		strcat(sql, ",");
	}
	strcat(sql, " ");
	strcat(sql, sets[i]);
	//where
	if(con_cnt > 0) {
		where_buf  = malloc(LENGTH_SQLCON);
		get_where_sql(condition, con_cnt, where_buf);
		strcat(sql, " ");
		strcat(sql, where_buf);
		free(where_buf);
	}
	strcat(sql, ";");
}


/********************************************************************************
 **	 函数名: get_insert_sql
 ** 功能	: 组合插入语句, 可以有where从句
 ** 由于列的数量必须与value的数量相等, 所以只输入列数量即可
 ********************************************************************************/
void get_insert_sql(char *table_name, char **cols, int col_cnt, char **values,char *sql)
{
	int i;

	if(col_cnt <= 0) {
		memset(sql, 0, sizeof(sql));
		return;
	}
	
	strcpy(sql, SQL_INSERT);
	strcat(sql, " ");
	strcat(sql, table_name);
	//columns
	strcat(sql, " ");
	strcat(sql, SQL_LEFT_PARENTHESIS);
	for(i=0;i<col_cnt-1;i++) {
		strcat(sql, cols[i]);
		strcat(sql, ", ");
	}
	strcat(sql, cols[i]);
	strcat(sql, SQL_RIGHT_PARENTHESIS);
	//values
	strcat(sql, SQL_VALUES);
	strcat(sql, SQL_LEFT_PARENTHESIS);
		for(i=0;i<col_cnt-1;i++) {
		strcat(sql, values[i]);
		strcat(sql, ", ");
	}
	strcat(sql, values[i]);
	strcat(sql, SQL_RIGHT_PARENTHESIS);

	strcat(sql, ";");
}

/********************************************************************************
 **	 函数名: get_query_sql
 ** 功能	: 组合查询语句, 可以有where从句
 **
 ********************************************************************************/
void get_query_sql(char *table_name, char **cols, int col_cnt, char **condition, int con_cnt, char *sql)
{
	char *select_buf = malloc(LENGTH_SQLBUF);
	char *where_buf;
	get_select_sql(table_name, cols, col_cnt, select_buf);
	strcpy(sql, select_buf);
	
	if(con_cnt > 0) {
		where_buf  = malloc(LENGTH_SQLCON);
		get_where_sql(condition, con_cnt, where_buf);
		strcat(sql, " ");
		strcat(sql, where_buf);
		free(where_buf);
	}
	free(select_buf);
	strcat(sql, ";");
}

/********************************************************************************
 **	 函数名: get_select_sql
 ** 功能	: 组合查询语句, 没有where从句
 **
 ********************************************************************************/
void get_select_sql(char *table_name, char **cols, int con_cnt, char *sql)
{
	int i;
	if(con_cnt <= 0) {
		memset(sql, 0, sizeof(sql));
		return;
	}
	
	strcpy(sql, SQL_SELECT);
	strcat(sql, " ");
	for(i=0;i<con_cnt-1;i++) {
		strcat(sql, cols[i]);
		strcat(sql, ", ");
	}
	strcat(sql, cols[i]);
	strcat(sql, " ");
	strcat(sql, SQL_FROM);
	strcat(sql, " ");
	strcat(sql, table_name);
}
/********************************************************************************
 **	 函数名: get_orderby_sql
 ** 功能	: 组合order by从句
 ** char **fields, 要进行排序的列名
 ** int f_cnt, 要进行排序的列数量
 ** int asc, 是否进行升序排序
 ** char *sql, 返回的sql字符串
 ********************************************************************************/
void get_orderby_sql(char **fields, int f_cnt, int asc, char *sql)
{
	int i;
	if(f_cnt <= 0){
		memset(sql, 0, sizeof(sql));
		return;
	}
	strcpy(sql, SQL_ORDER);
	for(i=0;i<f_cnt-1;i++) {
		strcat(sql, " ");
		strcat(sql, fields[i]);
		strcat(sql, ",");
	}
	strcat(sql, " ");
	strcat(sql, fields[i]);
	strcat(sql, " ");
	strcat(sql, asc ? "desc": "asc");
}
/********************************************************************************
 **	 函数名: get_where_sql
 ** 功能	: 组合where从句
 **
 ********************************************************************************/
void get_where_sql(char **condition, int con_cnt, char *sql)
{
	int i;
	if(con_cnt <= 0) {
		memset(sql, 0, sizeof(sql));
		return;
	}
	
	strcpy(sql, SQL_WHERE);
	for(i=0;i<con_cnt-1;i++) {
		strcat(sql, " ");
		strcat(sql, condition[i]);
		strcat(sql, " ");
		strcat(sql, SQL_AND);
	}
	strcat(sql, " ");
	strcat(sql, condition[i]);
}
