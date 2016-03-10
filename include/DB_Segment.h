#ifndef __DB_SEGMENT_H
#define __DB_SEGMENT_H

/********************************************************************************
**			数据库读取时，callback 回调函数参数定义
**	callback(void *NotUsed, int argc, char **argv, char **azColName)	
**  int argc --- 共有多少个字段
**	char **argv --- 指向各个字段指针数组的指针，*argv 则为某字段数据开始地址
**	char **azColName --- 指向各个字段名称指针数组的指针，
**        *azColName 则为某字段名称的开始地址
**
********************************************************************************/


/********************************************************************************
**							System_Config_Table
** 
**
** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
**
********************************************************************************/

enum DB_System_Config
{
	geSC_FirstIP	=	0,	//从0开始排序。
	geSC_FirstDNSIP,
	geSC_FirstPort,
	geSC_SecondIP,	//从0开始排序。
	geSC_SecondDNSIP,
	geSC_SecondPort,
	geSC_GatewayID,	//集中器自身ID编号。
	geSC_MD5Key,
	geSC_AESKey,
	geSC_NetType,		//网络类型。
	geSC_ReportMode,	//数据上报模式。
	geSC_CollectCyc,	//数据采集周期设置。
	geSC_CollectMode,	//自动采集模式，按照采集周期采集/按照定时时间采集。
	geSC_HeartCyc,		//心跳周期。

};


/********************************************************************************
**							Meter_Info_Table
** 
**
** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
**
********************************************************************************/

enum DB_Meter_Info
{
	geMI_MeterID	=	0,	//从0开始
	geMI_MeterAddr,
	geMI_MeterType,
	geMI_ProtocolType,
	geMI_ChannelNum,
	geMI_PositionInfo

};


/********************************************************************************
**							Request_Data_Table
** 
**
** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
**
********************************************************************************/

enum DB_Request_Data
{
	geRD_MeterType	=	0,	//从0开始
	geRD_DataIndex,
	geRD_DataName,

};


/********************************************************************************
**							Heat_Meter_Table
**
** 
**
** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
**
********************************************************************************/

enum DB_Heat_Meter
{
	geHM_MeterID	=	0,	//从0开始
	geHM_MeterAddr,
	geHM_MeterType,
	geHM_TimeNode,
	geHM_ReadTime,
	geHM_Remark,	//抄表备注，正常、失败、短路等等。
	
};




/********************************************************************************
**							Electric_Meter_Table
**
** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
**
********************************************************************************/

enum DB_Electric_Meter
{
	geEM_MeterID	=	0,	//从0开始
	geEM_MeterAddr,
	geEM_MeterType,
	geEM_TimeNode,
	geEM_ReadTime,
	geEM_Remark,	//抄表备注，正常、失败、短路等等。

};




/********************************************************************************
**							 Water_Meter_Table
**
** 
**
** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
**
********************************************************************************/

enum DB_Water_Meter
{
	geWM_MeterID	=	0,	//从0开始
	geWM_MeterAddr,
	geWM_MeterType,
	geWM_TimeNode,
	geWM_ReadTime,
	geWM_Remark,	//抄表备注，正常、失败、短路等等。

};



/********************************************************************************
**							 Time_Node_Table
**
** 
**
** 	给每个字段定义枚举数据，在数据库读取时根据枚举数据直接取该字段数据
**
********************************************************************************/

enum DB_Time_Node
{
	geTN_TimeNode	=	0,	//从0开始

};








typedef struct
{
	char Str_Building_id[20];
	char Str_Gateway_id[20];
	char Str_Report_mode[8];
	char Str_Period[8];
	char Str_Md5_key[40];
	char Str_Aes_key[40];
	char Str_Ip_addr[24];
	char Str_Port[16];
	char Str_Dns[100];
	char Str_Back_ip_addr[24];
	char Str_Back_port[16];
	char Str_Back_dns[100];
	
}System_ConfigType;

#endif
