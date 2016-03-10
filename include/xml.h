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






extern char *pXMLFileName[XML_BUF_FILE_NUM];
extern XML_FILE gXML_File[XML_BUF_FILE_NUM];





extern uint8 XMLBuf_Init(void);
extern uint8 Get_XMLBuf(void);
extern uint8 Put_XMLBuf(uint8 lu8BufIndex);
extern uint8 UpGetXMLEnd(uint8 XmlIndex,uint8 dev, uint32 OutTime);
extern uint8 UpGetXMLStart(uint8 XmlIndex,uint8 dev, uint32 OutTime);













#endif  //_XML_H_

  

