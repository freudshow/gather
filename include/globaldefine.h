/*
  *******************************************************************************
  * @file    globaldefine.h
  * @author  zjjin
  * @version V0.0.0
  * @date    02-24-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/ 

#ifndef _GLOBALDEFINE_H_
#define _GLOBALDEFINE_H_


//�������ͱ���
#define INT8     char
#define UINT8    unsigned char
#define INT16    short
#define UINT16   unsigned short
#define INT32    int
#define UINT32   unsigned int
typedef  char int8;
typedef  short int16;
typedef  int  int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


//�豸���Ͷ���
#define HEATMETER	0x20//�ȱ�
#define WATERMETER	0x10//ˮ��
#define ELECTMETER	0x40//���
#define GASMETER	0x30//ȼ����
#define SENSORDEV  0x50//�ɼ����ִ�������Rtuģ��

#define METER_TYPE_CNT 5//һ���ж����ֱ�

//�ֶγ��ȶ���
#define LENGTH_B_METER_ADDRESS	7//�ڳ����д�������Ǳ��ַ����, ���7�ֽ�, B����Byte
#define LENGTH_F_INSTALL_POS		50//�Ǳ�װλ�ó���
#define LENGTH_SYS_ANSWER       16384//ϵͳ����ص���Ϣ����

#define LENGTH_F_CONFIG_NAME		16//����������
#define LENGTH_F_CONFIG_VALUE		50//����ֵ����
#define LENGTH_F_METER_ADDRESS	    15//�����ݱ��д洢���Ǳ��ַ����, ���14���ַ�, ���Ͻ�����'\0', ����15���ַ�
#define LENGTH_F_METER_TYPE		2//�����ݱ��д洢���Ǳ����ͳ���, ���2���ַ�

/********************************************************************************
 ** �ַ��������
 ********************************************************************************/
#define LEN_BYTE	8//һ���ֽڵı�����
#define LEN_HALF_BYTE	4//һ���ֽڵı�����
#define BYTE_BCD_CNT	2//һ���ֽ��ɶ��ٸ�BCD���ʾ

#define LENGTH_MD5              50//MD5ֵ����󳤶�, һ��Ϊ32, ���ﶨ�Ĵ�һЩ


/*��λ����ѯ��λ��ȱ�ٵ�֡�����б���. ������λ���·��������֡��Ϊ1024֡,
 * ��Ϊ1~9, 9��1�ַ�; 10~99, 90��2�ַ�; 100~999, 900��3�ַ�; 1000~1024, 25��4�ַ�
 * ��������2989���ַ�, �ټ���ÿ�����������һ���ָ���(','����' '), ��1023��
 * �������ַ����Ⱦ���4012, �ո�����, 4096.
 * ��������������֡������1024, ��ô������֡������1024, �������������, �ټ�1,
 * �����������4096����, ������λ��������ô����ݴ��������治������xml֡
 */
#define LENGHTH_LACK_STRING     4096
/********************************************************************************
 ** MODBUS���
 ********************************************************************************/
#define MODBUS_READ    0x03//����·�Ĵ���
#define LC_ELEC_WORK_START  0x000C//������������������ʼ�Ĵ�����ַ
#define LC_ELEC_WORK_LEN    0x000C//���������������ļĴ�������



//�����豸�ļ�����
#define DEVICE4851		"/dev/ttyS1"
#define DEVICE4852   	"/dev/ttyS2"
#define DEVICEGPRS  	"/dev/ttyS3"
#define DEVICEMBUS  	"/dev/ttyS6"           

#define IO_CONTROL  "/dev/CONTROL_IO"

#define QMSG_FILE	"./"  //��Ϣ������


//���ݿ�����
#define SQLITE_NAME  "gatherdb.db"
#define APP_NAME    "gather_V0"
#define APP_TMPNAME "tmp_gather"

//IO�ڿ������
#define IOCTLTEST_MAGIC    0xA4

#define LED0_A30   _IO (IOCTLTEST_MAGIC, 0)
#define LED1_A31   _IO (IOCTLTEST_MAGIC, 1)
#define MBUS_C0    _IO (IOCTLTEST_MAGIC, 2)
#define MBUS_C1    _IO (IOCTLTEST_MAGIC, 3)
#define MBUS_C2    _IO (IOCTLTEST_MAGIC, 4)
#define MBUS_C3    _IO (IOCTLTEST_MAGIC, 5)
#define MBUS_C4    _IO (IOCTLTEST_MAGIC, 6)
#define MBUS_C5    _IO (IOCTLTEST_MAGIC, 7)

#define GPRS_SWITCH_C28   _IO (IOCTLTEST_MAGIC, 8)
#define GPRS_RESET_C29    _IO (IOCTLTEST_MAGIC, 9)

#define RS4851_C30    _IO (IOCTLTEST_MAGIC, 10)
#define RS4852_C31    _IO (IOCTLTEST_MAGIC, 11)

#define IOCTLTEST_MAXNR        12


//ͨѶ���ʺ궨��
#define RS485UP_COM_SPEED	19200 
#define GPRS_COM_SPEED		115200


//RS485 ���ͺͽ���
#define RS485_RECV  0
#define RS485_SED   1



#define FALSE 0xff 
#define TRUE  0x00 
#define OS_TICKS_PER_SEC     1000u   //Set the number of ticks in one second                        */
#define OS_ERR_NONE          0u


//������Ϣ�Ķ���
#define CONTROL_RET_FAIL 	-1
#define CONTROL_RET_SUC  	0
#define NO_ERR		0//�ɹ�
#define ERR_1			1//�ĵ�����
#define ERR_FF		0xff//��������

//��ʱʱ��궨��
#define REC_TIMEOUT_SEC  	2 	//��ʱʱ�� ��
#define REC_TIMEOUT_USEC  	0	//��ʱʱ�� ����

#define RS485UP_REC_TIMEOUT_SEC  	REC_TIMEOUT_SEC 	
#define RS485UP_REC_TIMEOUT_USEC  	REC_TIMEOUT_USEC	



#define METER_FRAME_LEN_MAX	256		//����֡��󳤶�, �����ݱ��뱣֤Ϊ2��N�η�

#define RS485_DOWN_CHANNEL	7		//�궨������485ͨ���š�

#define READ_METER_TIMES 2//�ظ��������






//COM�ڲ���ʹ�ýṹ��
typedef struct
{
    INT32 ByteBits;
    INT32 Speed;
    UINT8 Name[15];
}COM_INFO_T;

//��Ϣ��������Ϣ��
typedef struct{
	long int mtype;
	uint8 dev;
	uint8 functype;
    char timenode[30];
}QmsgType;



//����������Ϣ�ṹ�塣
typedef struct
{
	uint16 u16MeterID; //Hex
	uint8 u8MeterAddr[LENGTH_B_METER_ADDRESS];  //BCD��
	uint8 u8MeterType;	   //BCD
	uint8 u8ProtocolType;  //HEX
	uint8 u8Channel;	   //HEX
	char  install_pos[LENGTH_F_INSTALL_POS];  //�ַ���
}MeterFileType;

typedef enum{//д����ʷ����Э��ĵ�ǰ״̬
    stat_his_init=0,//��ʼ״̬, �ļ��հ�
    stat_his_trans,//�����������״̬
    stat_his_data,//row data״̬
    stat_his_end//����״̬
}wr_his_stat;

/*****************************************
 ** �Ǳ�����
 *****************************************/
#define MTYPE_CNT	4//���ص��Ǳ���������
enum meter_type_idx;
typedef enum meter_type_idx mtype_idx;
enum meter_type_idx{
	em_heat=0,//�ȱ�����
	em_water,//ˮ������
	em_elect,//�������
	em_gas//ȼ��������
};


#define LENGTH_ADDR	50//�������Ż�����λ��ip�ĵ�ַ����
#pragma pack(1)
typedef struct{
    uint8 sadd[LENGTH_ADDR];
    uint8 oadd[LENGTH_ADDR];
    uint8 func_type;
    uint8 oper_type;
    char timenode[30];
    xmlDocPtr xmldoc_rd;//����λ���·���xmlDoc
    xmlDocPtr xmldoc_wr;//��Ҫ����λ�����͵�xmlDoc
    uint8 xml_rd_file_idx;//��λ���·���xml�ļ�����
    uint8 xml_wr_file_idx;//Ҫд����ʷ���ݵ�xml�ļ�����

    

    int total_row[MTYPE_CNT];//��ǰ�����͹��ж���������
    int mod[MTYPE_CNT];//�ܹ������� mod ROW_PER_FRAME������, ���ڼ����ܹ���֡�������һ֡������, �Ǳ�������ص�
    int total_frame[MTYPE_CNT];//�ܹ���Ҫ������֡, �Ǳ�������ص�
    int cur_frame[MTYPE_CNT];//��ǰ�ڼ�֡, ��1��ʼ, �Ǳ�������ص�

    int cur_cnt;//��ǰ֡�м�������, ��������޹ص�
    int total_rows;//�ܹ�������, ��������޹ص�
    int cur_frame_indep;//��������޹ص�֡����
    int cur_rows;//�Ѿ������˶��������� if(read answer OK) cur_rows += cur_cnt;
    int cur_row_idx;//ÿһ֡���͵��е�����
    wr_his_stat cur_wr_state;//��ǰxml������״̬

    int meter_info_row_idx;

    int up_total_bytes;//ѹ���������ֽ���
    int up_total_frm;//ѹ��������֡��
    int up_cur_bytes;//��ǰ֡�������ֽ���
    int up_cur_frm_idx;//Զ������ʱ�ĵ�ǰ֡����
    char up_md5[LENGTH_F_CONFIG_VALUE];//��λ���·����³����MD5ֵ
    uint8 **pDataList;//�ݴ�ռ��ָ��
    int *pDataLen;//ÿһ֡�ݴ�ռ�Ĵ�С
} xml_info_str;
typedef xml_info_str* pXml_info;
#pragma pack()




typedef enum{//��,��λ��ֱ���·�function�����ͱ��
    em_FUNC_ID = 0,     //func_index -0-, ��¼
    em_FUNC_HEATBT,     //func_index -1-, ����
    em_FUNC_SYSCONF,    //func_index -2-, ϵͳ��������
    em_FUNC_RQDATA,     //func_index -3-, �Ǳ������������
    em_FUNC_TNODE,      //func_index -4-, �������ϱ�ʱ�������
    em_FUNC_MINFO,      //func_index -5-, ���ַ����
    em_FUNC_RPTUP,      //func_index -6-, �ϴ���ʷ����
    em_FUNC_RDSTAT,     //func_index -7-, ��ȡ������״̬
    em_FUNC_SWIP,       //func_index -8-, �л�ip
    em_FUNC_DBMANI,     //func_index -9-, ���ݿ�͸��
    em_FUNC_SYSCMD,     //func_index -10-, ����shell����͸��
    em_FUNC_CODEUP,     //func_index -11-, �������
    em_FUNC_PROTOTRS,   //func_index -12-, Э��͸��
    em_FUNC_CLOCK_SET,  //func_index -13-, ������Уʱ
    em_FUNC_COLLECT_SET //func_index -14-, ����������ģʽ�ͳ����������á�
}func_type_idx;



typedef enum{//����λ���������ͱ��
	em_OPER_RD=0,//��ȡ
	em_OPER_WR,//д��
	em_OPER_DO,//����
	em_OPER_ASW//Ӧ��
}oper_type_idx;

#define RPT_ACTIVE  0
#define RPT_PASSIVE 1

#define NETMOD_GPRS 0
#define NETMOD_RS485 0
#define NETMOD_ETH 0
#define NETMOD_GPRS 0

typedef enum em_net_mod{//������ʽ
    em_net_gprs   = 0,  //GPRS
    em_net_rs485,       //RS485
    em_net_eth,         //��̫��
    em_net_zigbee       //ZigBee
}net_mod;

typedef enum T_System_Config {
    CONFIG_PRIMARY_SERVER = 0,  //-0-, ��������IP
    CONFIG_PRIMARY_DNS,         //-1-, ��DNS IP
    CONFIG_PRIMARY_PORT,        //-2-, ���������˿�
    CONFIG_SECOND_SERVER,       //-3-, ��������IP
    CONFIG_SECOND_DNS,          //-4-, ��DNS IP
    CONFIG_SECOND_PORT,         //-5-, ���������˿�
    CONFIG_GATEWAY_ID,          //-6-, ���������
    CONFIG_NET_TYPE,            //-7-, �������ͣ�0-GPRS,1-485�ȡ�
    CONFIG_MD5_KEY,             //-8-, MD5��Կ
    CONFIG_AES_KEY,             //-9-, AES��Կ
    CONFIG_COLLECT_MODE,        //-10-, �Զ�����ʽ��0-����collect_cycle�������ó���1-���㳭��
    CONFIG_COLLECT_CYCLE,       //-11-, �Զ��ɼ����ڣ���COLLECT_MODEΪ0ʱ��Ч��
    CONFIG_REPORT_MODE,         //-12-, �����ϱ�ģʽ,0-�����ϱ���1-��������
    CONFIG_BEAT_CYCLE,          //-13-, �������ڣ���Χ1-10���ӡ�
    CONFIG_SVR_NUM,             //-14-, ���������ı��
    CONFIG_APP_MD5              //-15-, �����ļ���md5ֵ
}sys_config_idx;
#define SYS_CONFIG_COUNT	16//���������������

typedef enum item_idx_heat {//����CJ188Э��涨���ֶ���
    em_HColdE=1,        //����, ʵ�����ڽ���������
    em_HHeatE,          //�ۻ�����
    em_HPower,          //�ȹ���
    em_HFlowrate,       //����
    em_HAccumFlow,      //�ۻ�����
    em_HInTemp,         //��ˮ�¶�
    em_HOutTemp,        //��ˮ�¶�
    em_HAccumWorkTime,  //�ۻ�����ʱ��
    em_HCurTime,        //ʵʱʱ��
    em_HState           //״̬ST
}hItem_idx;

typedef enum item_idx_water {//ˮ��������
    em_WCurAccumFlow=1, //��ǰ�ۻ�����
    em_WFlowrate,       //����
    em_WSettleAccumFlow,//�������ۻ�����
    em_WSettleSate,     //������
    em_WAccumWorkTime   //�ۻ�����ʱ��
}wItem_idx;

typedef enum item_idx_elec {//���������
    em_EPActTotElec=1,	    //�����й��ܵ���
    em_ENActTotElec,	    //�����й��ܵ���
    em_EPReactTotElec,	    //�����޹��ܵ���
    em_ENReactTotElec,	    //�����޹��ܵ���
    em_EActTotElec,	    //�й��ܵ���
    em_EReactTotElec,	    //�޹��ܵ���
    em_EOvrPowFac,	    //�ܹ�������
    em_EPMaxDem	        //�����й����������
}eItem_idx;

#define PROTO_RES_LEN 512
typedef struct {//͸�����������Ϣ
    uint8 channel;      //ͨ����
    uint32 baud;        //������
    uint8 databits;     //����λ
    uint32 stop;        //ֹͣλ
    uint8 parity;      //У��λ
    uint8 cmd[256];     //͸������
    uint16 cmdlen;       //͸������ĳ���
    uint8 meter_type;    //�Ǳ�����
    uint8 proto_type;    //Э������
    uint8 res[PROTO_RES_LEN];     //͸���Ľ��
    uint16 resLen;
} proto_trans_str;
typedef proto_trans_str* pProto_trans;


#endif  //_GLOBALDEFINE_H_
