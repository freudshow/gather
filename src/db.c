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
  * ��ģ���ڵĺ������������̰߳�ȫ��, ʹ��֮ǰ���ÿ����������ź�����ƥ��ͼ��,
  * ���߷���xmlģ��, ��ÿһ���豸������һ����Ӧ�����ݿ����, �������̶߳�ͬһ��
  * ���ݿ������ж�д(δʵ��).
  *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "readallmeters.h"
#include "read_heatmeter.h"

#include <sqlite3/sqlite3.h>

#include "db.h"

sqlite3 *g_pDB = NULL;//���ݿ�ָ��, ˽�б���

/**********************
 ** ϵͳ������� **
 **********************/
static sys_config_str sys_config_array[SYS_CONFIG_COUNT];//���������б�, ˽�б���
static sys_config_list list_set_sysconf = NULL;//��λ������ϵͳ�������б�, ˽�б���
static int set_sysconf_idx;//����ϵͳ������������, ˽�б���
static int config_idx;//�������õĸ�������, ˽�б���
static int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name);

/**********************
 ** ��ȡ�Ǳ���Ϣ��� **
 **********************/
static meter_info_List list_meter_info = NULL;//�Ǳ���Ϣ�б�, ˽�б���
static int meter_info_idx;//���������ص��Ǳ�����������, ˽�б���
static int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name);

/**********************
 ** ��ȡ��������� **
 **********************/

//u8Meter_type��aItems_list������˳���ܸ���, ����enum meter_type_idx�涨��˳��Ϊ׼
static uint8 u8Meter_type[] = {HEATMETER, WATERMETER, ELECTMETER, GASMETER, SENSORDEV};
static request_data_list arrayRequest_list[MTYPE_CNT]={NULL};//�Ǳ���Ϣ�б������, ˽�б���
static request_data_list list_set_request_data = NULL;
static int set_request_data_idx=0;

static int request_data_idx[MTYPE_CNT]={0};//��������������������, ˽�б���
static int each_request_data(void *type_idx, int f_cnt, char **f_value, char **f_name);
static void empty_request_data_list(enum meter_type_idx type_idx);
/**********************
 ** �Ǳ���ʷ������� **
 **********************/
his_data_list his_data_list_array[MTYPE_CNT] = {NULL};
int hisdata_idx_array[MTYPE_CNT];

char his_sql_array[MTYPE_CNT][LENGTH_SQLBUF];
static uint8 data_item_idx;//��������±��, ����ָ����λ, �����̰߳�ȫ��

//����˳��ͬenum meter_type_idx
static char* tablename_array[] = {TABLE_HEAT, TABLE_WATER, TABLE_ELEC, TABLE_GAS};


/********************************************************************************
 **	 ������: open_db
 ** ����	: �����ݿ�
 ********************************************************************************/
uint8 open_db(void)
{
	 return (sqlite3_open(SQLITE_NAME, &g_pDB) == SQLITE_OK) ? NO_ERR: ERR_1;
}

/********************************************************************************
 **	 ������: close_db
 ** ����	: �ر����ݿ�
 ********************************************************************************/
uint8 close_db(void)
{
	 return (sqlite3_close(g_pDB) == SQLITE_OK) ? NO_ERR: ERR_1;
}


/********************************************************************************
 ** ��������	: �Ǳ���ʷ������� 
 ********************************************************************************/


void get_heatdata_sql(CJ188_Format* heatdata, request_data_list item_list, char* sql_buf)
{
    char tmp_data[LENGTH_F_COL_NAME];
    uint8 *p;
    while(item_list) {//item_list->f_item_index��˳���item_list->f_col_name��˳����һ�µ�, ���ص���valueֵ˳��Ļ���
        memset(tmp_data, 0, LENGTH_F_COL_NAME);//ʹ��֮ǰ��0
        switch(item_list->f_item_index) {
        case em_HColdE:
            p=(uint8 *)&(heatdata->DailyHeat);
            sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), \
            *(p+3), heatdata->DailyHeatUnit);//ʵ��Ϊ����, �����ǽ���������
            break;
        case em_HHeatE:
            p=(uint8 *)&(heatdata->CurrentHeat);
            sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), \
            *(p+3), heatdata->CurrentHeatUnit);
            break;
        case em_HPower:
            p=(uint8 *)&(heatdata->HeatPower);
            sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), \
            *(p+3), heatdata->HeatPowerUnit);
            break;
        case em_HFlowrate:
            p=(uint8 *)&(heatdata->Flow);
            sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), \
            *(p+3), heatdata->FlowUnit);
            break;
        case em_HAccumFlow:
            p=(uint8 *)&(heatdata->AccumulateFlow);
            sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), \
            *(p+3), heatdata->AccumulateFlowUnit);
            break;
        case em_HInTemp:
            sprintf(tmp_data, "%02x%02x%02x", heatdata->WaterInTemp[0], \
            heatdata->WaterInTemp[1], heatdata->WaterInTemp[2]);
            break;
        case em_HOutTemp:
            sprintf(tmp_data, "%02x%02x%02x", heatdata->WaterOutTemp[0], \
            heatdata->WaterOutTemp[1], heatdata->WaterOutTemp[2]);
            break;
        case em_HAccumWorkTime:
            sprintf(tmp_data, "%02x%02x%02x", heatdata->AccumulateWorkTime[0], \
            heatdata->AccumulateWorkTime[1], heatdata->AccumulateWorkTime[2]);
            break;
        case em_HCurTime:					
            sprintf(tmp_data, "%02x%02x%02x%02x%02x%02x%02x", heatdata->RealTime[0], \
            heatdata->RealTime[1], heatdata->RealTime[2], \
            heatdata->RealTime[3], heatdata->RealTime[4], 
            heatdata->RealTime[5], heatdata->RealTime[6]);
            break;
        case em_HState:
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
}

void get_elecdata_sql(lcModbusElec_str* elecdata, request_data_list item_list, char* sql_buf)
{
    char tmp_data[LENGTH_F_COL_NAME];
    while(item_list) {//item_list->f_item_index��˳���item_list->f_col_name��˳����һ�µ�, ���ص���valueֵ˳��Ļ���
        memset(tmp_data, 0, LENGTH_F_COL_NAME);//ʹ��֮ǰ��0
        switch(item_list->f_item_index) {
        case em_EPActTotElec:
            sprintf(tmp_data, "%f %s", elecdata->pact_tot_elec,\
                (char*)elecdata->pact_tot_elec_unit);
            break;
        case em_ENActTotElec:
            sprintf(tmp_data, "%f %s", elecdata->nact_tot_elec,\
                (char*)elecdata->nact_tot_elec_unit);
            break;
        case em_EPReactTotElec:
            sprintf(tmp_data, "%f %s", elecdata->preact_tot_elec,\
                (char*)elecdata->preact_tot_elec_unit);
            break;
        case em_ENReactTotElec:
            sprintf(tmp_data, "%f %s", elecdata->nreact_tot_elec,\
                (char*)elecdata->nreact_tot_elec_unit);
            break;
        case em_EActTotElec:
            sprintf(tmp_data, "%f %s", elecdata->act_tot_elec,\
                (char*)elecdata->act_tot_elec_unit);
            break;
        case em_EReactTotElec:
            sprintf(tmp_data, "%f %s", elecdata->react_tot_elec, \
                (char*)elecdata->react_tot_elec_unit);
            break;
        case em_EOvrPowFac:
            break;
        case em_EPMaxDem:
            break;
        default:
            sprintf(tmp_data, "not define");
            break;
        }
        strcat(sql_buf, SQL_SINGLE_QUOTES);
        strcat(sql_buf, tmp_data);
        strcat(sql_buf, SQL_SINGLE_QUOTES);
        if (item_list->pNext)//������ǵ�����һ��, ���ں���Ӷ���, ���򲻼�
        strcat(sql_buf, ",");
        item_list = item_list->pNext;
    }
}

/*
 * ���ڵ���vacuum�����ͷſռ�ʱ,
 * sqlite3Ҫ����һ����ʱ�����ݿ�"vacuum_db",
 * ������ʱ�����������ݿ��ڵ��ļ�.
 * �����ʱ�����ݿ��ļ���С�ܿ��������е�
 * ���ݿ��ļ���С�ӽ�.
 * ����ϵͳ�����п��пռ�Ĵ�С���ܱ����е�
 * ���ݿ��ļ��Ĵ�С��С.
 */
uint8 db_too_big()
{
    uint8 err = NO_ERR;
    FILE* fp;
    char log[1024];

    char* cmd_disk_idle = "df | grep rootfs | awk '{print $4}'";//���пռ�
    char* cmd_db_size = "ls -l gatherdb.db | awk '{print $5}'";//���ݿ��С
    char result[20];
    int disk_idle;
    int db_size;
    if(NULL==(fp=popen(cmd_disk_idle, "r"))) {
        sprintf(log, "[%s][%s][%d]execute command failed: %s", FILE_LINE, strerror(errno));
        write_log_file(log, strlen(log));
        return ERR_1;
    }
    if(fread(result, sizeof(char), sizeof(result), fp)) {
        disk_idle = atoi(result);
    } else {
        sprintf(log, "[%s][%s][%d]get result failed: %s", FILE_LINE, strerror(errno));
        write_log_file(log, strlen(log));
        return ERR_1;
    }

    pclose(fp);
    if(NULL==(fp=popen(cmd_db_size, "r"))) {
        sprintf(log, "[%s][%s][%d]execute command failed: %s", FILE_LINE, strerror(errno));
        write_log_file(log, strlen(log));
        return ERR_1;
    }
    if(fread(result, sizeof(char), sizeof(result), fp)) {
        db_size = atoi(result);
    } else {
        sprintf(log, "[%s][%s][%d]get result failed: %s", FILE_LINE, strerror(errno));
        write_log_file(log, strlen(log));
        return ERR_1;
    }
    pclose(fp);

    disk_idle *= 1024;//ϵͳ�Ŀ��С��1k
    if(disk_idle<db_size) {//���ϵͳ���пռ�С���������ݿ�Ĵ�С, ����Ϊ�ռ�ȽϽ�����
        sprintf(log, "[%s][%s][%d]disk_idle: %d, db_size: %d", \
            FILE_LINE, disk_idle, db_size);
        write_log_file(log, strlen(log));        
        return ERR_1;
    }
    return err;
}

/*
 * �������Ǳ�����ݱ���������һ������
 * ��ɾ�����ϴ��ɹ�����ʷ����
 */
uint8 clean_data(mtype_idx type_idx, char* pErr)
{
    uint8 err = NO_ERR;
    char log[512] = {0};
    char sql_buf[512]= {0};
    strcpy(sql_buf, "delete from ");
    strcat(sql_buf, tablename_array[type_idx]);
    strcat(sql_buf, " where f_upok = 1;");
    sprintf(log, "[%s][%s][%d]%s\n", FILE_LINE, sql_buf);
    write_log_file(log, strlen(log));
    if (db_too_big()) {/*check if database is too big*/
        err = sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);//ɾ������һ�������
        if(err != SQLITE_OK) {
            sprintf(log, "[%s][%s][%d]%s\n", FILE_LINE, pErr);
            write_log_file(log, strlen(log));
            return err;
        }
        err = sqlite3_exec(g_pDB, "vacuum;", NULL, NULL, &pErr);//�ͷſռ�
        if(err != SQLITE_OK) {
            sprintf(log, "[%s][%s][%d]%s\n", FILE_LINE, pErr);
            write_log_file(log, strlen(log));
            return err;
        }
    }
    return err;
}

mtype_idx idx_of_mtype(uint8 meterType)
{
    int i=0;
    for(i=0;i<METER_TYPE_CNT;i++) {
        if(meterType == u8Meter_type[i]) {
            return i;
        }
    }
    return -1;
}

uint8 insert_his_data(MeterFileType *pmf, void *pData, struct tm *pNowTime,struct tm *pTimeNode)
{
    char log[1024] = {0};
    uint8 err = NO_ERR;
    char *pErr;
    char sql_buf[LENGTH_SQLBUF] = {0};
    mtype_idx type_idx;
    char *table_name;
    int item_cnt;
    char *col_buf;
    char *tmp_col_buf;
    request_data_list item_list;
    char tmp_data[LENGTH_F_VALUE] = {0};
    int i = 0;

    if(NULL == g_pDB) {
        pErr = "database not open";
        sprintf(log, "[%s][%s][%d]%s", FILE_LINE, pErr);
        write_log_file(log, strlen(log));
        return ERR_1;
    }
    type_idx = idx_of_mtype(pmf->u8MeterType);
    err = clean_data(type_idx, pErr);
    if(err == ERR_1) {//���ɾ������ʧ��, �Ͳ�����������
        sprintf(log, "[%s][%s][%d]%s", FILE_LINE, pErr);
        write_log_file(log, strlen(log));
        return ERR_1;
    }
    table_name = tablename_array[type_idx];
    item_cnt = get_request_data_cnt(type_idx);
    item_list = arrayRequest_list[type_idx];
    col_buf = malloc(item_cnt*LENGTH_F_COL_NAME);
    memset(col_buf, 0, item_cnt*LENGTH_F_COL_NAME);
    tmp_col_buf = col_buf;

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
	strcat(sql_buf, FIELD_HIS_MTYPE);//������
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_DEVID);//�豸���
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_TSTAMP);//ʱ���
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_TNODE);//����ʱ���
	strcat(sql_buf, ",");
    strcat(sql_buf, FIELD_HIS_UPOK);
    strcat(sql_buf, ",");
	for(i=0;i<item_cnt-1;i++, tmp_col_buf += LENGTH_F_COL_NAME) {
		strcat(sql_buf, tmp_col_buf);
		strcat(sql_buf, ",");
	}
	strcat(sql_buf, tmp_col_buf);
	strcat(sql_buf, SQL_RIGHT_PARENTHESIS);
	tmp_col_buf = col_buf;//ָ���һ��Ԫ��
	
	//values
	strcat(sql_buf, SQL_VALUES);
	strcat(sql_buf, SQL_LEFT_PARENTHESIS);
	//�̶���
	sprintf(tmp_data, "'%02x%02x%02x%02x%02x%02x%02x'", pmf->u8MeterAddr[6], \
	pmf->u8MeterAddr[5], pmf->u8MeterAddr[4], pmf->u8MeterAddr[3], 
	pmf->u8MeterAddr[2], pmf->u8MeterAddr[1], pmf->u8MeterAddr[0]);
	strcat(sql_buf, tmp_data);//���ַ
	strcat(sql_buf, ",");
	sprintf(tmp_data, "'%02x'",pmf->u8MeterType);
	strcat(sql_buf, tmp_data);//������
	strcat(sql_buf, ",");
	sprintf(tmp_data, "'%d'", pmf->u16MeterID);
	strcat(sql_buf, tmp_data);//�豸���
	strcat(sql_buf, ",");

    //asctime_r��asctime���ع̶���ʽ'DDD MMM dd hh:mm:ss YYYY',���� �·� ���� ʱ�� ��
	asctime_r(pNowTime, tmp_data);//��sqlite3��ʱ�����ʽ������,��ת��Ϊ'YYYY-MM-DD hh:mm:ss'
    char dest[25]={0};
    asc_to_datestr(tmp_data, dest);
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, dest);//ʱ���
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, ",");
    
    asctime_r(pTimeNode, tmp_data);
    memset(dest, 0, 25);
    asc_to_datestr(tmp_data, dest);
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, dest);//����ʱ���
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, ",");
    strcat(sql_buf, "'0'");
    strcat(sql_buf, ",");
	item_list = arrayRequest_list[type_idx];
    if (NULL==pData) {//�������Լ��, ���������NULLֵ, ����Ϊ��ǰ�Ǳ�ĵ�ǰʱ������ʷ����û������
        while(item_list) {
            strcat(sql_buf, "'null'");
            if (item_list->pNext)//������ǵ�����һ��, ���ں���Ӷ���, ���򲻼�
                strcat(sql_buf, ",");

            item_list = item_list->pNext;
        }
    }
    else {
        switch(type_idx) {
        case em_heat:
            get_heatdata_sql(pData, item_list, sql_buf);
            break;
        case em_water:
            break;
        case em_elect:
            get_elecdata_sql(pData, item_list, sql_buf);
            break;
        case em_gas:
            break;
        default:
            break;
        }
    }
    strcat(sql_buf, SQL_RIGHT_PARENTHESIS);
    strcat(sql_buf, ";");

    err = sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);
    printf("[%s][%s][%d]sql_buf: %s, sql_buf length: %d, pErr: %s\n", FILE_LINE, \
        sql_buf, strlen(sql_buf), pErr);
    free(col_buf);
    write_err(err,pErr,log)
    return (err == SQLITE_OK) ? NO_ERR : ERR_1;
}

int read_one_item_info(pRequest_data pReques, void* pHisData)
{
    pMeter_item pItem = (pMeter_item) pHisData;
    strcpy(pItem[data_item_idx].field_name, pReques->f_col_name);//����
    pItem[data_item_idx].item_index = pReques->f_item_index;//����������
    data_item_idx++;
    return NO_ERR;
}

uint8 init_value_list(pMeter_item pHisData, int item_cnt, mtype_idx type_idx)
{
    data_item_idx = 0;//before read each data item, init index to 0
    retrieve_request_data_list(read_one_item_info, type_idx, (void*)pHisData);
    return NO_ERR;
}

int each_his_data(void *meter_type_idx, int f_cnt, char **f_value, char **f_name)
{
    int i, j;
    int idx = *((int*)meter_type_idx);

    pHis_data tmp_his = malloc(sizeof(struct his_data_str));
    memset(tmp_his, 0, sizeof(struct his_data_str));
    tmp_his->value_cnt = get_request_data_cnt(idx);
    tmp_his->value_list = malloc(tmp_his->value_cnt*sizeof(struct meter_item));
    memset(tmp_his->value_list, 0, tmp_his->value_cnt*sizeof(struct meter_item));
    init_value_list(tmp_his->value_list, tmp_his->value_cnt, *((mtype_idx*)meter_type_idx));
    for (i=0; i<f_cnt; i++) {
        if (0 == strcmp(f_name[i], FIELD_HIS_ID)){//������ID
            strcpy(tmp_his->f_id, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_MTYPE)){//�Ǳ�����(Hex String)
            strcpy(tmp_his->f_meter_type, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_DEVID)) {//�Ǳ��豸��ŵ�������(Dec String)
            strcpy(tmp_his->f_device_id, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_ADDRESS)){//���ַ(BCD String)
            strcpy(tmp_his->f_meter_address, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_TNODE)){//����ʱ���(String)
            strcpy(tmp_his->f_time, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_TSTAMP)){//ʱ���(String)
            strcpy(tmp_his->f_timestamp, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else {//������
            for(j=0;j<tmp_his->value_cnt;j++)
                if(0 == strcmp(tmp_his->value_list[j].field_name, f_name[i]))
                    strcpy(tmp_his->value_list[j].field_value, f_value[i]==NULL?"NULL":f_value[i]);
        }
    }
    add_node(his_data_list_array[idx], tmp_his)
    hisdata_idx_array[idx]++;

    return 0;
}

uint8 empty_hist_data(mtype_idx idx)
{
    pHis_data tmpNode;
    while(his_data_list_array[idx]) {
        tmpNode = his_data_list_array[idx];
        his_data_list_array[idx] = his_data_list_array[idx]->pNext;
        if(tmpNode->value_list)
            free(tmpNode->value_list);
        free(tmpNode);
    }
    hisdata_idx_array[idx] = 0;
    return NO_ERR;
}

uint8 empty_all_hisdata()
{
    int i;
    for(i=0;i<MTYPE_CNT;i++) {
        if(empty_hist_data(i))
            return ERR_1;
    }
    return NO_ERR;
}


uint8 get_his_sql(char* timenode, mtype_idx type_idx)
{
    strcpy(his_sql_array[type_idx], "select * from ");
    strcat(his_sql_array[type_idx], " ");
    strcat(his_sql_array[type_idx], tablename_array[type_idx]);
    strcat(his_sql_array[type_idx], " ");    
    strcat(his_sql_array[type_idx], SQL_WHERE);
    strcat(his_sql_array[type_idx], " ");
    strcat(his_sql_array[type_idx], FIELD_HIS_TNODE);
    strcat(his_sql_array[type_idx], SQL_EQUAL);
    strcat(his_sql_array[type_idx], SQL_SINGLE_QUOTES);
    strcat(his_sql_array[type_idx], timenode);
    strcat(his_sql_array[type_idx], SQL_SINGLE_QUOTES);
    return NO_ERR;
}

uint8 read_his_data(char* timenode, mtype_idx idx)
{
    uint8 err = NO_ERR;
    char* pErr;
    char log[1024];
    printf("&&&&[%s][%s][%d] retrieve_his_data() &&&&\n", FILE_LINE);
    empty_hist_data(idx);
    get_his_sql(timenode, idx);
    printf("&&&&[%s][%s][%d] sql: %s &&&&\n", FILE_LINE, his_sql_array[idx]);
    err = sqlite3_exec(g_pDB, his_sql_array[idx], each_his_data, &idx, &pErr);
    write_err(err,pErr,log)
    return (err==SQLITE_OK ? NO_ERR : ERR_1);
}

uint8 read_all_his_data(char* timenode)
{
    int i;
    for(i=0;i<MTYPE_CNT;i++) {
        if(read_his_data(timenode, i) != NO_ERR) {
            empty_hist_data(i);
            return ERR_1;
        }
    }
    return NO_ERR;
}

int get_his_cnt(mtype_idx idx)
{
    return hisdata_idx_array[idx];
}

uint8 retrieve_his_data(mtype_idx idx, int cnt, int (*read_one_his)(pHis_data, uint8), uint8 dev)
{
    int i=0;
    pHis_data pTmp_his = his_data_list_array[idx];
    pHis_data pRtn_his = malloc(sizeof(struct his_data_str));
    memset(pRtn_his, 0, sizeof(struct his_data_str));
    printf("&&&&[%s][%s][%d] retrieve_his_data() &&&&\n", FILE_LINE);
    while(pTmp_his && (i<cnt)) {//Ҫ��ȡ���������ܴ���ʣ�µ�����
        memcpy(pRtn_his, pTmp_his, sizeof(struct his_data_str));
        pRtn_his->pNext = NULL;
        pRtn_his->pPrev= NULL;
        pRtn_his->value_cnt = pTmp_his->value_cnt;
        pRtn_his->value_list = malloc(pTmp_his->value_cnt*sizeof(struct meter_item));    
        memcpy(pRtn_his->value_list, pTmp_his->value_list, pTmp_his->value_cnt*sizeof(struct meter_item));
        read_one_his(pRtn_his, dev);
        pTmp_his = pTmp_his->pNext;
        free(pRtn_his->value_list);
        i++;
    }
    free(pRtn_his);
    return NO_ERR;
}

uint8 retrieve_and_del_his_data(mtype_idx idx, int cnt, int (*read_one_his)(pHis_data, uint8), uint8 dev, uint32* f_id)
{
    int i=0;
    pHis_data pTmp_his = his_data_list_array[idx];
    pHis_data pRtn_his = malloc(sizeof(struct his_data_str));
    if(NULL == pRtn_his) {
        printf("@@@@@[%s][%s][%d]malloc error, pRtn_his is NULL: %p @@@@@\n",FILE_LINE, pRtn_his);
        return ERR_1;
    }
        
    memset(pRtn_his, 0, sizeof(struct his_data_str));
    
    while(pTmp_his && (i<cnt)) {//Ҫ��ȡ���������ܴ���ʣ�µ�����
        printf("@@@@@[%s][%s][%d]hisdata_idx_array[%d]: %d @@@@@\n",FILE_LINE, idx, hisdata_idx_array[idx]);
        printf("@@@@@[%s][%s][%d]pTmp_his: %p @@@@@\n",FILE_LINE, pTmp_his);
        printf("@@@@@[%s][%s][%d], idx is: %d, cnt: %d, hisdata_idx_array[%d]: %d\n", FILE_LINE, i, cnt, idx, hisdata_idx_array[idx]);
        memcpy(pRtn_his, pTmp_his, sizeof(struct his_data_str));
        pRtn_his->pNext = NULL;
        pRtn_his->pPrev= NULL;
        pRtn_his->value_cnt = pTmp_his->value_cnt;
        pRtn_his->value_list = malloc(pTmp_his->value_cnt*sizeof(struct meter_item));    
        memcpy(pRtn_his->value_list, pTmp_his->value_list, pTmp_his->value_cnt*sizeof(struct meter_item));
        printf("@@@@@[%s][%s][%d]before read_one_his(pRtn_his, dev), pRtn_his is : %p @@@@@\n",FILE_LINE, pRtn_his);
        f_id[i] = atoi(pRtn_his->f_id);
        read_one_his(pRtn_his, dev);
        free(pRtn_his->value_list);
        //ɾ���Ѷ�ȡ�Ľڵ�
        printf("^^^^^^[%s][%s][%d]start delete one node\n", FILE_LINE);
        his_data_list_array[idx] = his_data_list_array[idx]->pNext;
        if(his_data_list_array[idx]) {
            his_data_list_array[idx]->pPrev = pTmp_his->pPrev;
        }
        free(pTmp_his->value_list);
        free(pTmp_his);
        pTmp_his = his_data_list_array[idx];//ָ����һ���ڵ�
        printf("[%s][%s][%d]pTmp_his: %p, his_data_list_array[idx]: %p\n",FILE_LINE, pTmp_his, his_data_list_array[idx]);
        i++;
        hisdata_idx_array[idx]--;
        printf("[%s][%s][%d]meter_type: %d, read %d rows, %d rows left, request cnt: %d\n",FILE_LINE, idx, i, hisdata_idx_array[idx], cnt);
        printf("[%s][%s][%d]finish delete one node, idx is: %d\n",FILE_LINE, i);
    }
    free(pRtn_his);
    return NO_ERR;
}

uint8 upok_flag(uint32* idx_list, uint32 len, mtype_idx type_idx)
{
    uint8 err = NO_ERR;
    char* tablename = tablename_array[type_idx];
    int i=0;
    char* pErr;
    char log[1024];
    char sql_buf[LENGTH_SQLBUF];
    char f_id[10]={0};
    strcpy(sql_buf, "update ");
    strcat(sql_buf, tablename);
    strcat(sql_buf, " set f_upok=1 where f_id in (");
    
    for(i=0;i<len;i++) {
        sprintf(f_id, "%d", idx_list[i]);
        strcat(sql_buf, f_id);
        if(i<(len-1))
            strcat(sql_buf, ",");
    }
    strcat(sql_buf, ");");
    err = sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);
    write_err(err,pErr,log)
    return err;
}



/********************************************************************************
 ** ��������	: ��ȡ����������
 ********************************************************************************/
uint8 read_all_request_data()
{
	int i;
    uint8 err = NO_ERR;
	for(i=0;i<MTYPE_CNT;i++) {
		err = read_request_data(i);
        if(err != NO_ERR) {
            empty_request_data_list(i);
            return err;
        }
	}
    return err;
}

uint8 read_request_data(mtype_idx type_idx)
{
    uint8 err = NO_ERR;
    char log[1024];
    char *pErr;
	if(NULL == g_pDB) {
        sprintf(log, "[%s][%s][%d]g_db is NULL\n", FILE_LINE);
        write_log_file(log, strlen(log));
        return ERR_1;
	}


	char sql_buf[LENGTH_SQLBUF];
	char where_buf[LENGTH_SQLCON];
	char order_buf[LENGTH_SQLORDER];
	char m_type[LENGTH_F_METER_TYPE];
	char *con_buf = malloc(LENGTH_SQLCONS);
	char *table_name = TABLE_REQUEST_DATA;
	char *col_buf[5] =  {FIELD_REQUEST_ID, \
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
    free(con_buf);

    err = sqlite3_exec(g_pDB, sql_buf, each_request_data, (void*)(&type_idx), &pErr);
    write_err(err,pErr,log)
    return (err==SQLITE_OK) ? NO_ERR : ERR_1;
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

	add_node(arrayRequest_list[idx], tmp_request)
	request_data_idx[idx]++;

	return 0;
}

void retrieve_request_data_list(int (*read_one_item)(pRequest_data, void *), mtype_idx type_idx, void *pVoid)
{
	if(!read_one_item)
		return;

	pRequest_data rt_request  = malloc(sizeof(struct request_data_str));
	pRequest_data tmp_request = arrayRequest_list[type_idx];
	while(tmp_request) {
		memcpy(rt_request, tmp_request, sizeof(struct request_data_str));
		rt_request->pPrev = NULL;
		rt_request->pNext = NULL;
		read_one_item(rt_request, pVoid);
		tmp_request = tmp_request->pNext;
	}
	free(rt_request);
}

static void empty_request_data_list(mtype_idx type_idx)
{
    empty_list(pRequest_data, arrayRequest_list[type_idx])
    request_data_idx[type_idx] = 0;
}

int  get_request_data_cnt(mtype_idx idx)
{
	return request_data_idx[idx];
}

static uint8 del_request_data(char* pId)
{
    int err=0;
    char* pErr;
    char sql_buf[LENGTH_SQLBUF];
    char log[1024];
	strcpy(sql_buf, SQL_DELETE);
	strcat(sql_buf, " ");
	strcat(sql_buf, SQL_FROM);
	strcat(sql_buf, " ");
	strcat(sql_buf, TABLE_REQUEST_DATA);

    if(NULL != pId) {
        strcat(sql_buf, " ");
        strcat(sql_buf, SQL_WHERE);
        strcat(sql_buf, " ");
        strcat(sql_buf, FIELD_BASE_DEF_ID);
        strcat(sql_buf, SQL_EQUAL);
        strcat(sql_buf, pId);
    }
    err = sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);
    write_err(err,pErr,log)
    return err==SQLITE_OK ? NO_ERR : ERR_1;
}

uint8 insert_one_request_node(pRequest_data pRqData)
{
    uint8 err=NO_ERR;
    add_node(list_set_request_data, pRqData)
    set_request_data_idx++;
    return err;
}

int get_request_data_setted()
{
    return set_request_data_idx;
}

uint8 add_one_request_data(pRequest_data pNode)
{
    if(NULL == pNode) {
    	    return ERR_1;
    }
    uint8 err=NO_ERR;
    char* pErr;
    char log[1024];
    char sql_buf[LENGTH_SQLBUF];
    char *table_name = TABLE_REQUEST_DATA;
    char *col_buf[LENGTH_F_COL_NAME] = {FIELD_REQUEST_MTYPE, FIELD_REQUEST_ITEMIDX, FIELD_REQUEST_COLNAME, FIELD_REQUEST_COLTYPE};
    char meter_type[5];
    char item_idx[5];
    sprintf(meter_type, "%02x", pNode->f_meter_type);
    sprintf(item_idx, "%d", pNode->f_item_index);
    char *val_buf[LENGTH_SQLVALUE] = {meter_type, item_idx, pNode->f_col_name, pNode->f_col_type};
    get_insert_sql(table_name, col_buf, 4, val_buf, sql_buf, 1);
    err =  sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);
    write_err(err, pErr, log)
    printf("[%s][%s][%d]pErr: %s\n", FILE_LINE, pErr);
    return err==SQLITE_OK ? NO_ERR : ERR_1;
}

uint8 set_request_data()
{
    uint8 err=NO_ERR;
    err = del_request_data(NULL);
    if(err != NO_ERR)
        return ERR_1;
    pRequest_data pNode = list_set_request_data;
    	while(pNode) {
		if(NO_ERR != add_one_request_data(pNode)) {
			return ERR_1;
		}
		pNode = pNode->pNext;
	}

    empty_list(pRequest_data,list_set_request_data)
    set_request_data_idx = 0;
    err = read_all_request_data();
    return err;
}



/********************************************************************************
 **	 ������: read_meter_info
 ** ����	: �����ݿ��ж�ȡ�Ǳ��ַ��Ϣ, �ŵ�list_meter_info��
 ** ��Ҫ  : �������ʱ����list_meter_infoΪNULL, ���򱾳������ֶδ���
 ********************************************************************************/

uint8 read_meter_info()
{
    uint8 err = NO_ERR;
    char log[LENGTH_SQLBUF*2];
    char *pErr;
	if(NULL == g_pDB) {
        sprintf(log, "[%s][%s][%d]g_db is NULL\n", FILE_LINE);
        write_log_file(log, strlen(log));
        return ERR_1;
	}

    char sql_buf[LENGTH_SQLBUF];
    char order_buf[LENGTH_SQLORDER];
    char *table_name = TABLE_METER_INFO;
    char *col_buf[7] =  {FIELD_MINFO_ADDRESS, FIELD_MINFO_TYPE, FIELD_MINFO_CHANNEL,\
        FIELD_MINFO_ID, FIELD_MINFO_POS, FIELD_MINFO_DEVICE_ID, \
        FIELD_MINFO_PROTO_TYPE};//ָ������
    int  col_cnt = 7;

    meter_info_idx = 0;
    empty_meter_info_list();//�����ǰ����Ϣ, �����¶�ȡ
    get_select_sql(table_name, col_buf, col_cnt, sql_buf);
    get_orderby_sql(&col_buf[2], 2, 1, order_buf);
    strcat(sql_buf, " ");
    strcat(sql_buf, order_buf);
    strcat(sql_buf, ";");
    printf("[%s][%s][%d]read meter info: %s\n", FILE_LINE, sql_buf);
    sprintf(log, "[%s][%s][%d]read meter info: %s\n", FILE_LINE, sql_buf);
    write_log_file(log, strlen(log));
    err = sqlite3_exec(g_pDB, sql_buf, each_meter_info, NULL, &pErr);
    write_err(err,pErr,log)
    return (err==SQLITE_OK) ? NO_ERR : ERR_1;
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
				tmp_info->f_meter_address[j] = \
					(((low_idx < 0) ? 0: Ascii2Hex(f_value[i][low_idx])) << LEN_HALF_BYTE | Ascii2Hex(f_value[i][low_idx+1]));
			}
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_TYPE)) {//�Ǳ����ͱ���(HEX String), �̶�Ϊ�����ַ�
			printf("[%s][%s][%d] meter_type length : %d\n", FILE_LINE, strlen(f_value[i]));
			if(strlen(f_value[i]) > 0)
				printf("[%s][%s][%d] meter_type: %s\n", FILE_LINE, (f_value[i]));
			if (strlen(f_value[i]) == BYTE_BCD_CNT) {
				tmp_info->f_meter_type = (Ascii2Hex(f_value[i][0]) << LEN_HALF_BYTE | Ascii2Hex(f_value[i][1]));
			} else {//�쳣��
				printf("[%s][%s][%d] meter_type length error %d\n", FILE_LINE, strlen(f_value[i]));
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
    printf("[%s][%s][%d]", FILE_LINE);
    for(i=0;i<7;i++) {
        printf("%d",tmp_info->f_meter_address[i]);
    }
    printf("\n");
    add_node(list_meter_info, tmp_info)
    meter_info_idx++;
    return 0;
}

uint8 insert_one_meter_info(pMeter_info pMinfo)
{
    add_node(list_meter_info, pMinfo)
    meter_info_idx++;
    return NO_ERR;
}

int insert_one_meter_info_to_table(pMeter_info pMinfo)
{
    
	printf("now in  insert_one_meter_info_to_table():\n");
    char sql_buf[LENGTH_SQLBUF];
    char tmpstr[100];
    int err;
    strcpy(sql_buf, "insert into ");
    strcat(sql_buf, TABLE_METER_INFO);
    strcat(sql_buf, " ");
    strcat(sql_buf, "(f_meter_type, f_device_id, f_meter_address, f_meter_channel, f_meter_proto_type, f_install_pos)values(");
    //values
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    sprintf(tmpstr, "%02x", pMinfo->f_meter_type);
    strcat(sql_buf, tmpstr);
    strcat(sql_buf, " ");
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    strcat(sql_buf, ",");

    
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    sprintf(tmpstr, "%d", pMinfo->f_device_id);
    strcat(sql_buf, tmpstr);
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    strcat(sql_buf, ",");


    strcat(sql_buf, SQL_SINGLE_QUOTES);
    sprintf(tmpstr, "%02x%02x%02x%02x%02x%02x%02x", pMinfo->f_meter_address[6], \
        pMinfo->f_meter_address[5], pMinfo->f_meter_address[4], pMinfo->f_meter_address[3], \
        pMinfo->f_meter_address[2], pMinfo->f_meter_address[1], pMinfo->f_meter_address[0]);
    strcat(sql_buf, tmpstr);
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    strcat(sql_buf, ",");


    strcat(sql_buf, SQL_SINGLE_QUOTES);
    sprintf(tmpstr, "%d", pMinfo->f_meter_channel);
    strcat(sql_buf, tmpstr);
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    strcat(sql_buf, ",");


    strcat(sql_buf, SQL_SINGLE_QUOTES);
    sprintf(tmpstr, "%d", pMinfo->f_meter_proto_type);
    strcat(sql_buf, tmpstr);
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    strcat(sql_buf, ",");
    strcat(sql_buf, SQL_SINGLE_QUOTES);

    if(strlen(pMinfo->f_install_pos)>0) {
        strcat(sql_buf, pMinfo->f_install_pos);
    }else{
        strcat(sql_buf, "NULL");
    }
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    strcat(sql_buf, SQL_RIGHT_PARENTHESIS);
    printf("now in  insert_one_meter_info_to_table(), insert sql_buf: %s\n", sql_buf);
    err = sqlite3_exec(g_pDB, sql_buf, NULL, NULL, NULL);
    return (err==SQLITE_OK) ? NO_ERR : ERR_1;
}

uint8 insert_into_meter_info_table()
{
    int err=NO_ERR;
    printf("now in  insert_into_meter_info_table():\n");
    err = retrieve_meter_info_list(insert_one_meter_info_to_table);
    return err;
}


int get_meter_info_cnt()
{
	return meter_info_idx;
}

//����Ǳ���Ϣ, �Ա����¶�ȡ
//�˺������еĻ�����, list�����ȳ�ʼ��ΪNULL
void empty_meter_info_list()
{
    empty_list(pMeter_info, list_meter_info)
    meter_info_idx = 0;
}

uint8 empty_meter_info_table()
{
    int err;
    char *pErr;
    char log[1024];
    char *sql = "delete from t_meter_info";
    err = sqlite3_exec(g_pDB, sql, NULL, NULL, &pErr);
    write_err(err,pErr,log)
    return (err == SQLITE_OK) ? NO_ERR : ERR_1;
}


//�����Ǳ���Ϣ, ��ÿһ�������read_one_meter����
uint8 retrieve_meter_info_list(int (*read_one_meter)(pMeter_info))
{
	if(!read_one_meter)
		return ERR_1;
	printf("now in  retrieve_meter_info_list():\n");
	pMeter_info pInfo_return = malloc(sizeof(struct meter_info_str));//�ش�һ�������Ľṹ��, ��֤ԭʼ���ݵİ�ȫ
    if(list_meter_info == NULL)
        return ERR_1;

	pMeter_info pInfo = list_meter_info;

	while(pInfo) {
		memcpy(pInfo_return, pInfo, sizeof(struct meter_info_str));
		pInfo_return->pNext = NULL;
		pInfo_return->pPrev = NULL;
		read_one_meter(pInfo_return);
		pInfo = pInfo->pNext;
	}
	free(pInfo_return);
    return  NO_ERR;
}



/********************************************************************************
 **	 ������: read_sys_config
 ** ����	: �����ݿ��ж�ȡ��������, �ŵ�sys_config_array��
 ********************************************************************************/
uint8 read_sys_config()
{
    uint8 err = NO_ERR;
    char log[1024];
    char *pErr;
	if(NULL == g_pDB) {
        sprintf(log, "[%s][%s][%d]g_db is NULL\n", FILE_LINE);
        write_log_file(log, strlen(log));
        return ERR_1;
	}

	char sql_buf[LENGTH_SQLBUF];
	char order_buf[LENGTH_SQLORDER];
	char *table_name = TABLE_BASE_DEF;
	char *col_buf[3] = {FIELD_BASE_DEF_ID, FIELD_BASE_DEF_NAME, FIELD_BASE_DEF_VALUE};
	int	col_cnt = 3;

	memset(sys_config_array, 0, SYS_CONFIG_COUNT*sizeof(sys_config_str));//���ԭ������
	config_idx = 0;

    //��ȡ���ݱ��ڵ�����
    get_select_sql(table_name, col_buf, col_cnt, sql_buf);
    get_orderby_sql(col_buf, 1, 0, order_buf);
    strcat(sql_buf, " ");
    strcat(sql_buf, order_buf);
    err = sqlite3_exec(g_pDB, sql_buf, each_config, NULL, &pErr);
    write_err(err, pErr, log)
    sysConfig_Ascii2hex();
    return (err==SQLITE_OK) ? NO_ERR : ERR_1;
}

static int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name)
{
    int i;
    int idx= -1;
    char config_name[LENGTH_F_CONFIG_NAME];
    char config_value[LENGTH_F_CONFIG_VALUE];
    for (i=0; i<f_cnt; i++) {//�Ƚ�ֵ�����м�ֵ��, Ȼ��ֵ, �Ա�֤����������ֵ��Ӧ,���ϸ���ö�ٵ�˳����
        if (0 == strcmp(f_name[i], FIELD_BASE_DEF_ID))
            idx = atoi(f_value[i]);
        else if(0 == strcmp(f_name[i], FIELD_BASE_DEF_NAME))
            strcpy(config_name, f_value[i]);
        else if(0 == strcmp(f_name[i], FIELD_BASE_DEF_VALUE))
            strcpy(config_value, f_value[i]);
    }
    if(idx<1 || idx>SYS_CONFIG_COUNT) {
        printf("[%s][%s][%d] sys_config's idx is overflow!\n", FILE_LINE);
        return ERR_1;
    }
    sys_config_array[idx-1].f_id  = idx;//���ݿ������ֵ�Ǵ�1��ʼ��, ����Ҫ��һ
    strcpy(sys_config_array[idx-1].f_config_name, config_name);
    strcpy(sys_config_array[idx-1].f_config_value, config_value);
    config_idx++;
    return 0;
}

int get_sys_config_cnt()
{
	return config_idx;
}

uint8 get_sys_config(sys_config_idx idx, pSys_config pConfig)
{
	if(NULL==pConfig || idx<0 || idx>=SYS_CONFIG_COUNT)
		return ERR_1;
	
	memcpy((uint8*)pConfig, (uint8*)&sys_config_array[idx], sizeof(sys_config_str));
	return NO_ERR;
}

/*
 * ���ϵͳ������Ϣ
 * ע��: �˺���Ӧ���ɵ����������б��������ǰ����һ��,
 * ���򽫳��ִ���(�罫��ǰ����Ϣ���뵽���ݱ���, ���߳���������ͻ)
 */
uint8 empty_sysconf_list()
{
    empty_list(pSys_config, list_set_sysconf)
    set_sysconf_idx = 0;
    return NO_ERR;
}

uint8 insert_sysconf(pSys_config pConfig)
{
	if(NULL == pConfig) {
		return ERR_1;
	}
	printf("[%s][%s][%d]list_set_sysconf: %p\n", FILE_LINE, list_set_sysconf);
	pConfig->pNext = list_set_sysconf;
	if (list_set_sysconf) {
        pConfig->pPrev = list_set_sysconf->pPrev;
        list_set_sysconf->pPrev = pConfig;
	}
	list_set_sysconf = pConfig;
	set_sysconf_idx++;
	return NO_ERR;
}

uint8 add_one_config(pSys_config pConf)
{
    if(NULL == pConf) {
    	return ERR_1;
    }
    uint8 err = NO_ERR;
    char *pErr;
    char log[1024];
    char sql_buf[LENGTH_SQLBUF];
    char *table_name = TABLE_BASE_DEF;
    char *col_buf[LENGTH_F_COL_NAME] = {FIELD_BASE_DEF_ID, FIELD_BASE_DEF_NAME, FIELD_BASE_DEF_VALUE};
    char pIdstr[LENGTH_F_CONFIG_VALUE];
    char *val_buf[LENGTH_F_CONFIG_VALUE] = {pIdstr, pConf->f_config_name, pConf->f_config_value};
    
    int row_cnt=0;//��ѯ��ǰf_id�м���. ���Ϊ1, ���������; ���Ϊ0, �����������
    sqlite3_stmt *countstmt;

    sprintf(pIdstr, "%d", pConf->f_id+1);
    printf("[%s][%s][%d]list_set_sysconf: %p, pIdstr: %s\n", FILE_LINE, list_set_sysconf, pIdstr);
    strcpy(sql_buf, "select count(*) from t_base_define where f_id=");
    strcat(sql_buf, pIdstr);
    printf("[%s][%s][%d]sql_buf: %s\n", FILE_LINE, sql_buf);
    if(sqlite3_prepare_v2(g_pDB, sql_buf, -1, &countstmt, NULL) == SQLITE_OK) {
        if(sqlite3_step(countstmt) == SQLITE_ROW){
            row_cnt = sqlite3_column_int(countstmt, 0);
            printf("cnt is: %d\n", row_cnt);
        }
    }
    
    printf("[%s][%s][%d]row_cnt: %d\n", FILE_LINE, row_cnt);
    if(row_cnt == 0) {//���ݱ���û�е�����, �����
        get_insert_sql(table_name, col_buf, 3, val_buf, sql_buf, 1);
    }
    else {//���ݱ����е�����, �����
        strcpy(sql_buf, "update t_base_define set f_config_value=");
        strcat(sql_buf, SQL_SINGLE_QUOTES);
        strcat(sql_buf, pConf->f_config_value);
        strcat(sql_buf, SQL_SINGLE_QUOTES);
        strcat(sql_buf, " where f_id=");
        strcat(sql_buf, pIdstr);
    }
    printf("[%s][%s][%d]sql_buf: %s\n", FILE_LINE, sql_buf);
    err =  sqlite3_exec(g_pDB, sql_buf, NULL, NULL, &pErr);
    write_err(err, pErr, log)
    return err==SQLITE_OK ? NO_ERR : ERR_1;
}

uint8 set_sysconf()
{
	printf("[%s][%s][%d]list_set_sysconf: %p\n", FILE_LINE, list_set_sysconf);
	pSys_config pTmp_conf = list_set_sysconf;
	while(pTmp_conf) {
		if(NO_ERR != add_one_config(pTmp_conf)) {
			return ERR_1;
		}
		pTmp_conf = pTmp_conf->pNext;
	}
	empty_sysconf_list();//��������б�
	return read_sys_config();//���¶�ȡ���ݿ������Ϣ
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
