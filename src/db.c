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
 **	 ������: get_delete_sql
 ** ����	: ��ϲ������, ������where�Ӿ�
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
		//����: �������ݶ���ɾ��, ���������!
		
	}
	strcat(sql, ";");
}
/********************************************************************************
 **	 ������: get_update_sql
 ** ����	: ��ϸ������, ������where�Ӿ�
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
 **	 ������: get_insert_sql
 ** ����	: ��ϲ������, ������where�Ӿ�
 ** �����е�����������value���������, ����ֻ��������������
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

