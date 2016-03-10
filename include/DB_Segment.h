#ifndef __DB_SEGMENT_H
#define __DB_SEGMENT_H

/********************************************************************************
**			���ݿ��ȡʱ��callback �ص�������������
**	callback(void *NotUsed, int argc, char **argv, char **azColName)	
**  int argc --- ���ж��ٸ��ֶ�
**	char **argv --- ָ������ֶ�ָ�������ָ�룬*argv ��Ϊĳ�ֶ����ݿ�ʼ��ַ
**	char **azColName --- ָ������ֶ�����ָ�������ָ�룬
**        *azColName ��Ϊĳ�ֶ����ƵĿ�ʼ��ַ
**
********************************************************************************/


/********************************************************************************
**							System_Config_Table
** 
**
** 	��ÿ���ֶζ���ö�����ݣ������ݿ��ȡʱ����ö������ֱ��ȡ���ֶ�����
**
********************************************************************************/

enum DB_System_Config
{
	geSC_FirstIP	=	0,	//��0��ʼ����
	geSC_FirstDNSIP,
	geSC_FirstPort,
	geSC_SecondIP,	//��0��ʼ����
	geSC_SecondDNSIP,
	geSC_SecondPort,
	geSC_GatewayID,	//����������ID��š�
	geSC_MD5Key,
	geSC_AESKey,
	geSC_NetType,		//�������͡�
	geSC_ReportMode,	//�����ϱ�ģʽ��
	geSC_CollectCyc,	//���ݲɼ��������á�
	geSC_CollectMode,	//�Զ��ɼ�ģʽ�����ղɼ����ڲɼ�/���ն�ʱʱ��ɼ���
	geSC_HeartCyc,		//�������ڡ�

};


/********************************************************************************
**							Meter_Info_Table
** 
**
** 	��ÿ���ֶζ���ö�����ݣ������ݿ��ȡʱ����ö������ֱ��ȡ���ֶ�����
**
********************************************************************************/

enum DB_Meter_Info
{
	geMI_MeterID	=	0,	//��0��ʼ
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
** 	��ÿ���ֶζ���ö�����ݣ������ݿ��ȡʱ����ö������ֱ��ȡ���ֶ�����
**
********************************************************************************/

enum DB_Request_Data
{
	geRD_MeterType	=	0,	//��0��ʼ
	geRD_DataIndex,
	geRD_DataName,

};


/********************************************************************************
**							Heat_Meter_Table
**
** 
**
** 	��ÿ���ֶζ���ö�����ݣ������ݿ��ȡʱ����ö������ֱ��ȡ���ֶ�����
**
********************************************************************************/

enum DB_Heat_Meter
{
	geHM_MeterID	=	0,	//��0��ʼ
	geHM_MeterAddr,
	geHM_MeterType,
	geHM_TimeNode,
	geHM_ReadTime,
	geHM_Remark,	//����ע��������ʧ�ܡ���·�ȵȡ�
	
};




/********************************************************************************
**							Electric_Meter_Table
**
** 	��ÿ���ֶζ���ö�����ݣ������ݿ��ȡʱ����ö������ֱ��ȡ���ֶ�����
**
********************************************************************************/

enum DB_Electric_Meter
{
	geEM_MeterID	=	0,	//��0��ʼ
	geEM_MeterAddr,
	geEM_MeterType,
	geEM_TimeNode,
	geEM_ReadTime,
	geEM_Remark,	//����ע��������ʧ�ܡ���·�ȵȡ�

};




/********************************************************************************
**							 Water_Meter_Table
**
** 
**
** 	��ÿ���ֶζ���ö�����ݣ������ݿ��ȡʱ����ö������ֱ��ȡ���ֶ�����
**
********************************************************************************/

enum DB_Water_Meter
{
	geWM_MeterID	=	0,	//��0��ʼ
	geWM_MeterAddr,
	geWM_MeterType,
	geWM_TimeNode,
	geWM_ReadTime,
	geWM_Remark,	//����ע��������ʧ�ܡ���·�ȵȡ�

};



/********************************************************************************
**							 Time_Node_Table
**
** 
**
** 	��ÿ���ֶζ���ö�����ݣ������ݿ��ȡʱ����ö������ֱ��ȡ���ֶ�����
**
********************************************************************************/

enum DB_Time_Node
{
	geTN_TimeNode	=	0,	//��0��ʼ

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
