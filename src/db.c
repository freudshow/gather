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

extern SQLITE_API int SQLITE_STDCALL sqlite3_open(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);

int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name);
int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name);
void empty_meter_info_list();
void init_meter_info_list();


sqlite3 *g_pDB;
sys_config_str sys_config_array[SYS_CONFIG_COUNT];
int config_cnt;//数据表返回的行数
int meter_info_cnt;//集中器挂载的仪表数量
meter_info_List list_meter_info;

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
	
	meter_info_cnt = 0;
	if(!list_meter_info)
		empty_meter_info_list(list_meter_info);

	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(&col_buf[5], 1, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	strcat(sql_buf, ";");
	printf("%s\n", sql_buf);
	sqlite3_exec(g_pDB, sql_buf, each_meter_info, NULL, &pErr);
	free(sql_buf);
}

int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
	int i;
	pMeter_info tmp_info = malloc(sizeof(struct meter_info_str));
	
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_MINFO_ID))
			tmp_info->f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_ADDRESS))
			strcpy(tmp_info->f_meter_address, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_TYPE))
			tmp_info->f_meter_type = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_CHANNEL))
			tmp_info->f_meter_channel = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_POS))
			strcpy(tmp_info->f_install_pos, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_DEVICE_ID))
			tmp_info->f_device_id = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_PROTO_TYPE))
			tmp_info->f_meter_proto_type = atoi(f_value[i]);
	}
	tmp_info->pPrev = NULL;
	tmp_info->pNext = list_meter_info;
	if (list_meter_info) {
		list_meter_info->pPrev = tmp_info;
	}
	list_meter_info = tmp_info;
	meter_info_cnt++;
	return 0;
}

//清空仪表信息, 以便重新读取
void empty_meter_info_list(meter_info_List list)
{
	pMeter_info pInfo, tmp_info;
	pInfo = list;
	while(pInfo) {
		tmp_info = pInfo;
		pInfo = pInfo->pNext;
		free(tmp_info);
	}
	init_meter_info_list();
}

void init_meter_info_list()
{
	list_meter_info = NULL;
}

//遍历仪表信息, 对每一个表进行read_one_meter操作
void retrieve_meter_info_list(meter_info_List list, int (*read_one_meter)(pMeter_info))
{
	pMeter_info pInfo = list;
	while(pInfo) {
		printf("%d, %d, %d, %s, %d, %d, %s\n", pInfo->f_id, pInfo->f_meter_type, pInfo->f_device_id,\
				pInfo->f_meter_address, pInfo->f_meter_channel, pInfo->f_meter_proto_type, pInfo->f_install_pos);
		if(read_one_meter)
			read_one_meter(pInfo);
		pInfo = pInfo->pNext;
	}
}


/********************************************************************************
 **	 函数名: read_sys_config
 ** 功能	: 从数据库中读取基本参数, 放到sys_config_array中
 ********************************************************************************/
void read_sys_config(char *pErr)
{
	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *order_buf = malloc(LENGTH_SQLORDER);
	char *table_name = TABLE_BASE_DEF;
	char *col_buf[LENGTH_F_COL_NAME] = {FIELD_BASE_DEF_ID, FIELD_BASE_DEF_NAME, FIELD_BASE_DEF_VALUE};
	int	col_cnt = 3;

	config_cnt = 0;
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(col_buf, 1, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	sqlite3_exec(g_pDB, sql_buf, each_config, NULL, &pErr);
	free(sql_buf);
}

int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
	int i;
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_BASE_DEF_ID))
			sys_config_array[config_cnt].f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_BASE_DEF_NAME))
			strcpy(sys_config_array[config_cnt].f_config_name, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_BASE_DEF_VALUE))
			strcpy(sys_config_array[config_cnt].f_config_value, f_value[i]);
	}
	config_cnt++;
	return 0;
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
 **
 ********************************************************************************/
void get_orderby_sql(char **fields, int f_cnt, char *sql)
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
