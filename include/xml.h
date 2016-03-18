/*
  *******************************************************************************
  * @file    xml.h
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

#ifndef _XML_H_
#define _XML_H_

#include "includes.h"



#define  XML_BUF_FILE_NUM   15




typedef struct {
	uint8   usedflag;  //标记是否被占用，0-未占用，1-占用。
    	sem_t   sem_read;
    	sem_t   sem_write;
    	char *pXMLFile;    
} XML_FILE;





/*
//采集器发送标志
#define  REQUEST    0
#define  MD5_XML        1
#define  NOTIFY     2
#define  REPLY      3
#define  REPORT     4
#define  CONTINUOUS    5
#define  PERIOD_ACK    6
#define  IP_PORT_ACK   7
#define  BASIC_DB_REP  8
#define  PACK_LOST_ACK 9
#define  REPLY_CONFIG  10
#define  ID_ACK        11
#define  REPORT_ACK    12
#define  KEY_ACK       13
#define  CONTROL_ACK   14
#define  MODIFY_ACK        15
#define  REALTIME_ACK      16
#define   QUERY_REQ_ACK  17
#define  TRANS_ACK           18


//采集器接收标志
#define  SEQUENCE        20
#define  RESULT          21
#define  TIME            22
#define  QUERY           23
#define  CONTINUOUS_ACK  24
#define  PERIOD          25
#define  IPPORT          26
#define  BASIC_DB        27
#define  FILE_TRANS      28
#define  PACK_LOST       29
#define  QUERY_CONFIG    30
#define  ID_CONFIG       31
#define  REPORT_MODE     32
#define  MD5_KEY         33
#define  AES_KEY         34
#define  REBOOT          35
#define  MODIFY          36
#define  REALTIME_QRY 37
#define Query_req        38
#define TRANS_REQ         39

#define  NONE            99
*/

#define CONST_CAST (const xmlChar *)

#define XML_BEGIN			"<?xml"
#define NODE_ROOT			"root"
#define NODE_COMMON			"common"
#define NODE_SRC_ADDR		"sadd"
#define NODE_OBJ_ADDR		"oadd"
#define NODE_FUNC_TYPE		"func_type"
#define NODE_OPER_TYPE		"oper_type"

extern char *pXMLFileName[XML_BUF_FILE_NUM];
extern XML_FILE gXML_File[XML_BUF_FILE_NUM];





extern uint8 XMLBuf_Init(void);
extern uint8 Get_XMLBuf(void);
extern uint8 Put_XMLBuf(uint8 lu8BufIndex);
extern uint8 UpGetXMLEnd(uint8 XmlIndex,uint8 dev, uint32 OutTime);
extern uint8 UpGetXMLStart(uint8 XmlIndex,uint8 dev, uint32 OutTime);

extern uint8 makexml(xml_info_str *xmlInfo,uint8 xmlIndex);
//extern uint8 XmlInfo_Analyze(uint8 Dev, uint8 XmlIndex);
//extern uint8 XmlInfo_Exec(uint8 Dev, uint8 XmlIndex);
extern uint8 setXmlInfo(uint8 dev, pXml_info pXml_info);
extern uint8 getXmlInfo(uint8 dev,pXml_info pXml_info);
extern uint8 parse_xml(uint8 dev, uint8 xml_idx);
extern uint8 xml_exec(uint8 dev, uint8 xml_idx);


typedef enum{//上,下位机直接下发function的类型编号
	em_FUNC_ID=0,//登录
	em_FUNC_HEATBT,//心跳
	em_FUNC_SYSCONF,//系统参数配置
	em_FUNC_RQDATA,//仪表的数据项配置
	em_FUNC_TNODE,//抄表与上报时间点配置
	em_FUNC_MINFO,//表地址配置
	em_FUNC_RPTUP,//上传历史数据
	em_FUNC_RDSTAT,//读取集中器状态
	em_FUNC_SWIP,//切换ip
	em_FUNC_DBMANI,//数据库透传
	em_FUNC_SYSCMD,//本地shell命令透传
	em_FUNC_CODEUP,//程序更新
	em_FUNC_PROTOTRS//协议透传
}func_type_idx;

typedef enum{
	em_OPER_RD=0,//读取
	em_OPER_WR,//写入
	em_OPER_DO,//操作
	em_OPER_ASW//应答
}oper_type_idx;


#endif  //_XML_H_
