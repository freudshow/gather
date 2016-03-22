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
#include <stdarg.h>

#include "readallmeters.h"
#include "read_heatmeter.h"

#include <sqlite3/sqlite3.h>

#include "db.h"

sqlite3 *g_pDB = NULL;//���ݿ�ָ��, ˽�б���, ֻ����ͨ�����ļ��ṩ�ĺ�������, �������ⲿ�������

/**********************
 ** ϵͳ������� **
 **********************/
static sys_config_str sys_config_array[SYS_CONFIG_COUNT];//���������б�, ˽�б���
static sys_config_list list_set_sysconf = NULL;//��λ������ϵͳ�������б�
static int set_sysconf_idx;//����ϵͳ������������
static int config_idx;//�������õĸ�������, ˽�б���
static int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name);

/**********************
 ** ��ȡ�Ǳ���Ϣ��� **
 **********************/
static meter_info_List list_meter_info = NULL;//�Ǳ���Ϣ�б�, ˽�б���
static int meter_info_idx;//���������ص��Ǳ�����������, ˽�б���
static int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name);
static void empty_meter_info_list();
/**********************
 ** ��ȡ��������� **
 **********************/

//u8Meter_type��aItems_list������˳���ܸ���, ����enum meter_type_idx�涨��˳��Ϊ׼
static uint8 u8Meter_type[] = {HEATMETER, WATERMETER, ELECTMETER, GASMETER};
static request_data_list arrayRequest_list[MTYPE_CNT]={0};//�Ǳ���Ϣ�б������, ˽�б���

static int request_data_idx[MTYPE_CNT]={0};//��������������������, ˽�б���
static int each_request_data(void *type_idx, int f_cnt, char **f_value, char **f_name);
static void empty_request_data_list(enum meter_type_idx type_idx);
/**********************
 ** �Ǳ���ʷ������� **
 **********************/
static char* tablename_array[] = {TABLE_HEAT, TABLE_WATER, TABLE_ELEC, TABLE_GAS};//����˳��ͬu8Meter_type��arrayRequest_list


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
 ** ��������	: �Ǳ���ʷ������� 
 ********************************************************************************/
void insert_his_data(MeterFileType *pmf, void *pData, struct tm *pNowTime,struct tm *pTimeNode, char *pErr)
{
	if(NULL == g_pDB) {
		pErr = "database not open";
		return;
	}
	enum meter_type_idx type_idx;

	switch(pmf->u8MeterType){
		case HEATMETER:
			type_idx = em_heat;
			break;
		case WATERMETER:
			type_idx = em_water;
			break;
		case ELECTMETER:
			type_idx = em_elect;
			break;
		case GASMETER:
			type_idx = em_gas;
			break;
		default:
			pErr = "meter type error";
			return;
	}
	
	char *sql_buf = malloc(LENGTH_SQLBUF);
	memset(sql_buf, 0, LENGTH_SQLBUF);	
	char *table_name = tablename_array[type_idx];
	int item_cnt = get_request_data_cnt(type_idx);
	char *col_buf = malloc(item_cnt*LENGTH_F_COL_NAME);
	memset(col_buf, 0, item_cnt*LENGTH_F_COL_NAME);
	char *tmp_col_buf = col_buf;
	request_data_list item_list = arrayRequest_list[type_idx];
	char *tmp_data = malloc(LENGTH_F_VALUE);
	memset(tmp_data, 0, LENGTH_F_VALUE);
	
	int i = 0;
	while(item_list) {
		strcpy(tmp_col_buf, item_list->f_col_name);
		tmp_col_buf += LENGTH_F_COL_NAME;
		item_list = item_list->pNext;
	}
	tmp_col_buf = col_buf;//ָ���һ��Ԫ��
	//insert into
	strcpy(sql_buf, SQL_INSERT);
	strcat(sql_buf, " ");
	strcat(sql_buf, table_name);
	strcat(sql_buf, SQL_LEFT_PARENTHESIS);
	//columns
	//�̶���
	strcat(sql_buf, FIELD_HIS_ADDRESS);//���ַ
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_TYPE);//������
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_DEVID);//�豸���
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_TSTAMP);//ʱ���
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_TNODE);//����ʱ���
	strcat(sql_buf, ",");
	for(i=0;i<item_cnt-1;i++, tmp_col_buf += LENGTH_F_COL_NAME) {
		printf("tmp_col_buf: %s\n", tmp_col_buf);
		strcat(sql_buf, tmp_col_buf);
		strcat(sql_buf, ",");
	}
	printf("tmp_col_buf: %s\n", tmp_col_buf);
	strcat(sql_buf, tmp_col_buf);
	strcat(sql_buf, SQL_RIGHT_PARENTHESIS);
	tmp_col_buf = col_buf;//ָ���һ��Ԫ��
	
	//values
	strcat(sql_buf, SQL_VALUES);
	strcat(sql_buf, SQL_LEFT_PARENTHESIS);
	//�̶���
	sprintf(tmp_data, "%02x%02x%02x%02x%02x%02x%02x", pmf->u8MeterAddr[6], \
	pmf->u8MeterAddr[5], pmf->u8MeterAddr[4], pmf->u8MeterAddr[3], 
	pmf->u8MeterAddr[2], pmf->u8MeterAddr[1], pmf->u8MeterAddr[0]);
	strcat(sql_buf, tmp_data);//���ַ
	strcat(sql_buf, ",");
	sprintf(tmp_data, "%02x",type_idx);
	strcat(sql_buf, tmp_data);//������
	strcat(sql_buf, ",");
	sprintf(tmp_data, "%s", "NULL");
	strcat(sql_buf, tmp_data);//�豸���
	strcat(sql_buf, ",");
	sprintf(tmp_data, "%s", asctime(pNowTime));
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, tmp_data);//ʱ���
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, ",");
	sprintf(tmp_data, "%s", asctime(pTimeNode));
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, tmp_data);//����ʱ���
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, ",");
	item_list = arrayRequest_list[type_idx];
	CJ188_Format* heatdata;
	uint8 *p;//�ı�CJ188_Format��uint32���͵��ֽ���
	switch(type_idx) {
	case em_heat:
			heatdata = (CJ188_Format*)pData;
			while(item_list) {//item_list->f_item_index��˳���item_list->f_col_name��˳����һ�µ�, ���ص���valueֵ˳��Ļ���
				memset(tmp_data, 0, LENGTH_F_COL_NAME);//ʹ��֮ǰ��0
				switch(item_list->f_item_index) {
				case HITEM_CUR_COLD_E:
					p=(uint8 *)&(heatdata->DailyHeat);
					sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), *(p+3), heatdata->DailyHeatUnit);//ʵ��Ϊ����, �����ǽ���������
					break;
				case HITEM_CUR_HEAT_E:
					p=(uint8 *)&(heatdata->CurrentHeat);
					sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), *(p+3), heatdata->CurrentHeatUnit);
					break;
				case HITEM_HEAT_POWER:
					p=(uint8 *)&(heatdata->HeatPower);
					sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), *(p+3), heatdata->HeatPowerUnit);
					break;
				case HITEM_FLOWRATE:
					p=(uint8 *)&(heatdata->Flow);
					sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), *(p+3), heatdata->FlowUnit);
					break;
				case HITEM_ACCUM_FLOW:
					p=(uint8 *)&(heatdata->AccumulateFlow);
					sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), *(p+3), heatdata->AccumulateFlowUnit);
					break;
				case HITEM_IN_TEMP:
					sprintf(tmp_data, "%02x%02x%02x", heatdata->WaterInTemp[0], \
						heatdata->WaterInTemp[1], heatdata->WaterInTemp[2]);
					break;
				case HITEM_OUT_TEMP:
					sprintf(tmp_data, "%02x%02x%02x", heatdata->WaterOutTemp[0], \
						heatdata->WaterOutTemp[1], heatdata->WaterOutTemp[2]);
					break;
				case HITEM_ACCUM_WORK_TIME:
					sprintf(tmp_data, "%02x%02x%02x", heatdata->AccumulateWorkTime[0], \
						heatdata->AccumulateWorkTime[1], heatdata->AccumulateWorkTime[2]);
					break;
				case HITEM_REAL_TIME:					
					sprintf(tmp_data, "%02x%02x%02x%02x%02x%02x%02x", heatdata->RealTime[0], \
						heatdata->RealTime[1], heatdata->RealTime[2], \
						heatdata->RealTime[3], heatdata->RealTime[4], 
						heatdata->RealTime[5], heatdata->RealTime[6]);
					break;
				case HITEM_STATE:
					sprintf(tmp_data, "%04x", heatdata->ST);
					break;
				default:
					sprintf(tmp_data, "Err");
					break;
				}
				strcat(sql_buf, SQL_SINGLE_QUOTES);
				strcat(sql_buf, tmp_data);
				strcat(sql_buf, SQL_SINGLE_QUOTES);
				if (item_list->pNext)//������ǵ�����һ��, ���ں���Ӷ���, ���򲻼�
					strcat(sql_buf, ",");
				item_list = item_list->pNext;
			}
		break;
	case em_water:
		break;
	case em_elect:
		break;
	case em_gas:
		break;
	default:
		break;
	}

	strcat(sql_buf, SQL_RIGHT_PARENTHESIS);
	strcat(sql_buf, ";");
	
	sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);
	printf("sql_buf: %s, sql_buf length: %d, pErr: %s\n", sql_buf, strlen(sql_buf), pErr);
	free(col_buf);
	free(sql_buf);
	
}

/********************************************************************************
 ** ��������	: ��ȡ������
 ********************************************************************************/
void read_all_request_data(char	*pErr)
{
	int i;
	for(i=0;i<MTYPE_CNT;i++)
		read_request_data(pErr, i);
}

void read_request_data(char *pErr, enum meter_type_idx type_idx)
{
	if(NULL == g_pDB) {
		pErr = "database not open";
		return;
	}

	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *where_buf = malloc(LENGTH_SQLCON);
	char *order_buf = malloc(LENGTH_SQLORDER);
	char *m_type = malloc(LENGTH_F_METER_TYPE);
	//char *con_buf[LENGTH_SQLCONS] = {FIELD_REQUEST_MTYPE};//������ʼ����, con_buf[0]ָ��const char*, �����ٽ�������, copy�Ȳ���
	char *con_buf = malloc(LENGTH_SQLCONS);
	char *table_name = TABLE_REQUEST_DATA;
	char *col_buf[LENGTH_F_COL_NAME] =  {FIELD_REQUEST_ID, \
				FIELD_REQUEST_MTYPE, FIELD_REQUEST_ITEMIDX, FIELD_REQUEST_COLNAME,\
				FIELD_REQUEST_COLTYPE};
	int  col_cnt = 5;

	request_data_idx[type_idx] = 0;
	sprintf(m_type, "%2x", u8Meter_type[type_idx]);
	strcpy(con_buf, FIELD_REQUEST_MTYPE);
	strcat(con_buf, SQL_EQUAL);
	strcat(con_buf, m_type);

	empty_request_data_list(type_idx);//�����ǰ����Ϣ, �����¶�ȡ
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_where_sql(&con_buf, 1, where_buf);
	get_orderby_sql(&col_buf[2], 1, 1, order_buf);//�������õ������Ա����Ƚ����, ���Խ����ȡ, ����洢
	strcat(sql_buf, " ");
	strcat(sql_buf, where_buf);
	strcat(sql_buf,	" ");
	strcat(sql_buf, order_buf);
	strcat(sql_buf, ";");

	sqlite3_exec(g_pDB, sql_buf, each_request_data, (void*)(&type_idx), &pErr);
	free(m_type);
	free(order_buf);
	free(sql_buf);
	free(where_buf);
}

static int each_request_data(void *meter_type_idx, int f_cnt, char **f_value, char **f_name)
{
	int i;
	int idx = *((int*)meter_type_idx);
	pRequest_data tmp_request = malloc(sizeof(struct request_data_str));
	memset(tmp_request, 0, sizeof(struct request_data_str));
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_REQUEST_ID))//������ID
			tmp_request->f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_MTYPE)){//�Ǳ�����(Hex String)
			tmp_request->f_meter_type = (Ascii2Hex(f_value[i][0])<<LEN_HALF_BYTE | Ascii2Hex(f_value[i][1]));
			printf("meter type after convert: %02x\n", tmp_request->f_meter_type);
		}
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_ITEMIDX)) {//�Ǳ��������������(Dec String)
			tmp_request->f_item_index = atoi(f_value[i]);
		}
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_COLNAME))//�Ǳ������������(Dec String)
			strcpy(tmp_request->f_col_name, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_COLTYPE))//����������
			strcpy(tmp_request->f_col_type, f_value[i]);
		else {//�쳣���
			
		}
	}
	tmp_request->pPrev = NULL;
	tmp_request->pNext = arrayRequest_list[idx];
	if (arrayRequest_list[idx]) {
		arrayRequest_list[idx]->pPrev = tmp_request;
	}
	arrayRequest_list[idx] = tmp_request;
	request_data_idx[idx]++;

	return 0;
}


void retrieve_request_data_list(int (*read_one_item)(pRequest_data), enum meter_type_idx type_idx)
{
	if(!read_one_item)
		return;

	pRequest_data rt_request  = malloc(sizeof(struct request_data_str));
	pRequest_data tmp_request = arrayRequest_list[type_idx];
	while(tmp_request) {
		memcpy(rt_request, tmp_request, sizeof(struct request_data_str));
		rt_request->pPrev = NULL;
		rt_request->pNext = NULL;
		read_one_item(rt_request);
		tmp_request = tmp_request->pNext;
	}
	free(rt_request);
}

static void empty_request_data_list(enum meter_type_idx type_idx)
{
	pRequest_data tmp_request;
	while(arrayRequest_list[type_idx]) {
		tmp_request = arrayRequest_list[type_idx];
		arrayRequest_list[type_idx] = arrayRequest_list[type_idx]->pNext;
		free(tmp_request);
	}
}

int  get_request_data_cnt(enum meter_type_idx idx)
{
	return request_data_idx[idx];
}

/********************************************************************************
 **	 ������: read_meter_info
 ** ����	: �����ݿ��ж�ȡ�Ǳ��ַ��Ϣ, �ŵ�list_meter_info��
 ** ��Ҫ  : �������ʱ����list_meter_infoΪNULL, ���򱾳������ֶδ���
 ********************************************************************************/

void read_meter_info(char	*pErr)
{
	if(NULL == g_pDB) {
		pErr = "database not open";
		return;
	}

	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *order_buf = malloc(LENGTH_SQLORDER);
	char *table_name = TABLE_METER_INFO;
	char *col_buf[LENGTH_F_COL_NAME] =  {FIELD_MINFO_ID, \
				FIELD_MINFO_ADDRESS, FIELD_MINFO_TYPE, FIELD_MINFO_CHANNEL,\
				FIELD_MINFO_POS, FIELD_MINFO_DEVICE_ID, FIELD_MINFO_PROTO_TYPE};
	int  col_cnt = 7;
	
	meter_info_idx = 0;
	empty_meter_info_list();//�����ǰ����Ϣ, �����¶�ȡ
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(&col_buf[3], 1, 1, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	strcat(sql_buf, ";");
	sqlite3_exec(g_pDB, sql_buf, each_meter_info, NULL, &pErr);
	free(sql_buf);
	free(order_buf);
}

static int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
	int i, j;//i, һ����¼���ֶ�������; j���ַ
	int length;//�����ݱ��ж�ȡ�ı��ַ�ַ����ĳ���
	int low_idx;//�����ݱ��ж�ȡ�ı��ַ�ַ���, ��λ(������)�±�
	pMeter_info tmp_info = malloc(sizeof(struct meter_info_str));
	memset(tmp_info, 0, sizeof(struct meter_info_str));
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_MINFO_ID))//�Ǳ�ID
			tmp_info->f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_ADDRESS)) {//�Ǳ��ַ(BCD String)
			length = strlen(f_value[i]);
			for (j=0; j<(length+1)/BYTE_BCD_CNT;j++) {
				low_idx = length-BYTE_BCD_CNT*j-2;
				/*tmp_info->f_meter_address[LENGTH_B_METER_ADDRESS-1-j] = \
					(((low_idx < 0) ? 0: (f_value[i][low_idx]-ZERO_CHAR)) << LEN_HALF_BYTE | (f_value[i][low_idx+1]-ZERO_CHAR));*/
				tmp_info->f_meter_address[j] = \
					(((low_idx < 0) ? 0: Ascii2Hex(f_value[i][low_idx])) << LEN_HALF_BYTE | Ascii2Hex(f_value[i][low_idx+1]));
			}
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_TYPE)) {//�Ǳ����ͱ���(HEX String), �̶�Ϊ�����ַ�
			if (strlen(f_value[i]) == BYTE_BCD_CNT) {
				tmp_info->f_meter_type = (Ascii2Hex(f_value[i][0]) << LEN_HALF_BYTE | Ascii2Hex(f_value[i][1]));
			}
			else {//�쳣���
			}
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_CHANNEL))//�Ǳ�ͨ��(Dec String)
			tmp_info->f_meter_channel = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_POS))//�Ǳ�װλ��
			strcpy(tmp_info->f_install_pos, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_DEVICE_ID))//�Ǳ���豸���(Dec String)
			tmp_info->f_device_id = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_PROTO_TYPE))//�Ǳ��Э�����(Dec String)
			tmp_info->f_meter_proto_type = atoi(f_value[i]);
		else {//�쳣���

		}
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

int get_meter_info_cnt()
{
	return meter_info_idx;
}

//����Ǳ���Ϣ, �Ա����¶�ȡ
//�˺������еĻ�����, list�����ȳ�ʼ��ΪNULL
static void empty_meter_info_list()
{
	pMeter_info tmp_info;
	while(list_meter_info) {
		tmp_info = list_meter_info;
		list_meter_info = list_meter_info->pNext;
		free(tmp_info);
	}
}

//�����Ǳ���Ϣ, ��ÿһ�������read_one_meter����
void retrieve_meter_info_list(int (*read_one_meter)(pMeter_info))
{
	if(!read_one_meter)
		return;
	
	pMeter_info pInfo_return = malloc(sizeof(struct meter_info_str));//�ش�һ�������Ľṹ��, ��֤ԭʼ���ݵİ�ȫ
	pMeter_info pInfo = list_meter_info;

	while(pInfo) {
		memcpy(pInfo_return, pInfo, sizeof(struct meter_info_str));
		pInfo_return->pNext = NULL;
		pInfo_return->pPrev = NULL;
		read_one_meter(pInfo_return);
		pInfo = pInfo->pNext;
	}
	free(pInfo_return);
}



/********************************************************************************
 **	 ������: read_sys_config
 ** ����	: �����ݿ��ж�ȡ��������, �ŵ�sys_config_array��
 ********************************************************************************/
void read_sys_config(char *pErr)
{
	if(NULL == g_pDB) {
		pErr = "database not open";
		return;
	}

	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *order_buf = malloc(LENGTH_SQLORDER);
	char *table_name = TABLE_BASE_DEF;
	char *col_buf[LENGTH_F_COL_NAME] = {FIELD_BASE_DEF_ID, FIELD_BASE_DEF_NAME, FIELD_BASE_DEF_VALUE};
	int	col_cnt = 3;

	memset(sys_config_array, 0, SYS_CONFIG_COUNT*sizeof(sys_config_str));//���ԭ������
	config_idx = 0;

	//��ȡ���ݱ��ڵ�����
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(col_buf, 1, 0, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	sqlite3_exec(g_pDB, sql_buf, each_config, NULL, &pErr);
	free(sql_buf);
	free(order_buf);
}

static int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name)
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

int get_sys_config_cnt()
{
	return config_idx;
}

uint8 get_sys_config(enum T_System_Config idx, pSys_config pConfig)
{
	if(NULL==pConfig || idx<0 || idx>=SYS_CONFIG_COUNT)
		return ERR_1;
	
	memcpy((uint8*)pConfig, (uint8*)&sys_config_array[idx], sizeof(sys_config_str));
	return NO_ERR;
}

static uint8 del_sysconf(char* pId, char* pErr)
{
	int err=0;
	char *sql_buf = malloc(LENGTH_SQLBUF);
	strcpy(sql_buf, SQL_DELETE);
	strcat(sql_buf, " ");
	strcat(sql_buf, SQL_FROM);
	strcat(sql_buf, " ");
	strcat(sql_buf, TABLE_BASE_DEF);

	if(NULL != pId) {
		strcat(sql_buf, " ");
		strcat(sql_buf, SQL_WHERE);
		strcat(sql_buf, " ");
		strcat(sql_buf, FIELD_BASE_DEF_ID);
		strcat(sql_buf, SQL_EQUAL);
		strcat(sql_buf, pId);
	}
	err = sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);
	free(sql_buf);
	return err==SQLITE_OK ? NO_ERR:ERR_FF;
}

/*
 * ���ϵͳ������Ϣ
 * ע��: �˺���Ӧ���ɵ����������б��������ǰ����һ��,
 * ���򽫳��ִ���(�罫��ǰ����Ϣ���뵽���ݱ���, ���߳���������ͻ)
 */
uint8 empty_sysconf_list()
{
	pSys_config tmp_info;
	while(list_set_sysconf) {
		tmp_info = list_set_sysconf;
		list_set_sysconf = list_set_sysconf->pNext;
		free(tmp_info);
	}
	set_sysconf_idx = 0;
	return NO_ERR;
}

uint8 insert_sysconf(pSys_config pConf)
{
	pSys_config pConfig = malloc(sizeof(sys_config_str));//Ϊ�˰�ȫ���, ��ֱ��ʹ�ô��ݽ����Ĳ���
	if(NULL != pConf) {
		memcpy(pConfig, pConf, sizeof(sys_config_str));
	}
	else {
		return ERR_1;
	}
	
	pConfig->pPrev = NULL;
	pConfig->pNext = list_set_sysconf;
	if (list_set_sysconf) {
		list_set_sysconf->pPrev = pConfig;
	}
	list_set_sysconf = pConfig;
	set_sysconf_idx++;
	return NO_ERR;
}

uint8 add_one_config(pSys_config pConf, char* pErr)
{
	if(NULL == pConf) {
		return ERR_1;
	}
	uint8 err = NO_ERR;
	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *table_name = TABLE_BASE_DEF;
	char *col_buf[LENGTH_F_COL_NAME] = {FIELD_BASE_DEF_ID, FIELD_BASE_DEF_NAME, FIELD_BASE_DEF_VALUE};
	char pIdstr[LENGTH_F_CONFIG_VALUE];
	sprintf(pIdstr, "%d", pConf->f_id);
	char *val_buf[LENGTH_F_CONFIG_VALUE] = {pIdstr, pConf->f_config_name, pConf->f_config_value};
	
	get_insert_sql(table_name, col_buf, 3, val_buf, sql_buf, 1);
	err =  sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);
	free(sql_buf);	
	return err==SQLITE_OK ? NO_ERR : ERR_1;
}

uint8 set_sysconf(char* pErr)
{
	pSys_config pTmp_conf = list_set_sysconf;
	del_sysconf(NULL, pErr);//�����ԭ�еı�����
	while(pTmp_conf) {
		if(NO_ERR != add_one_config(pTmp_conf, pErr)) {
			return ERR_1;
		}
		pTmp_conf = pTmp_conf->pNext;
	}
	empty_sysconf_list();//��������б�

	
	//���¶�ȡ���ݿ������Ϣ
	read_sys_config(pErr);
	return (strlen(pErr) ? ERR_1 : NO_ERR );
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
void get_insert_sql(char *table_name, char **cols, int col_cnt, char **values,char *sql, int add_quote)
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
		if(add_quote){
			strcat(sql, SQL_SINGLE_QUOTES);
		}
		strcat(sql, values[i]);
		if(add_quote){
			strcat(sql, SQL_SINGLE_QUOTES);
		}
		strcat(sql, ", ");
	}
	if(add_quote){
		strcat(sql, SQL_SINGLE_QUOTES);
	}
	strcat(sql, values[i]);
	if(add_quote){
		strcat(sql, SQL_SINGLE_QUOTES);
	}
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
