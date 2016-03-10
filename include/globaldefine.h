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





//�����豸�ļ�����
#define DEVICE4851		"/dev/ttyS1"
#define DEVICE4852   	"/dev/ttyS2"
#define DEVICEGPRS  	"/dev/ttyS3"
#define DEVICEMBUS  	"/dev/ttyS6"           

#define IO_CONTROL  "/dev/CONTROL_IO"


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



#define FALSE -1 
#define TRUE 0 
#define OS_TICKS_PER_SEC     1000u   //Set the number of ticks in one second                        */
#define OS_ERR_NONE          0u


//������Ϣ�Ķ���
#define CONTROL_RET_FAIL 	-1
#define CONTROL_RET_SUC  	0
#define NO_ERR		0
#define ERR_1		1
#define ERR_FF		0xff


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













#endif  //_GLOBALDEFINE_H_

