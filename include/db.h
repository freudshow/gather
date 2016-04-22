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
#include "readallmeters.h"
#include "read_heatmeter.h"

/*�������ڵ��ͨ��ģʽ, ǰ��������ṹ���ڵ�Ԫ��ָ���������ΪpNext��pPrev
 *����, ����ӽڵ�ĺ��ָ��϶�Ҫָ��ǰ������ͷ.
 *�����ǰ������ͷΪ��, ��ô����ӵĽڵ��ǰ��ָ���ָ���Լ�, ���Լ����Ǳ�β.
 *�����ǰ������ͷ����, ��ô��ǰ�����ǰ��ָ���ָ���β, �Ͱ������β�ĵ�ַ
 *��������ӽڵ��ǰ��ָ��, Ȼ���ٰѱ���ӽڵ�ĵ�ַ������ǰ����ͷ����ǰ��ָ��.
 *���, �ѵ�ǰ����ͷָ����ӽڵ�ĵ�ַ.
 *������������, �����ͷ��β���Ժܷ�����ҵ�
 */
#define add_node(pListHead, pNode)  pNode->pNext = pListHead;\
                    if (pListHead) {\
                        pNode->pPrev = pListHead->pPrev;/*����β��*/\
                        pListHead->pPrev = pNode;\
                    }\
                    else {\
                            pNode->pPrev = pNode;\
                    }\
                    pListHead = pNode;

#define empty_list(listType, list)   listType tmpNode;\
                                    while(list) {\
                                        tmpNode = list;\
                                        list = list->pNext;\
                                        free(tmpNode);\
                                    }
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
 ** SQL�������
 ********************************************************************************/
#define SQL_MTYPE_CNT "select distinct f_meter_type from t_request_data"//��ȡ�Ǳ�����������SQL���
#define SQL_MTYPE	  "select count(distinct f_meter_type) from t_request_data"//��ȡ�Ǳ�����������SQL���


/********************************************************************************
 ** SQL���ݱ�����
 ********************************************************************************/
#define TABLE_BASE_DEF		"t_base_define"//�������ñ�
#define TABLE_REQUEST_DATA	"t_request_data"//�������Ǳ���Ҫ���������
#define TABLE_ELEC			"t_elect_data"//�����ʷ����
#define TABLE_WATER			"t_water_data"//ˮ����ʷ����
#define TABLE_HEAT			"t_heat_data"//��������ʷ����
#define TABLE_GAS			"t_air_data"//��Ȼ������ʷ����
#define TABLE_METER_INFO	"t_meter_info"//�Ǳ������Ϣ��
#define TABLE_TIME_NODE		"t_time_node"//ʱ�����ñ�



/********************************************************************************
 ** �ַ����
 ********************************************************************************/
#define ZERO_CHAR	0x30//char '0'

/********************************************************************************
 ** ���ַ����ֶεĳ��� F ����Field
 ********************************************************************************/
#define LENGTH_F_CONFIG_NAME		16//����������
#define LENGTH_F_CONFIG_VALUE		50//����ֵ����
#define LENGTH_F_METER_ADDRESS	    15//�����ݱ��д洢���Ǳ��ַ����, ���14���ַ�, ���Ͻ�����'\0', ����15���ַ�
#define LENGTH_F_METER_TYPE		2//�����ݱ��д洢���Ǳ����ͳ���, ���2���ַ�

#define LENGTH_F_ID                 6//�������ŵ���󳤶�
#define LENGTH_F_MTYPE              2//�Ǳ����ͱ�ŵĳ���
#define LENGTH_F_DEVID              3//�豸��ŵ���󳤶�
#define LENGTH_F_TIMESTAMP			50//ʱ�������
#define LENGTH_F_TIME				50//����ʱ��㳤��
#define LENGTH_F_COL_NAME			20//��������������
#define LENGTH_F_VALUE				100//������ʷ������󳤶�
#define LENGTH_F_COL_TYPE			20//����������������
#define LENGTH_F_TIME_NAME			20//ʱ������Ƴ���
#define LENGTH_F_TIME_NODE			100//ʱ������ó���

#define LENGTH_SQLBUF		4096//sql������󳤶�; �е����ܳ�, ���Գ��ȶ��ĳ�Щ
#define LENGTH_SQLCONS		50//һ������������󳤶�, ��"col_number_id=23"
#define LENGTH_SQLCON		1024//һ�������Ӿ����󳤶�, ��"col_number_id=23 and col_number2_id=24"
#define LENGTH_SQLINSERT	4096//һ��insert���(����where�Ӿ�)����󳤶�
#define LENGTH_SQLVALUE		50//һ��������ֵ����󳤶�
#define LENGTH_SQLSET		200//һ��set�Ӿ����󳤶�
#define LENGTH_SQLORDER		200//һ��order by�Ӿ����󳤶�

/********************************************************************************
 ** t_base_define
 ** ����������Ϣ��, ��t_base_define.f_id��������Ӧ������
 ********************************************************************************/

#define FIELD_BASE_DEF_ID		"f_id"
#define FIELD_BASE_DEF_NAME	"f_config_name"
#define FIELD_BASE_DEF_VALUE	"f_config_value"

struct sys_config_structure;
typedef struct sys_config_structure sys_config_str;
typedef sys_config_str *pSys_config;
typedef pSys_config sys_config_list;

struct sys_config_structure{
	char f_config_name[LENGTH_F_CONFIG_NAME];
	char f_config_value[LENGTH_F_CONFIG_VALUE];
	int f_id;
	pSys_config pNext;
	pSys_config pPrev;
};


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

struct meter_info_str;
typedef struct meter_info_str *pMeter_info;
typedef pMeter_info meter_info_List;

struct meter_info_str{
	uint32 f_id;//������, �����ŵ�ǰ��, ����ֵ��uint8[]���͵������дHEX
	uint8 f_meter_address[LENGTH_B_METER_ADDRESS];//���ַ7�ֽ�BCD
	uint8 f_meter_type;//1�ֽ�BCD
	uint8 f_meter_channel;//1�ֽ�HEX
	uint16 f_device_id;//2�ֽ�HEX
	uint8 f_meter_proto_type;//1�ֽ�HEX

	//��װλ��, 50�ֽ�; Ӧ�÷ŵ�ֵ���������, 
	//������Ϊ"��װλ��"�ĳ��ȳ���LENGTH_F_INSTALL_POS, 
	//�Ժ���ĳ�Ա�������и�д.
	//ͨ��ָ���Ա�Ĳ���������ֵ��Ա�Ĳ�����ɺ����,
	//����"��װλ��"�����һ�㲻������ָ�����ͳ�Ա��ֵ�仯
	char f_install_pos[LENGTH_F_INSTALL_POS];
	
	pMeter_info pNext;//�¸�Ԫ��
	pMeter_info pPrev;//�ϸ�Ԫ��
};


/********************************************************************************
 **	 t_request_data
 ** �Ǳ���Ҫ���ص����������ñ�
 ********************************************************************************/
#define FIELD_REQUEST_ID			"f_id"
#define FIELD_REQUEST_MTYPE		"f_meter_type"
#define FIELD_REQUEST_ITEMIDX		"f_item_index"
#define FIELD_REQUEST_COLNAME		"f_col_name"
#define FIELD_REQUEST_COLTYPE		"f_col_type"

typedef enum Type_His_Data{//��ʷ���ݵĴ洢��������
    STRING = 0,
    INT,
    FLOAT
}EM_DType;

struct request_data_str;
typedef struct request_data_str* pRequest_data;
typedef pRequest_data request_data_list;

struct request_data_str{
	int32 f_id;
	uint8 f_meter_type;
	uint8 f_item_index;
	char f_col_name[LENGTH_F_COL_NAME];
	char f_col_type[LENGTH_F_COL_TYPE];
	pRequest_data pNext;
	pRequest_data pPrev;
};

/********************************************************************************
 ** �Ǳ���ʷ������
 ********************************************************************************/
#define FIELD_HIS_ID			"f_id"//��������
#define FIELD_HIS_ADDRESS		"f_meter_address"//���ַ
#define FIELD_HIS_MTYPE		"f_meter_type"//������
#define FIELD_HIS_DEVID		"f_device_id"//�豸���
#define FIELD_HIS_TSTAMP		"f_timestamp"//ʱ���
#define FIELD_HIS_TNODE		"f_time"//����ʱ���

struct meter_item;
typedef struct meter_item *pMeter_item;
typedef pMeter_item meter_item_list;
struct meter_item{
    uint8 item_index;//����������
    char field_value[LENGTH_F_VALUE];//��ʷ����
    char field_name[LENGTH_F_COL_NAME];//��ʷ���ݶ�Ӧ������
    pMeter_item pNext;
    pMeter_item pPrev;
};

struct his_data_str;
typedef struct his_data_str *pHis_data;
typedef pHis_data his_data_list;

struct his_data_str{//��ʷ����һ�����ݵĸ�ʽ, ÿ��������ĳ��ȶ�Ҫ��Լ���������ٴ�1, ���ڴ洢strcpy��ӵ�'\0'
    char   f_id[LENGTH_F_ID];//����ֵ
    char   f_device_id[5];//�Ǳ���豸���, �������п������"NULL", ������Ϊ5���ַ�
    char   f_meter_type[5];//�Ǳ�����
    char   f_meter_address[LENGTH_F_METER_ADDRESS];//�Ǳ��ַ
    char   f_timestamp[LENGTH_F_TIMESTAMP];//ʱ���
    char   f_time[LENGTH_F_TIME];//����ʱ���
    int  value_cnt;//�����������
    //ʹ������, ÿ�ο��ٹ̶���С(value_cnt, ���Դ�get_request_data_cnt()��ȡ)�Ŀռ�
    //������ÿ�β���һ������, ����������һ������, �ȽϷ�ʱ
    pMeter_item value_list;//����������, ��Ҫ��̬����, 
    pHis_data   pNext;//�¸�Ԫ��
    pHis_data   pPrev;//�ϸ�Ԫ��
};

/********************************************************************************
 **	 t_time_node
 ** ʱ������ñ�
 ********************************************************************************/
 struct time_node_str;
 typedef struct time_node_str *pTime_node;
 typedef pTime_node time_node_list;
 struct time_node_str{
	char f_time_name[LENGTH_F_TIME_NAME];
	char f_time_node[LENGTH_F_TIME_NODE];
	int f_id;
    pTime_node pNext;
    pTime_node pPrev;
};



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
void get_insert_sql(char *table_name, char **cols, int col_count, char **values,char *sql, int add_quote);
void get_update_sql(char *table_name, char **sets, int set_count, char **condition, int con_count, char *sql);
void get_delete_sql(char *table_name, char **condition, int con_count, char *sql);
/**********************
 ** ϵͳ���ò������ **
 **********************/
void read_sys_config(char *pErr);//�����ݿ��ȡ��������
uint8 get_sys_config(sys_config_idx idx, pSys_config pConfig);//��ȡ����һ������
int get_sys_config_cnt();//��ȡϵͳ���ò����ĸ���
uint8 insert_sysconf(pSys_config pConf);//���뵥��ϵͳ����
uint8 empty_sysconf_list();//��������б�
uint8 set_sysconf(char* pErr);//����ϵͳ����
uint8 add_one_config(pSys_config pConf, char* pErr);
void get_sys_config_name(sys_config_idx idx, char* config_name);

/**********************
 ** �Ǳ��ַ��Ϣ��� **
 **********************/
void read_meter_info(char	*pErr);//�����ݿ��ȡ�Ǳ��ַ��Ϣ
void retrieve_meter_info_list(int (*read_one_meter)(pMeter_info));//˳������Ǳ��ַ��Ϣ
int  get_meter_info_cnt();//��ȡ�Ǳ��ַ��Ϣ�ĸ���
void empty_meter_info_list();
uint8 empty_meter_info_table(char*);
uint8 insert_one_meter_info(pMeter_info);
uint8 insert_into_meter_info_table(char* pErr);

/**********************
 ** ��ȡ��������� **
 **********************/
void read_all_request_data(char	*pErr);
void read_request_data(char	*pErr, mtype_idx type_idx);//�����Ǳ����Ͷ�ȡ������
void retrieve_request_data_list(int (*read_one_item)(pRequest_data, void*), mtype_idx type_idx, void* pVar);//˳�������������Ϣ
int  get_request_data_cnt(mtype_idx);//��ȡ�Ǳ�������ĸ���
uint8 insert_one_request_node(pRequest_data pRqData);
uint8 set_request_data(char* pErr);
int get_request_data_setted();

/**********************
 ** �Ǳ���ʷ������� **
 **********************/
void insert_his_data(MeterFileType *pmf, void *pData, struct tm *pNowTime,struct tm *pTimeNode, char *pErr);
uint8 read_his_data(char* timenode, mtype_idx idx, char* pErr);
uint8 read_all_his_data(char* timenode, char* pErr);
uint8 empty_all_hisdata(void);
int get_his_cnt(mtype_idx idx);
uint8 retrieve_his_data(mtype_idx idx, int cnt, int (*read_one_his)(pHis_data, uint8), uint8 dev);
uint8 retrieve_and_del_his_data(mtype_idx idx, int cnt, int (*read_one_his)(pHis_data, uint8), uint8 dev);
uint8 asc_to_datestr(char* src, char* dest);



#endif  //_DB_H_
