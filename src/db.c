/*
  *******************************************************************************
  * @file    db.c
  * @author  songbaoshan
  * @version V0.0.0
  * @date    03-03-2016
  * @brief   
  * @attention: 本文件的 get_*_sql() 函数, 结构与功能类似
  * 参数列表结构: (数据表名, [(列名, 列数量); (条件, 条件数量); ...], 要得到的SQL语句)
  * 无返回值.
  * 如果给出的[列名/条件]数量小于等于0, 那么就将传入的SQL指向的内存区域全部设置为
  * 空字符'\0', sql在传入之前要提前指向有效的内存区, 在函数内不再初始化.
  * 本模块内的函数基本不是线程安全的, 使用之前需对每项操作进行信号量的匹配和检测,
  * 或者仿照xml模块, 给每一种设备都设置一个对应的数据库对象, 避免多个线程对同一个
  * 数据库对象进行读写(未实现).
  *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "readallmeters.h"
#include "read_heatmeter.h"

#include <sqlite3/sqlite3.h>

#include "db.h"

sqlite3 *g_pDB = NULL;//数据库指针, 私有变量

/**********************
 ** 系统配置相关 **
 **********************/
static sys_config_str sys_config_array[SYS_CONFIG_COUNT];//基本配置列表, 私有变量
static sys_config_list list_set_sysconf = NULL;//上位机设置系统参数的列表, 私有变量
static int set_sysconf_idx;//设置系统参数的索引号, 私有变量
static int config_idx;//基本配置的个数索引, 私有变量
static int each_config(void *NotUsed, int f_cnt, char **f_value, char **f_name);

/**********************
 ** 读取仪表信息相关 **
 **********************/
static meter_info_List list_meter_info = NULL;//仪表信息列表, 私有变量
static int meter_info_idx;//集中器挂载的仪表数量的索引, 私有变量
static int each_meter_info(void *NotUsed, int f_cnt, char **f_value, char **f_name);

/**********************
 ** 读取数据项相关 **
 **********************/

//u8Meter_type和aItems_list的索引顺序不能搞乱, 须以enum meter_type_idx规定的顺序为准
static uint8 u8Meter_type[] = {HEATMETER, WATERMETER, ELECTMETER, GASMETER};
static request_data_list arrayRequest_list[MTYPE_CNT]={NULL};//仪表信息列表的数组, 私有变量

static int request_data_idx[MTYPE_CNT]={0};//配置数据项数量的索引, 私有变量
static int each_request_data(void *type_idx, int f_cnt, char **f_value, char **f_name);
static void empty_request_data_list(enum meter_type_idx type_idx);
/**********************
 ** 仪表历史数据相关 **
 **********************/
his_data_list his_data_list_array[MTYPE_CNT] = {NULL};
int hisdata_idx_array[MTYPE_CNT];

char his_sql_array[MTYPE_CNT][LENGTH_SQLBUF];
static uint8 data_item_idx;//数据项的下标号, 用于指针移位, 不是线程安全的

//索引顺序同enum meter_type_idx
static char* tablename_array[] = {TABLE_HEAT, TABLE_WATER, TABLE_ELEC, TABLE_GAS};




/********************************************************************************
 **	 函数名: open_db
 ** 功能	: 打开数据库
 ********************************************************************************/
int open_db(void)
{
	 return (sqlite3_open(SQLITE_NAME, &g_pDB) == SQLITE_OK) ? SQLITE_OK : -1;
}

/********************************************************************************
 **	 函数名: close_db
 ** 功能	: 关闭数据库
 ********************************************************************************/
int close_db(void)
{
	 return (sqlite3_close(g_pDB) == SQLITE_OK) ? SQLITE_OK : -1;
}

/********************************************************************************
 ** 功能区域	: 仪表历史数据相关 
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
			strcpy(pErr, "meter type error");//不能直接赋值, 否则函数运行结束后, 常量字符串的空间就被销毁
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
	tmp_col_buf = col_buf;//指向第一个元素
	//insert into
	strcpy(sql_buf, SQL_INSERT);
	strcat(sql_buf, " ");
	strcat(sql_buf, table_name);
	strcat(sql_buf, SQL_LEFT_PARENTHESIS);
	//columns
	//固定项
	strcat(sql_buf, FIELD_HIS_ADDRESS);//表地址
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_MTYPE);//表类型
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_DEVID);//设备编号
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_TSTAMP);//时间戳
	strcat(sql_buf, ",");
	strcat(sql_buf, FIELD_HIS_TNODE);//抄表时间点
	strcat(sql_buf, ",");
	for(i=0;i<item_cnt-1;i++, tmp_col_buf += LENGTH_F_COL_NAME) {
		//printf("tmp_col_buf: %s\n", tmp_col_buf);
		strcat(sql_buf, tmp_col_buf);
		strcat(sql_buf, ",");
	}
	//printf("tmp_col_buf: %s\n", tmp_col_buf);
	strcat(sql_buf, tmp_col_buf);
	strcat(sql_buf, SQL_RIGHT_PARENTHESIS);
	tmp_col_buf = col_buf;//指向第一个元素
	
	//values
	strcat(sql_buf, SQL_VALUES);
	strcat(sql_buf, SQL_LEFT_PARENTHESIS);
	//固定项
	sprintf(tmp_data, "%02x%02x%02x%02x%02x%02x%02x", pmf->u8MeterAddr[6], \
	pmf->u8MeterAddr[5], pmf->u8MeterAddr[4], pmf->u8MeterAddr[3], 
	pmf->u8MeterAddr[2], pmf->u8MeterAddr[1], pmf->u8MeterAddr[0]);
	strcat(sql_buf, tmp_data);//表地址
	strcat(sql_buf, ",");
	sprintf(tmp_data, "%02x",pmf->u8MeterType);
	strcat(sql_buf, tmp_data);//表类型
	strcat(sql_buf, ",");
	sprintf(tmp_data, "%s", "NULL");//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!暂时为NULL, 后期完善!!!!!!!!!!!!!!!!!!!!!
	strcat(sql_buf, tmp_data);//设备编号
	strcat(sql_buf, ",");

    //asctime_r与asctime返回固定格式'DDD MMM dd hh:mm:ss YYYY',星期 月份 日期 时间 年
	asctime_r(pNowTime, tmp_data);//与sqlite3的时间戳格式不兼容,需转换为'YYYY-MM-DD hh:mm:ss'
    char dest[25]={0};
    asc_to_datestr(tmp_data, dest);
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, dest);//时间戳
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, ",");
    
    asctime_r(pTimeNode, tmp_data);
    memset(dest, 0, 25);
    asc_to_datestr(tmp_data, dest);
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, dest);//抄表时间点
	strcat(sql_buf, SQL_SINGLE_QUOTES);
	strcat(sql_buf, ",");
	item_list = arrayRequest_list[type_idx];
	CJ188_Format* heatdata;
	uint8 *p;//改变CJ188_Format中uint32类型的字节序
	switch(type_idx) {
	case em_heat:
        if (NULL==pData) {//与调用者约定, 如果传入了NULL值, 就认为当前仪表的当前时间点的历史数据没抄上来
            while(item_list) {
    			strcat(sql_buf, "'NULL'");
    			if (item_list->pNext)//如果不是倒数第一个, 就在后面加逗号, 否则不加
    				strcat(sql_buf, ",");
    			item_list = item_list->pNext;
            }
        }
         else{
    		heatdata = (CJ188_Format*)pData;
    		while(item_list) {//item_list->f_item_index的顺序和item_list->f_col_name的顺序是一致的, 不必担心value值顺序的混淆
    			memset(tmp_data, 0, LENGTH_F_COL_NAME);//使用之前置0
    			switch(item_list->f_item_index) {
    			case HITEM_CUR_COLD_E:
    				p=(uint8 *)&(heatdata->DailyHeat);
    				sprintf(tmp_data, "%02x%02x%02x%02x%02x", *p, *(p+1), *(p+2), *(p+3), heatdata->DailyHeatUnit);//实际为冷量, 而不是结算日热量
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
    			if (item_list->pNext)//如果不是倒数第一个, 就在后面加逗号, 否则不加
    				strcat(sql_buf, ",");
    			item_list = item_list->pNext;
    		}
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
    free(tmp_data);
    free(col_buf);
    free(sql_buf);
}

int read_one_item_info(pRequest_data pReques, void* pHisData)
{
    pMeter_item pItem = (pMeter_item) pHisData;
    strcpy(pItem[data_item_idx].field_name, pReques->f_col_name);//列名
    pItem[data_item_idx].item_index = pReques->f_item_index;//数据项索引
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
        if (0 == strcmp(f_name[i], FIELD_HIS_ID)){//数据项ID
            strcpy(tmp_his->f_id, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_MTYPE)){//仪表类型(Hex String)
            strcpy(tmp_his->f_meter_type, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_DEVID)) {//仪表设备编号的索引号(Dec String)
            strcpy(tmp_his->f_device_id, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_ADDRESS)){//表地址(BCD String)
            strcpy(tmp_his->f_meter_address, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_TNODE)){//抄表时间点(String)
            strcpy(tmp_his->f_time, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else if(0 == strcmp(f_name[i], FIELD_HIS_TSTAMP)){//时间戳(String)
            strcpy(tmp_his->f_timestamp, f_value[i]==NULL?"NULL":f_value[i]);
        }
        else {//数据项
            for(j=0;j<tmp_his->value_cnt;j++)
                if(0 == strcmp(tmp_his->value_list[j].field_name, f_name[i]))
                    strcpy(tmp_his->value_list[j].field_value, f_value[i]==NULL?"NULL":f_value[i]);
        }
    }
    printf("&&&&[%s][%s][%d] his_data_list_array[%d]: %p\n", FILE_LINE, idx, his_data_list_array[idx]);
    add_node(his_data_list_array[idx], tmp_his)
    hisdata_idx_array[idx]++;

    return 0;
}

uint8 empty_hist_data(mtype_idx idx)
{
    printf("&&&&[%s][%s][%d] empty_hist_data() &&&&\n", FILE_LINE);
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
    //printf("his_sql_array[%d]: %s\n", type_idx, his_sql_array[type_idx]);
    return NO_ERR;
}

uint8 read_his_data(char* timenode, mtype_idx idx, char* pErr)
{
    uint8 err = NO_ERR;
    printf("&&&&[%s][%s][%d] retrieve_his_data() &&&&\n", FILE_LINE);
    empty_hist_data(idx);
    get_his_sql(timenode, idx);
    err = sqlite3_exec(g_pDB, his_sql_array[idx], each_his_data, &idx, &pErr);
    return (err==SQLITE_OK?NO_ERR:ERR_1);
}

uint8 read_all_his_data(char* timenode, char* pErr)
{
    int i;
    for(i=0;i<MTYPE_CNT;i++) {
        if(read_his_data(timenode, i, pErr))
            return ERR_1;
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
    while(pTmp_his && (i<cnt)) {//要读取的行数不能大于剩下的行数
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

uint8 retrieve_and_del_his_data(mtype_idx idx, int cnt, int (*read_one_his)(pHis_data, uint8), uint8 dev)
{
    int i=0;
    pHis_data pTmp_his = his_data_list_array[idx];
    pHis_data pRtn_his = malloc(sizeof(struct his_data_str));
    memset(pRtn_his, 0, sizeof(struct his_data_str));
    
    while(pTmp_his&& (i<cnt) /* && (i<hisdata_idx_array[idx])*/) {//要读取的行数不能大于剩下的行数
        printf("@@@@@[%s][%s][%d]hisdata_idx_array[%d]: %d @@@@@\n",FILE_LINE, idx, hisdata_idx_array[idx]);
        printf("@@@@@[%s][%s][%d]pTmp_his: %p @@@@@\n",FILE_LINE, pTmp_his);
        printf("@@@@@[%s][%s][%d], idx is: %d, cnt: %d, hisdata_idx_array[%d]: %d\n", FILE_LINE, i, cnt, idx, hisdata_idx_array[idx]);
        memcpy(pRtn_his, pTmp_his, sizeof(struct his_data_str));
        pRtn_his->pNext = NULL;
        pRtn_his->pPrev= NULL;
        pRtn_his->value_cnt = pTmp_his->value_cnt;
        pRtn_his->value_list = malloc(pTmp_his->value_cnt*sizeof(struct meter_item));    
        memcpy(pRtn_his->value_list, pTmp_his->value_list, pTmp_his->value_cnt*sizeof(struct meter_item));
        read_one_his(pRtn_his, dev);
        free(pRtn_his->value_list);
        //删除已读取的节点
        printf("^^^^^^[%s][%s][%d]start delete one node\n", FILE_LINE);
        his_data_list_array[idx] = his_data_list_array[idx]->pNext;
        if(his_data_list_array[idx]) {
            his_data_list_array[idx]->pPrev = pTmp_his->pPrev;
        }
        free(pTmp_his->value_list);
        free(pTmp_his);
        pTmp_his = his_data_list_array[idx];//指向下一个节点
        printf("pTmp_his: %p, his_data_list_array[idx]: %p\n", pTmp_his, his_data_list_array[idx]);
        i++;
        hisdata_idx_array[idx]--;
        printf("finish delete one node, idx is: %d\n", i);
    }
    free(pRtn_his);
    return NO_ERR;
}


/********************************************************************************
 ** 功能区域	: 读取数据项配置
 ********************************************************************************/
void read_all_request_data(char	*pErr)
{
	int i;
	for(i=0;i<MTYPE_CNT;i++){//读取t_request_data的所有数据, 如果当前t_request_data没有燃气表的信息, 
								//那么也不会读取到arrayRequest_list中, 因为数据库在查询燃气表信息时, 不会返回结果, 
								//也就把arrayRequest_list[燃气]的数据置之不理
		read_request_data(pErr, i);
	}
}

void read_request_data(char *pErr, mtype_idx type_idx)
{
	if(NULL == g_pDB) {
		pErr = "database not open";
		return;
	}

	char *sql_buf = malloc(LENGTH_SQLBUF);
	char *where_buf = malloc(LENGTH_SQLCON);
	char *order_buf = malloc(LENGTH_SQLORDER);
	char *m_type = malloc(LENGTH_F_METER_TYPE);
	//char *con_buf[LENGTH_SQLCONS] = {FIELD_REQUEST_MTYPE};//这样初始化后, con_buf[0]指向const char*, 不能再进行连接, copy等操作
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

	empty_request_data_list(type_idx);//清空以前的信息, 以重新读取
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_where_sql(&con_buf, 1, where_buf);
	get_orderby_sql(&col_buf[2], 1, 1, order_buf);//本程序用到的线性表是先进后出, 所以降序读取, 升序存储
	strcat(sql_buf, " ");
	strcat(sql_buf, where_buf);
	strcat(sql_buf,	" ");
	strcat(sql_buf, order_buf);
	strcat(sql_buf, ";");

	sqlite3_exec(g_pDB, sql_buf, each_request_data, (void*)(&type_idx), &pErr);
	free(con_buf);
	free(m_type);
	free(order_buf);
	free(where_buf);
	free(sql_buf);	
}

static int each_request_data(void *meter_type_idx, int f_cnt, char **f_value, char **f_name)
{
	int i;
	int idx = *((int*)meter_type_idx);
	pRequest_data tmp_request = malloc(sizeof(struct request_data_str));
	memset(tmp_request, 0, sizeof(struct request_data_str));
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_REQUEST_ID))//数据项ID
			tmp_request->f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_MTYPE)){//仪表类型(Hex String)
			tmp_request->f_meter_type = (Ascii2Hex(f_value[i][0])<<LEN_HALF_BYTE | Ascii2Hex(f_value[i][1]));
		}
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_ITEMIDX)) {//仪表数据项的索引号(Dec String)
			tmp_request->f_item_index = atoi(f_value[i]);
		}
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_COLNAME))//仪表数据项的列名(Dec String)
			strcpy(tmp_request->f_col_name, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_REQUEST_COLTYPE))//列名的类型
			strcpy(tmp_request->f_col_type, f_value[i]);
		else {//异常情况
			
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

/********************************************************************************
 **	 函数名: read_meter_info
 ** 功能	: 从数据库中读取仪表地址信息, 放到list_meter_info中
 ** 重要  : 程序加载时须置list_meter_info为NULL, 否则本程序会出现段错误
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
	char *col_buf[7] =  {FIELD_MINFO_ID, \
				FIELD_MINFO_ADDRESS, FIELD_MINFO_TYPE, FIELD_MINFO_CHANNEL,\
				FIELD_MINFO_POS, FIELD_MINFO_DEVICE_ID, FIELD_MINFO_PROTO_TYPE};//指针数组
	int  col_cnt = 7;
	
	meter_info_idx = 0;
	empty_meter_info_list();//清空以前的信息, 以重新读取
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
	int i, j;//i, 一条记录的字段名索引; j表地址
	int length;//从数据表中读取的表地址字符串的长度
	int low_idx;//从数据表中读取的表地址字符串, 高位(低索引)下标
	pMeter_info tmp_info = malloc(sizeof(struct meter_info_str));
	memset(tmp_info, 0, sizeof(struct meter_info_str));
	for (i=0; i<f_cnt; i++) {
		if (0 == strcmp(f_name[i], FIELD_MINFO_ID))//仪表ID
			tmp_info->f_id  = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_ADDRESS)) {//仪表地址(BCD String)
			length = strlen(f_value[i]);
			for (j=0; j<(length+1)/BYTE_BCD_CNT;j++) {
				low_idx = length-BYTE_BCD_CNT*j-2;
				tmp_info->f_meter_address[j] = \
					(((low_idx < 0) ? 0: Ascii2Hex(f_value[i][low_idx])) << LEN_HALF_BYTE | Ascii2Hex(f_value[i][low_idx+1]));
			}
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_TYPE)) {//仪表类型编码(HEX String), 固定为两个字符
			if (strlen(f_value[i]) == BYTE_BCD_CNT) {
				tmp_info->f_meter_type = (Ascii2Hex(f_value[i][0]) << LEN_HALF_BYTE | Ascii2Hex(f_value[i][1]));
			}
			else {//异常情况
			}
		}
		else if(0 == strcmp(f_name[i], FIELD_MINFO_CHANNEL))//仪表通道(Dec String)
			tmp_info->f_meter_channel = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_POS))//仪表安装位置
			strcpy(tmp_info->f_install_pos, f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_DEVICE_ID))//仪表的设备编号(Dec String)
			tmp_info->f_device_id = atoi(f_value[i]);
		else if(0 == strcmp(f_name[i], FIELD_MINFO_PROTO_TYPE))//仪表的协议编码(Dec String)
			tmp_info->f_meter_proto_type = atoi(f_value[i]);
		else {//异常情况

		}
	}

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
    strcat(sql_buf, pMinfo->f_install_pos);
    strcat(sql_buf, SQL_SINGLE_QUOTES);
    
    strcat(sql_buf, SQL_RIGHT_PARENTHESIS);
    printf("now in  insert_one_meter_info_to_table(), insert sql_buf: %s\n", sql_buf);
    err = sqlite3_exec(g_pDB, sql_buf, NULL, NULL, NULL);
    return (err==SQLITE_OK) ? NO_ERR : ERR_1;
}

uint8 insert_into_meter_info_table(char* pErr)
{
    int err=NO_ERR;
    printf("now in  insert_into_meter_info_table():\n");
    retrieve_meter_info_list(insert_one_meter_info_to_table);
    return err;
}


int get_meter_info_cnt()
{
	return meter_info_idx;
}

//清空仪表信息, 以便重新读取
//此函数运行的基础是, list必须先初始化为NULL
void empty_meter_info_list()
{
    empty_list(pMeter_info, list_meter_info)
    meter_info_idx = 0;
}

uint8 empty_meter_info_table(char* pErr)
{
    int err;
    char *sql = "delete from t_meter_info";
    err = sqlite3_exec(g_pDB, sql, NULL, NULL, &pErr);
    return (err == SQLITE_OK) ? NO_ERR : ERR_1;
}


//遍历仪表信息, 对每一个表进行read_one_meter操作
void retrieve_meter_info_list(int (*read_one_meter)(pMeter_info))
{
	if(!read_one_meter)
		return;
	printf("now in  retrieve_meter_info_list():\n");
	pMeter_info pInfo_return = malloc(sizeof(struct meter_info_str));//回传一个独立的结构体, 保证原始数据的安全
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
 **	 函数名: read_sys_config
 ** 功能	: 从数据库中读取基本参数, 放到sys_config_array中
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
	char *col_buf[3] = {FIELD_BASE_DEF_ID, FIELD_BASE_DEF_NAME, FIELD_BASE_DEF_VALUE};
	int	col_cnt = 3;

	memset(sys_config_array, 0, SYS_CONFIG_COUNT*sizeof(sys_config_str));//清空原有配置
	config_idx = 0;

	//读取数据表内的配置
	get_select_sql(table_name, col_buf, col_cnt, sql_buf);
	get_orderby_sql(col_buf, 1, 0, order_buf);
	strcat(sql_buf, " ");
	strcat(sql_buf, order_buf);
	sqlite3_exec(g_pDB, sql_buf, each_config, NULL, &pErr);
	free(sql_buf);
	free(order_buf);
	sysConfig_Ascii2hex();
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
 * 清空系统配置信息
 * 注意: 此函数应该由调用者在向列表插入数据前调用一次,
 * 否则将出现错误(如将以前的信息插入到数据表中, 或者出现主键冲突)
 */
uint8 empty_sysconf_list()
{
    empty_list(pSys_config, list_set_sysconf)
	set_sysconf_idx = 0;
	return NO_ERR;
}

uint8 insert_sysconf(pSys_config pConf)
{
	pSys_config pConfig = malloc(sizeof(sys_config_str));//为了安全起见, 不直接使用传递进来的参数
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
	del_sysconf(NULL, pErr);//先清空原有的表数据
	while(pTmp_conf) {
		if(NO_ERR != add_one_config(pTmp_conf, pErr)) {
			return ERR_1;
		}
		pTmp_conf = pTmp_conf->pNext;
	}
	empty_sysconf_list();//清空设置列表

	
	//重新读取数据库里的信息
	read_sys_config(pErr);
	return (strlen(pErr) ? ERR_1 : NO_ERR );
}


/********************************************************************************
 **	 函数名: create_table
 ** 功能	: 创建数据表
 ********************************************************************************/
void create_table(char* table_name, char *pErr)
{
	
}

/********************************************************************************
 **	 函数名: get_delete_sql
 ** 功能	: 组合插入语句, 可以有where从句
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
		//警告: 所有数据都将删除, 请谨慎操作!
		
	}
	strcat(sql, ";");
}
/********************************************************************************
 **	 函数名: get_update_sql
 ** 功能	: 组合更新语句, 可以有where从句
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
 **	 函数名: get_insert_sql
 ** 功能	: 组合插入语句, 可以有where从句
 ** 由于列的数量必须与value的数量相等, 所以只输入列数量即可
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
 **	 函数名: get_query_sql
 ** 功能	: 组合查询语句, 可以有where从句
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
 **	 函数名: get_select_sql
 ** 功能	: 组合查询语句, 没有where从句
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
 **	 函数名: get_orderby_sql
 ** 功能	: 组合order by从句
 ** char **fields, 要进行排序的列名
 ** int f_cnt, 要进行排序的列数量
 ** int asc, 是否进行升序排序
 ** char *sql, 返回的sql字符串
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
 **	 函数名: get_where_sql
 ** 功能	: 组合where从句
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
