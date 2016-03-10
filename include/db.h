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

#include "globaldefine.h"
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
#define TABLE_ELEC			"t_elec_data"//�����ʷ����
#define TABLE_WATER			"t_water_data"//ˮ����ʷ����
#define TABLE_HEAT			"t_heat_data"//��������ʷ����
#define TABLE_AIR			"t_air_data"//��Ȼ������ʷ����
#define TABLE_METER_INFO	"t_meter_info"//�Ǳ������Ϣ��
#define TABLE_TIME_NODE		"t_time_node"//ʱ�����ñ�

/********************************************************************************
 ** �ַ��������
 ********************************************************************************/
#define LEN_BYTE	8//һ���ֽڵı�����
#define LEN_HALF_BYTE	4//һ���ֽڵı�����
#define BYTE_BCD_CNT	2//һ���ֽ��ɶ��ٸ�BCD���ʾ

/********************************************************************************
 ** �ַ����
 ********************************************************************************/
#define ZERO_CHAR	0x30//char '0'

/********************************************************************************
 ** ���ַ����ֶεĳ��� F ����Field
 ********************************************************************************/
#define LENGTH_F_CONFIG_NAME		16//����������
#define LENGTH_F_CONFIG_VALUE		50//����ֵ����
#define LENGTH_F_METER_ADDRESS	14//�����ݱ��д洢���Ǳ��ַ����, ���14���ַ�
#define LENGTH_B_METER_ADDRESS	7//�ڳ����д�������Ǳ��ַ����, ���7�ֽ�, B����Byte
#define LENGTH_F_METER_TYPE		2//�����ݱ��д洢���Ǳ����ͳ���, ���2���ַ�

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
#define FIELD_MINFO_ID				"f_id"
#define FIELD_MINFO_ADDRESS		"f_meter_address"
#define FIELD_MINFO_TYPE			"f_meter_type"
#define FIELD_MINFO_CHANNEL		"f_meter_channel"
#define FIELD_MINFO_POS				"f_install_pos"
#define FIELD_MINFO_DEVICE_ID		"f_device_id"
#define FIELD_MINFO_PROTO_TYPE	"f_meter_proto_type"

struct meter_info_str{
	uint32 f_id;//������, �����ŵ�ǰ��, ����ֵ��uint8[]���͵������д
	uint8 f_meter_address[LENGTH_B_METER_ADDRESS];//���ַ7�ֽ�
	uint8 f_meter_type;//1�ֽ�
	uint8 f_meter_channel;//1�ֽ�
	uint16 f_device_id;//2�ֽ�
	uint8 f_meter_proto_type;//1�ֽ�

	//��װλ��, 50�ֽ�; Ӧ�÷ŵ�ֵ���������, 
	//������Ϊ"��װλ��"�ĳ��ȳ���LENGTH_F_INSTALL_POS, 
	//�Ժ���ĳ�Ա�������и�д.
	//ͨ��ָ���Ա�Ĳ���������ֵ��Ա�Ĳ�����ɺ����,
	//����"��װλ��"�����һ�㲻������ָ�����ͳ�Ա��ֵ�仯
	char f_install_pos[LENGTH_F_INSTALL_POS];
	
	struct meter_info_str* pNext;//�¸�Ԫ��
	struct meter_info_str* pPrev;//�ϸ�Ԫ��
};
typedef struct meter_info_str *pMeter_info;
typedef pMeter_info meter_info_List;

/********************************************************************************
 **	 t_request_data
 ** �Ǳ���Ҫ���ص����������ñ�
 ********************************************************************************/
#define MTYPE_WATER	0x10//ˮ���Ǳ���
#define MTYPE_HEAT	0x20//�������Ǳ���
#define MTYPE_ELECT	0x40//����Ǳ���

#define FIELD_REQUEST_ID			"f_id"
#define FIELD_REQUEST_MTYPE		"f_meter_type"
#define FIELD_REQUEST_ITEMIDX		"f_item_index"
#define FIELD_REQUEST_COLNAME		"f_col_name"
#define FIELD_REQUEST_COLTYPE		"f_col_type"


 
struct request_data_str{
	int32 f_id;
	uint8 f_meter_type;
	uint8 f_item_index;
	char f_col_name[LENGTH_F_COL_NAME];
	char f_col_type[LENGTH_F_COL_TYPE];
	struct request_data_str *pNext;
};
typedef struct request_data_str *pRequest_data;
typedef pRequest_data request_data_list;
/********************************************************************************
 **	 t_time_node
 ** ʱ������ñ�
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
enum T_His_Field{
	STRING = 0,
	INT,
	FLOAT
};

struct his_field_data{
	char *pData;
	char field_name[LENGTH_F_COL_NAME];
	enum T_His_Field eData_type;
	struct his_field_data *pNext;
};
typedef struct his_field_data *pHis_field_data;
typedef pHis_field_data his_data_list;

struct his_data_str{
	char f_timestamp[LENGTH_F_TIMESTAMP];//ʱ���
	char f_time[LENGTH_F_TIME];//����ʱ���
	char f_meter_address[LENGTH_F_METER_ADDRESS];//�Ǳ��ַ
	int  f_device_id;//�Ǳ���豸���
	int  f_id;//����ֵ
	uint8  f_meter_type;//�Ǳ�����
	his_data_list value_list;//����������
	int value_cnt;//�����������
	struct his_data_str* pNext;//�¸�Ԫ��
	struct his_data_str* pPrev;//�ϸ�Ԫ��
};
typedef struct his_data_str *pHis_data;
typedef pHis_data his_data_List;


/********************************************************************************
 *******************          ȫ������       ************************************
 ********************************************************************************/
 
/***************************************
 ** ���ݿ����ر�, sql��������� **
 ***************************************/
int open_db(void);
int close_db(void);
void get_select_sql(char *table_name, char **cols, int col_count, char *sql);
void get_where_sql(char **condition, int con_count, char *sql);
void get_orderby_sql(char **fields, int f_cnt, int asc, char *sql);
void get_query_sql(char *table_name, char **cols, int col_count, char **condition, int con_count, char *sql);
void get_insert_sql(char *table_name, char **cols, int col_count, char **values,char *sql);
void get_update_sql(char *table_name, char **sets, int set_count, char **condition, int con_count, char *sql);
void get_delete_sql(char *table_name, char **condition, int con_count, char *sql);
/**********************
 ** ϵͳ���ò������ **
 **********************/
void read_sys_config(char *pErr);//�����ݿ��ȡ��������
sys_config_str get_sys_config(enum T_System_Config);//��ȡ����һ������
int get_sys_config_cnt();//��ȡϵͳ���ò����ĸ���

/**********************
 ** �Ǳ��ַ��Ϣ��� **
 **********************/
void read_meter_info(char	*pErr);//�����ݿ��ȡ�Ǳ��ַ��Ϣ
void retrieve_meter_info_list(int (*read_one_meter)(pMeter_info));//˳������Ǳ��ַ��Ϣ
int  get_meter_info_cnt();//��ȡ�Ǳ��ַ��Ϣ�ĸ���

/**********************
 ** ��ȡ��������� **
 **********************/
void read_request_data(char	*pErr, uint8);//�����Ǳ����Ͷ�ȡ������
void retrieve_request_data_list(int (*read_one_meter)(pMeter_info));//˳�������������Ϣ
int  get_request_data_cnt();//��ȡ�Ǳ�������ĸ���

#endif  //_DB_H_
