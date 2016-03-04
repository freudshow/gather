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

/********************************************************************************
 ** t_base_define
 ** ����������Ϣ��, ��t_base_define.f_id��������Ӧ������
 ********************************************************************************/
#define SYS_CONFIG_COUNT	14//���������������

enum T_System_Config {
	CONFIG_PRIMARY_SERVER 	= 0,
	CONFIG_PRIMARY_DNS 		= 1,
	CONFIG_PRIMARY_PORT 	= 2,
	CONFIG_SECOND_SERVER 	= 3,
	CONFIG_SECOND_DNS 		= 4,
	CONFIG_SECOND_PORT 		= 5,
	CONFIG_GATEWAY_ID 		= 6,
	CONFIG_NET_TYPE 			= 7,
	CONFIG_MD5_KEY 			= 8,
	CONFIG_AES_KEY 			= 9,
	CONFIG_COLLECT_MODE 	= 10,
	CONFIG_COLLECT_CYCLE 	= 12,
	CONFIG_REPORT_MODE 		= 13,
	CONFIG_BEAT_CYCLE 		= 14
};

typedef struct {
	char f_config_name[LENGTH_F_CONFIG_NAME];
	char f_config_value[LENGTH_F_CONFIG_VALUE];
	int f_id;
} sys_config_str;
typedef sys_config_str *pSys_config;
typedef pSys_config sys_config_list;
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
typedef pMeter_address meter_address_list;

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
typedef pRequest_data request_data_list;

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
typedef pTime_node time_node_list;

extern sys_config_str sys_config_array[SYS_CONFIG_COUNT];

extern void init_sys_config();

#endif  //_DB_H_
