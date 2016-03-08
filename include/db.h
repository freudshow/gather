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
 **			���ݿ��ȡʱ��callback �ص�������������
 **	callback(void *NotUsed, int argc, char **argv, char **azColName)	
 **  int argc --- ���ж��ٸ��ֶ�
 **	char **argv --- ָ������ֶ�ָ�������ָ�룬*argv ��Ϊĳ�ֶ����ݿ�ʼ��ַ
 **	char **azColName --- ָ������ֶ�����ָ�������ָ�룬
 **        *azColName ��Ϊĳ�ֶ����ƵĿ�ʼ��ַ
 **
 ********************************************************************************/

#ifndef _DB_H_
#define _DB_H_

/********************************************************************************
 ** SQL����
 ********************************************************************************/
#define SQL_SELECT			"select"
#define SQL_DELETE			"delete"
#define SQL_INSERT			"insert into"
#define SQL_DROP				"drop table if exists"
#define SQL_CREATE_TABLE	"create table"
#define SQL_UPDATE			"update"
#define SQL_SET				"set"

/********************************************************************************
 ** SQL��������
 ********************************************************************************/
#define SQL_FROM		"from"
#define SQL_WHERE	"where"
#define SQL_AND		"and"
#define SQL_VALUES	"values"
#define SQL_ORDER	"order by"
/********************************************************************************
 ** SQL����
 ********************************************************************************/
#define SQL_STAR					"*"
#define SQL_LEFT_PARENTHESIS	"("
#define SQL_RIGHT_PARENTHESIS	")"
#define SQL_SINGLE_QUOTES		"'"
#define SQL_EQUAL				"="

/********************************************************************************
 ** SQL���ݱ�����
 ********************************************************************************/
#define TABLE_BASE_DEF		"t_base_define"//�������ñ�
#define TABLE_REQUEST_DATA	"t_request_data"//�������Ǳ���Ҫ���������
#define TABLE_ELEC			"t_elec_data"//���
#define TABLE_WATER			"t_water_data"//ˮ��
#define TABLE_HEAT			"t_heat_data"//������
#define TABLE_AIR			"t_air_data"//��Ȼ����


/********************************************************************************
 ** ���ַ����ֶεĳ���
 ********************************************************************************/
#define LENGTH_F_CONFIG_NAME		16//����������
#define LENGTH_F_CONFIG_VALUE		50//����ֵ����
#define LENGTH_F_METER_ADDRESS	50//���ַ����
#define LENGTH_F_INSTALL_POS		50//�Ǳ�װλ�ó���
#define LENGTH_F_TIMESTAMP			50//ʱ�������
#define LENGTH_F_TIME				50//����ʱ��㳤��
#define LENGTH_F_COL_NAME			20//��������������
#define LENGTH_F_COL_TYPE			20//����������������
#define LENGTH_F_TIME_NAME			20//ʱ������Ƴ���
#define LENGTH_F_TIME_NODE			100//ʱ������ó���

#define LENGTH_SQLBUF		4096//sql������󳤶�; �е����ܳ�, ���Գ��ȶ��ĳ�Щ
#define LENGTH_SQLCONS		50//һ������������󳤶�, ��"col_number_id=23"
#define LENGTH_SQLCON		1024//һ�������Ӿ����󳤶�, ��"col_number_id=23 and col_number2_id=24"
#define LENGTH_SQLINSERT	2048//һ��insert���(����where�Ӿ�)����󳤶�
#define LENGTH_SQLVALUE		50//һ��������ֵ����󳤶�
#define LENGTH_SQLSET		200//һ��set�Ӿ����󳤶�
#define LENGTH_SQLORDER		200//һ��order by�Ӿ����󳤶�


/********************************************************************************
 ** t_base_define
 ** ����������Ϣ��, ��t_base_define.f_id��������Ӧ������
 ********************************************************************************/
#define SYS_CONFIG_COUNT	14//���������������

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
 ** �豸��ַ��Ϣ��
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
 ** �Ǳ���Ҫ���ص����������ñ�
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
 ** �Ǳ���Ҫ���ص����������ñ�
 ********************************************************************************/
 typedef struct {
	char f_time_name[LENGTH_F_TIME_NAME];
	char f_time_node[LENGTH_F_TIME_NODE];
	int f_id;
}time_node_str;
typedef time_node_str *pTime_node;
/********************************************************************************
 ** �Ǳ���ʷ������
 ********************************************************************************/
typedef struct {
	char f_timestamp[LENGTH_F_TIMESTAMP];//ʱ���
	char f_time[LENGTH_F_TIME];//����ʱ���
	char f_meter_address[LENGTH_F_METER_ADDRESS];//�Ǳ��ַ
	int  f_device_id;//�Ǳ���豸���
	int  f_id;//����ֵ
	void *pValues;//�������б�
	int value_len;//�������
}his_data_str;
typedef his_data_str *pHis_data;


/********************************************************************************
 ** ȫ������
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
