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
 **	 ������: open_db
 ** ����	: �����ݿ�
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
 **	 ������: init_sys_config
 ** ����	: �����ݿ��ж�ȡ��������, �ŵ�sys_config_array��
 ********************************************************************************/
void read_sys_config(char *pErr)
{
	
}

/********************************************************************************
 **	 ������: create_table
 ** ����	: �������ݱ�
 ********************************************************************************/
void create_table(char* sql, char *pErr)
{
	
}

/********************************************************************************
 **	 ������: get_data
 ** ����	: ��table_name�ж�ȡ������
 **
 ********************************************************************************/
void get_data(char* table_name, char *pErr)
{
	
}

/********************************************************************************
 **	 ������: get_query_sql
 ** ����	: ��ϲ�ѯ���, ������where�Ӿ�
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
 **	 ������: get_select_sql
 ** ����	: ��ϲ�ѯ���, û��where�Ӿ�
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
 **	 ������: get_where_sql
 ** ����	: ���where�Ӿ�
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

