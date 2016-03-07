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
 **	 函数名: get_delete_sql
 ** 功能	: 组合插入语句, 可以有where从句
 ********************************************************************************/
void get_delete_sql(char *table_name, char **condition, int con_count, char *sql)
{
	//DELETE FROM Person WHERE LastName = 'Wilson'
	int i;
	char *where_buf;
	
	strcpy(sql, SQL_DELETE);
	strcat(sql, " ");
	strcat(sql, SQL_FROM);
	strcat(sql, " ");
	strcat(sql, table_name);
	//where
	if(con_count >= 0) {
		where_buf  = malloc(LENGTH_SQLCON);
		get_where_sql(condition, con_count, where_buf);
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
void get_update_sql(char *table_name, char **sets, int set_count, char **condition, int con_count, char *sql)
{
	//UPDATE Person SET Address = 'Zhongshan 23', City = 'Nanjing' WHERE LastName = 'Wilson'
	int i;
	char *where_buf;
	
	if(set_count <= 0) {
		memset(sql, 0, LENGTH_SQLBUF);
		return;
	}

	strcpy(sql, SQL_UPDATE);
	strcat(sql, " ");
	strcat(sql, table_name);
	strcat(sql, " ");
	strcat(sql, SQL_SET);	
	//sets
	for(i=0;i<set_count-1;i++) {
		strcat(sql, " ");
		strcat(sql, sets[i]);
		strcat(sql, ",");
	}
	strcat(sql, " ");
	strcat(sql, sets[i]);
	//where
	if(con_count >= 0) {
		where_buf  = malloc(LENGTH_SQLCON);
		get_where_sql(condition, con_count, where_buf);
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
void get_insert_sql(char *table_name, char **cols, int col_count, char **values,char *sql)
{
	int i;

	if(col_count <= 0) {
		memset(sql, 0, LENGTH_SQLBUF);
		return;
	}
	
	strcpy(sql, SQL_INSERT);
	strcat(sql, " ");
	strcat(sql, table_name);
	//columns
	strcat(sql, " ");
	strcat(sql, SQL_LEFT_PARENTHESIS);
	for(i=0;i<col_count-1;i++) {
		strcat(sql, cols[i]);
		strcat(sql, ", ");
	}
	strcat(sql, cols[i]);
	strcat(sql, SQL_RIGHT_PARENTHESIS);
	//values
	strcat(sql, SQL_VALUES);
	strcat(sql, SQL_LEFT_PARENTHESIS);
		for(i=0;i<col_count-1;i++) {
		strcat(sql, values[i]);
		strcat(sql, ", ");
	}
	strcat(sql, values[i]);
	strcat(sql, SQL_RIGHT_PARENTHESIS);

	strcat(sql, ";");
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
	char *select_buf = malloc(LENGTH_SQLBUF);
	char *where_buf;
	get_select_sql(table_name, cols, col_count, select_buf);
	strcpy(sql, select_buf);
	
	if(con_count >= 0) {
		where_buf  = malloc(LENGTH_SQLCON);
		get_where_sql(condition, con_count, where_buf);
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



int main(void)
{
	char *sqlbuf = malloc(LENGTH_SQLBUF);
	char *colbuf[LENGTH_F_COL_NAME]={"f_id","f_config_name", "f_config_value"};
	char *conditionbuf[LENGTH_SQLCONS]={"f_id>2","f_id<20", "f_config_name>13",\
												"f_id<8"};
	char *valuebuf[LENGTH_SQLVALUE]={"1", "'d41d8cd98f00b204e9800998ecf8427e'", "'213.5.1.2'"};
	char *setbuf[LENGTH_SQLSET]={"f_id=1", "f_config_name='f_time_node'", "f_type='varchar(10)'", "f_length=50", \
										"f_timestamp='2016-03-06 12:06:52'"};
	char *table_name = "t_base_define";
	get_query_sql(table_name, colbuf, 3, conditionbuf, 4, sqlbuf);
	printf("%s\n", sqlbuf);
	memset(sqlbuf, 0, LENGTH_SQLBUF);
	get_insert_sql(table_name, colbuf, 3, valuebuf,sqlbuf);
	printf("%s\n", sqlbuf);
	memset(sqlbuf, 0, LENGTH_SQLBUF);
	get_update_sql(table_name, setbuf, 5, conditionbuf, 4, sqlbuf);
	printf("%s\n", sqlbuf);
	memset(sqlbuf, 0, LENGTH_SQLBUF);
	get_delete_sql(table_name, conditionbuf, 4, sqlbuf);
	printf("%s\n", sqlbuf);
	memset(sqlbuf, 0, LENGTH_SQLBUF);
	return 0;
}

