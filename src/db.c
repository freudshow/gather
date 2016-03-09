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
);//sqlite3 �������ݿ�ӿ�

sqlite3 *g_pDB;//���ݿ�ָ��, ˽�б���, ֻ����ͨ�����ļ��ṩ�ĺ�������, �������ⲿ�������

sys_config_str sys_config_array[SYS_CONFIG_COUNT];//���������б�, ���б���
int config_idx;//�������õĸ�������, ˽�б���
int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name);
int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name);


meter_info_List list_meter_info;//�Ǳ���Ϣ�б�, ���б���
int meter_info_idx;//���������ص��Ǳ�����������
void empty_meter_info_list();
void init_meter_info_list();//��



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
 **	 ������: read_meter_info
 ** ����	: �����ݿ��ж�ȡ�Ǳ��ַ��Ϣ, �ŵ�list_meter_info��
 ** ��Ҫ  : �������ʱ����list_meter_infoΪNULL, ���򱾳������ֶδ���
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
	if(!list_meter_info)
		empty_meter_info_list(list_meter_info);

	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(&col_buf[3], 1, 1, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	strcat(sql_buf, ";");
	//printf("%s\n", sql_buf);
	sqlite3_exec(g_pDB, sql_buf, each_meter_info, NULL, &pErr);
	free(sql_buf);
}

int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
	int i, j;
	int length;
	int low_idx;
	pMeter_info tmp_info = malloc(sizeof(struct meter_info_str));
	memset(tmp_info, 0, sizeof(struct meter_info_str));
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_MINFO_ID))//�Ǳ�ID
			tmp_info->f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_ADDRESS)) {//�Ǳ��ַ
			length = strlen(f_value[i]);
			for (j=0; j<(length+1)/BYTE_BCD_CNT;j++) {
				low_idx = length-BYTE_BCD_CNT*j-2;
				tmp_info->f_meter_address[LENGTH_B_METER_ADDRESS-1-j] = \
					(((low_idx < 0) ? 0: (f_value[i][low_idx]-'0')) << LEN_HALF_BYTE | (f_value[i][low_idx+1]-'0'));
			}
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_TYPE)) {//�Ǳ����ͱ���
			tmp_info->f_meter_type = (((f_value[i][0]-'0')<<4) | (f_value[i][1]-'0'));
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_CHANNEL))//�Ǳ�ͨ��
			tmp_info->f_meter_channel = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_POS))//�Ǳ�װλ��
			strcpy(tmp_info->f_install_pos, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_DEVICE_ID))//�Ǳ���豸���
			tmp_info->f_device_id = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_PROTO_TYPE))//�Ǳ��Э�����
			tmp_info->f_meter_proto_type = atoi(f_value[i]);
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

//����Ǳ���Ϣ, �Ա����¶�ȡ
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

//�����Ǳ���Ϣ, ��ÿһ�������read_one_meter����
void retrieve_meter_info_list(int (*read_one_meter)(pMeter_info))
{
	pMeter_info pInfo = list_meter_info;
	while(pInfo) {		
		if(read_one_meter)
			read_one_meter(pInfo);
		pInfo = pInfo->pNext;
	}
}


/********************************************************************************
 **	 ������: read_sys_config
 ** ����	: �����ݿ��ж�ȡ��������, �ŵ�sys_config_array��
 ********************************************************************************/
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
 ** char **fields, Ҫ�������������
 ** int f_cnt, Ҫ���������������
 ** int asc, �Ƿ������������
 ** char *sql, ���ص�sql�ַ���
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
