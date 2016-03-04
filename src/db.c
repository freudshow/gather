/*
  *******************************************************************************
  * @file    db.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3/sqlite3.h>
#include "globaldefine.h" 
#include "db.h"

sqlite3 *g_pDB;
sys_config_str sys_config_array[SYS_CONFIG_COUNT];

/********************************************************************************
 **	 函数名: open_db
 ** 功能	: 打开数据库
 ********************************************************************************/
int open_db(void)
{
	 //int rc = sqlite3_open(SQLITE_NAME, &g_pDB);  
    //if (rc) {
	 	//sqlite3_close(g_pDB);
		//return SQLERR_OPEN_DB;
    //}
	 return SQLSUS_OPEN_DB;
}

/********************************************************************************
 **	 函数名: init_sys_config
 ** 功能	: 从数据库中读取基本参数, 放到sys_config_array中
 ********************************************************************************/
void read_sys_config(char *pErr)
{
	
}

/********************************************************************************
 **	 函数名: create_table
 ** 功能	: 创建数据表
 ********************************************************************************/
void create_table(char* sql, char *pErr)
{
	
}

/********************************************************************************
 **	 函数名: get_data
 ** 功能	: 从table_name中读取表数据
 **
 ********************************************************************************/
void get_data(char* table_name, char *pErr)
{
	
}

/********************************************************************************
 **	 函数名: get_query_sql
 ** 功能	: 组合查询语句, 可以有where从句
 **
 ********************************************************************************/
void get_query_sql(char *table_name, char **cols, int col_count, char **condition, int con_count, char *sql)
{
	int i;
	if(col_count <= 0) {
		memset(sql, 0, LENGTH_SQLBUF);
		return;
	}
	
	strcpy(sql, SQL_SELECT);
	strcat(sql, " ");
	for(i=0;i<col_count-1;i++) {
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
 **	 函数名: get_select_sql
 ** 功能	: 组合查询语句, 没有where从句
 **
 ********************************************************************************/
void get_select_sql(char *table_name, char **cols, int col_count, char *sql)
{
	int i;
	if(col_count <= 0) {
		memset(sql, 0, LENGTH_SQLBUF);
		return;
	}
	
	strcpy(sql, SQL_SELECT);
	strcat(sql, " ");
	for(i=0;i<col_count-1;i++) {
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
 **	 函数名: get_where_sql
 ** 功能	: 组合where从句
 **
 ********************************************************************************/
void get_where_sql(char **condition, int con_count, char *sql)
{
	int i;
	if(con_count <= 0) {
		memset(sql, 0, LENGTH_SQLBUF);
		return;
	}
	
	strcpy(sql, SQL_WHERE);
	
	for(i=0;i<con_count-1;i++) {
		strcat(sql, " ");
		strcat(sql, condition[i]);
		strcat(sql, " ");
		strcat(sql, SQL_AND);
	}
	strcat(sql, " ");
	strcat(sql, condition[i]);
}



int main()
{
	char *sqlbuf = malloc(LENGTH_SQLBUF);
	//char sqlbuf[LENTH_SQLBUF];
	char *cols[LENGTH_F_COL_NAME]={"col1=1","col2=2", "col3=3"};
	get_where_sql(cols, 3, sqlbuf);
	printf("%s\n", sqlbuf);
	return 0;
}

