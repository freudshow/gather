/*
  *******************************************************************************
  * @file    gprs.h
  * @author  zjjin
  * @version V0.0.0
  * @date    03-14-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/  

#ifndef _GPRS_H_
#define _GPRS_H_

#include "includes.h"





typedef struct {
	uint8  ModuId;//模块类型
	uint8  Ready;//模块是否准备好，TRUE or FALSE
	uint8  Connect;//模块是否成功连接上位机，TRUE or FALSE
	uint8  SocketType;//0:TCP;1:UDP
	uint8  IpCloseNum;//TRUE or FALSE 
	uint8  IpSendFailNum;
	uint8  IpSendRemnBufNum;//上一次IPSEND时GPRS模块的剩余缓冲区数量
	uint8  Csq;//信号强度
	
	uint32 FrmRecTimes;//接收帧次数 //测试观察用
	uint32 FrmSndTimes;//发送帧次数 //测试观察用
	
	uint32 FrmRecBs;//接收字节数
	uint32 FrmSndBs;//发送字节数
	
	uint32  RecDog; //监视GPRS接收的看门狗
	uint32  SndDog; //监视GPRS发送的看门狗
} GPRS_RUN_STA;

typedef struct {
	uint8   GprsGvc;		// TRUE OR  FALSE，为TRUE时，表示对GPRS的管理，下面几个参数无效
    uint8   Device;			// 发送数据的设备，GPRS, Zigbee, RS485
    uint16  ByteNum;		// 发送字节数
    uint8   buff[1024];		//发送数据
} UP_COMM_MSG;



#define MODU_ID_UNKNOWN		0x55
#define MODU_ID_HW			0x12
#define MODU_ID_XMZ			0x21


#define	HEAD_MAX_LEN    64

#define	SOCKET_TCP	 0
#define	SOCKET_UDP	 1

#define  GPRS_CHECK_CYCLE	 	10 //单位秒。
#define  GPRS_SELF_CHECK_CYCLE	 5*GPRS_CHECK_CYCLE
#define  GPRS_HEART_FRM_TIME		60
#define  GPRS_HEART_FRM_REC_OUTTIME	GPRS_HEART_FRM_TIME*4







#define MAX_IPSEND_GTM900C_BYTENUM 1024  //GTM900C一次发送(AT%IPSEND)最多为1024字节(引号里是2048个可显字符)
#define MAX_IPDATA_GTM900C_BYTENUM	2048  
//测试先是发现900C接收到IPDATA后发过来的最大包是1072字节(多了会按1072拆成多个IPDATA发过来),但后来又发现会出现大于1072的情况
//所以这个值搞不清,字节定义了一个大值,原来900B好像是按1400拆包的

#define MAX_IPSEND_MC52i_BYTENUM	1500  //MC52i一次发送(AT^SISW)最多为1500字节,二进制格式发送
#define MAX_REC_MC52i_BYTENUM		1500  //MC52i一次最多可读来自网络的数据也是1500字节,不过它存在自己一个比1500大的多得缓冲区里,等CPU去要







extern UP_COMM_MSG gUpCommMsg;
extern uint8 gGprsFirst;
extern GPRS_RUN_STA gGprsRunSta;



extern int pthread_GPRS_Mana(void);





#endif  //_GPRS_H_

