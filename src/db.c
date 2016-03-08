/*
  *******************************************************************************
  * @file    db.c
  * @author  songbaoshan
  * @version V0.0.0
  * @date    03-03-2016
  * @brief   
  * @attention: ���ļ��� get_*_sql() ����, �ṹ�빦������
  * �����б�ṹ: (���ݱ���, [(����, ������); (����, ��������); ...], Ҫ�õ���SQL���)
  * �޷���ֵ.
  * ���������[����/����]����С�ڵ���0, ��ô�ͽ������SQLָ����ڴ�����ȫ������Ϊ
  * ���ַ�'\0', sql�ڴ���֮ǰҪ��ǰָ����Ч���ڴ���, �ں����ڲ��ٳ�ʼ��.
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


sqlite3 *g_pDB;
sys_config_str sys_config_array[SYS_CONFIG_COUNT];
int row_cnt;//���ݱ��ص�����

/********************************************************************************
 **	 ������: open_db
 ** ����	: �����ݿ�
 ********************************************************************************/
int open_db(void)
{
	 return (sqlite3_open(SQLITE_NAME, &g_pDB) == SQLITE_OK) ? SQLITE_OK : -1;
}

/********************************************************************************
 **	 ������: close_db
 ** ����	: �ر����ݿ�
 ********************************************************************************/
int close_db(void)
{
	 return (sqlite3_close(g_pDB) == SQLITE_OK) ? SQLITE_OK : -1;
}


/********************************************************************************
 **	 ������: init_sys_config
 ** ����	: �����ݿ��ж�ȡ��������, �ŵ�sys_config_array��
 ********************************************************************************/
void read_sys_config(char *pErr)
{
	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *order_buf = malloc(LENGTH_SQLORDER);
	char *table_name = TABLE_BASE_DEF;
	char *col_buf[LENGTH_F_COL_NAME] = {FIELD_BASE_DEF_ID, FIELD_BASE_DEF_NAME, FIELD_BASE_DEF_VALUE};
	int	col_cnt = 3;

	row_cnt = 0;
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(col_buf, 1, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	printf("%s\n", sql_buf);
	sqlite3_exec(g_pDB, sql_buf, each_config, NULL, &pErr);
	free(sql_buf);
}

int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
	int i;
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_BASE_DEF_ID))
			sys_config_array[row_cnt].f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_BASE_DEF_NAME))
			strcpy(sys_config_array[row_cnt].f_config_name, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_BASE_DEF_VALUE))
			strcpy(sys_config_array[row_cnt].f_config_value, f_value[i]);
	}
	row_cnt++;
	return 0;
}


/********************************************************************************
 **	 ������: create_table
 ** ����	: �������ݱ�
 ********************************************************************************/
void create_table(char* table_name, char *pErr)
{
	
}

/********************************************************************************
 **	 ������: get_delete_sql
 ** ����	: ��ϲ������, ������where�Ӿ�
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
		//����: �������ݶ���ɾ��, ���������!
		
	}
	strcat(sql, ";");
}
/********************************************************************************
 **	 ������: get_update_sql
 ** ����	: ��ϸ������, ������where�Ӿ�
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
 **	 ������: get_insert_sql
 ** ����	: ��ϲ������, ������where�Ӿ�
 ** �����е�����������value���������, ����ֻ��������������
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
 **	 ������: get_query_sql
 ** ����	: ��ϲ�ѯ���, ������where�Ӿ�
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
 **	 ������: get_select_sql
 ** ����	: ��ϲ�ѯ���, û��where�Ӿ�
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
 **	 ������: get_orderby_sql
 ** ����	: ���order by�Ӿ�
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
 **	 ������: get_where_sql
 ** ����	: ���where�Ӿ�
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

#if 1
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
	char pErr[100];
	memset(pErr, 0, 100);

	open_db();
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
	get_orderby_sql(colbuf, 3, sqlbuf);
	printf("%s\n", sqlbuf);
	memset(sqlbuf, 0, LENGTH_SQLBUF);
	
	read_sys_config(pErr);
	printf("%s\n", pErr);
	/*1*/printf("%d, %s, %s\n", sys_config_array[CONFIG_PRIMARY_SERVER].f_id, \
		sys_config_array[CONFIG_PRIMARY_SERVER].f_config_name, \
		sys_config_array[CONFIG_PRIMARY_SERVER].f_config_value);
	/*2*/printf("%d, %s, %s\n", sys_config_array[CONFIG_PRIMARY_DNS].f_id, \
		sys_config_array[CONFIG_PRIMARY_DNS].f_config_name, \
		sys_config_array[CONFIG_PRIMARY_DNS].f_config_value);
	/*3*/printf("%d, %s, %s\n", sys_config_array[CONFIG_PRIMARY_PORT].f_id, \
		sys_config_array[CONFIG_PRIMARY_PORT].f_config_name, \
		sys_config_array[CONFIG_PRIMARY_PORT].f_config_value);
	/*4*/printf("%d, %s, %s\n", sys_config_array[CONFIG_SECOND_SERVER].f_id, \
		sys_config_array[CONFIG_SECOND_SERVER].f_config_name, \
		sys_config_array[CONFIG_SECOND_SERVER].f_config_value);
	/*5*/printf("%d, %s, %s\n", sys_config_array[CONFIG_SECOND_DNS].f_id, \
		sys_config_array[CONFIG_SECOND_DNS].f_config_name, \
		sys_config_array[CONFIG_SECOND_DNS].f_config_value);
	/*6*/printf("%d, %s, %s\n", sys_config_array[CONFIG_SECOND_PORT].f_id, \
		sys_config_array[CONFIG_SECOND_PORT].f_config_name, \
		sys_config_array[CONFIG_SECOND_PORT].f_config_value);
	/*7*/printf("%d, %s, %s\n", sys_config_array[CONFIG_GATEWAY_ID].f_id, \
		sys_config_array[CONFIG_GATEWAY_ID].f_config_name, \
		sys_config_array[CONFIG_GATEWAY_ID].f_config_value);
	/*8*/printf("%d, %s, %s\n", sys_config_array[CONFIG_NET_TYPE].f_id, \
		sys_config_array[CONFIG_NET_TYPE].f_config_name, \
		sys_config_array[CONFIG_NET_TYPE].f_config_value);
	/*9*/printf("%d, %s, %s\n", sys_config_array[CONFIG_MD5_KEY].f_id, \
		sys_config_array[CONFIG_MD5_KEY].f_config_name, \
		sys_config_array[CONFIG_MD5_KEY].f_config_value);
	/*10*/printf("%d, %s, %s\n", sys_config_array[CONFIG_AES_KEY].f_id, \
		sys_config_array[CONFIG_AES_KEY].f_config_name, \
		sys_config_array[CONFIG_AES_KEY].f_config_value);
	/*11*/printf("%d, %s, %s\n", sys_config_array[CONFIG_COLLECT_MODE].f_id, \
		sys_config_array[CONFIG_COLLECT_MODE].f_config_name, \
		sys_config_array[CONFIG_COLLECT_MODE].f_config_value);
	/*12*/printf("%d, %s, %s\n", sys_config_array[CONFIG_COLLECT_CYCLE].f_id, \
		sys_config_array[CONFIG_COLLECT_CYCLE].f_config_name, \
		sys_config_array[CONFIG_COLLECT_CYCLE].f_config_value);
	/*13*/printf("%d, %s, %s\n", sys_config_array[CONFIG_REPORT_MODE].f_id, \
		sys_config_array[CONFIG_REPORT_MODE].f_config_name, \
		sys_config_array[CONFIG_REPORT_MODE].f_config_value);
	/*14*/printf("%d, %s, %s\n", sys_config_array[CONFIG_BEAT_CYCLE].f_id, \
		sys_config_array[CONFIG_BEAT_CYCLE].f_config_name, \
		sys_config_array[CONFIG_BEAT_CYCLE].f_config_value);

	close_db();
	return 0;
}
#endif
