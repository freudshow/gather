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
#define HEATMETER	0x20
#define WATERMETER	0x10
#define ELECTMETER	0x40
#define GASMETER	0x30

//�ȱ���Ҫ������������
#define	HITEM_CUR_COLD_E			0x01//��ǰ����
#define	HITEM_CUR_HEAT_E			0x02//��ǰ����
#define	HITEM_HEAT_POWER			0x03//�ȹ���
#define	HITEM_FLOWRATE			0x04//����
#define	HITEM_ACCUM_FLOW			0x05//�ۻ�����
#define	HITEM_IN_TEMP				0x06//��ˮ�¶�
#define	HITEM_OUT_TEMP			0x07//��ˮ�¶�
#define	HITEM_ACCUM_WORK_TIME	    0x08//�ۼƹ���ʱ��
#define	HITEM_REAL_TIME			0x09//ʵʱʱ��
#define	HITEM_STATE				0x0a//״̬ST
//ˮ����Ҫ������������
#define	WITEM_CUR_ACCUM_FLOW			0x01//��ǰ�ۼ�����
#define	WITEM_FLOWRATE					0x02//����
#define	WITEM_SETTLE_DATE_ACCUM_FLOW	0x03//�������ۻ�����
#define	WITEM_SETTLE_DATE				0x04//������
#define	WITEM_ACCUM_WORK_TIME			0x05//�ۼƹ���ʱ��
//�����Ҫ������������
#define	EITEM_ACTIVE_TOTAL_ELECTRICITY	0x01//�й��ܵ���
#define	EITEM_CUR_VOLTAGE					0x02//��ǰ��ѹֵ
#define	EITEM_CUR_CURRENT					0x03//��ǰ����ֵ
#define	EITEM_CUR_ACTIVE_POWER				0x04//��ǰ�й�����ֵ
#define	EITEM_CUR_FREQ						0x05//��ǰƵ��ֵ
#define	EITEM_CUR_REACTIVE_POWER			0x06//��ǰ�޹�����ֵ
#define	EITEM_CUR_APPARENT_POWER			0x07//��ǰ���ڹ���ֵ
#define	EITEM_CUR_POWER_FACTOR				0x08//��ǰ��������ֵ

#define LENGTH_B_METER_ADDRESS	7//�ڳ����д�������Ǳ��ַ����, ���7�ֽ�, B����Byte
#define LENGTH_F_INSTALL_POS		50//�Ǳ�װλ�ó���

/********************************************************************************
 ** �ַ��������
 ********************************************************************************/
#define LEN_BYTE	8//һ���ֽڵı�����
#define LEN_HALF_BYTE	4//һ���ֽڵı�����
#define BYTE_BCD_CNT	2//һ���ֽ��ɶ��ٸ�BCD���ʾ

//�����豸�ļ�����
#define DEVICE4851		"/dev/ttyS1"
#define DEVICE4852   	"/dev/ttyS2"
#define DEVICEGPRS  	"/dev/ttyS3"
#define DEVICEMBUS  	"/dev/ttyS6"           

#define IO_CONTROL  "/dev/CONTROL_IO"

#define QMSG_FILE	"./"  //��Ϣ������


//���ݿ�����
#define SQLITE_NAME  "gatherdb.db"

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
    stat_his_result,//���״̬
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
} xml_info_str;
typedef xml_info_str* pXml_info;




typedef enum{//��,��λ��ֱ���·�function�����ͱ��
	em_FUNC_ID = 0,//��¼
	em_FUNC_HEATBT,//����
	em_FUNC_SYSCONF,//ϵͳ��������
	em_FUNC_RQDATA,//�Ǳ������������
	em_FUNC_TNODE,//�������ϱ�ʱ�������
	em_FUNC_MINFO,//���ַ����
	em_FUNC_RPTUP,//�ϴ���ʷ����
	em_FUNC_RDSTAT,//��ȡ������״̬
	em_FUNC_SWIP,//�л�ip
	em_FUNC_DBMANI,//���ݿ�͸��
	em_FUNC_SYSCMD,//����shell����͸��
	em_FUNC_CODEUP,//�������
	em_FUNC_PROTOTRS,//Э��͸��
	em_FUNC_CLOCK_SET//������Уʱ
}func_type_idx;

typedef enum{//����λ���������ͱ��
	em_OPER_RD=0,//��ȡ
	em_OPER_WR,//д��
	em_OPER_DO,//����
	em_OPER_ASW//Ӧ��
}oper_type_idx;


#endif  //_GLOBALDEFINE_H_
