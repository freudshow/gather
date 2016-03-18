/*
  *******************************************************************************
  * @file    xml.c
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

#include "includes.h"
#include "queue.h"
#include "commap.h"


char *pXMLFileName[XML_BUF_FILE_NUM]={"buff0.xml","buff1.xml","buff2.xml","buff3.xml","buff4.xml","buff5.xml",
 		"buff6.xml","buff7.xml","buff8.xml","buff9.xml","buff10.xml","buff11.xml","buff12.xml",
 		"buff13.xml","buff14.xml"};

XML_FILE gXML_File[XML_BUF_FILE_NUM];  //定义xml文件相关变量。




/*
  ******************************************************************************
  * 函数名称： uint8 XMLBuf_Init(void)
  * 说    明： 初始化XML  BUF，只允许在系统初始化时初始化一次，运行中不允许调用。
  * 参    数： 无
  ******************************************************************************
*/
static uint8 gsu8XmlBufInitFlag = 0;//用于标记XMLBuf_Init是否已经被调用，因为该函数只允许调用一次。
uint8 XMLBuf_Init(void)
{
	uint8 i = 0;
	int ret = 0; 

	if(gsu8XmlBufInitFlag != 0)  //如果已经被调用，再次调用则返回错误。
		return ERR_FF;

	for(i=0;i<XML_BUF_FILE_NUM;i++){
		gXML_File[i].usedflag = 0;

		ret = sem_init(&(gXML_File[i].sem_read),0,1);
		if(ret != 0){
	   		perror("sem_init\n");
		}

		ret = sem_init(&(gXML_File[i].sem_write),0,1);
		if(ret != 0){
	   		perror("sem_init\n");
		}

		gXML_File[i].pXMLFile = pXMLFileName[i];

		//printf("i=%d.\n",i);  //调试跟踪用。

	}

	gsu8XmlBufInitFlag = 1;

	return NO_ERR;

}

/*
  ******************************************************************************
  * 函数名称： uint8 Get_XMLBuf(void)
  * 说    明： 用于获取一个可用的XML Buf序号。
  * 参    数： 返回可用的XML Buf序号。
  			如果返回0xff，说明当前没有可用的buf.
  ******************************************************************************
*/
uint8 Get_XMLBuf(void)
{
	uint8 i = 0;
	uint8 ret = ERR_FF;

	OS_ENTER_CRITICAL();
	for(i=0;i<XML_BUF_FILE_NUM;i++){
		if(gXML_File[i].usedflag == 0){
			gXML_File[i].usedflag = 1;  //标记为占用。
			ret = i;	
			break;
		}
	}
	OS_EXIT_CRITICAL();

	return ret;
}


/*
  ******************************************************************************
  * 函数名称： uint8 Put_XMLBuf(uint8 lu8BufIndex)
  * 说    明： 用于释放一个已被占用的XML Buf序号。
  * 参    数： 返回释放释放成功。
  			如果返回0xff，说明输入参数超限。
  ******************************************************************************
*/
uint8 Put_XMLBuf(uint8 lu8BufIndex)
{

	if(lu8BufIndex >= XML_BUF_FILE_NUM) //超限检查，防止错误。
		return ERR_FF;

	OS_ENTER_CRITICAL();
	gXML_File[lu8BufIndex].usedflag = 0;
	OS_EXIT_CRITICAL();

	return NO_ERR;

}
		
		
		
		
uint8 UpGetXMLStart(uint8 XmlIndex,uint8 dev, uint32 OutTime)
{
	uint8 err = 0x00;
	int8  XMLHead[] ="<?xml version";//=\"1.0\" encoding=\"utf-8\" ?>";
	uint8 DataTemp = 0x00;
	uint8 Flag = 0x01;
	uint32 i;
	uint32 lu32tmp = 0;
	FILE *fp;

	lu32tmp = strlen(XMLHead);
	while(Flag){		
		for(i=0;i<lu32tmp;i++){
			while(QueueRead(&DataTemp, (void*)pQueues[dev]) != QUEUE_OK){
				OSSemPend(dev, OutTime, &err);
				if(err != OS_ERR_NONE){
					//printf("[%s][%s][%d]OSSemPend err=%d\n",FILE_LINE,err);
					return err;
				}
			}
			   
			if(DataTemp == XMLHead[i])
			{;}
			else{
				i=0;
				continue;
			}
		}
		
		Flag = 0;
	}
	
	printf("start fopen %s . \n", gXML_File[XmlIndex].pXMLFile);
	fp=fopen(gXML_File[XmlIndex].pXMLFile,"w+");
	
	sem_wait(&gXML_File[XmlIndex].sem_write);
	fwrite(XMLHead,1,strlen(XMLHead),fp);
	sem_post(&gXML_File[XmlIndex].sem_write);
	fclose(fp);
	
	return NO_ERR;
	
}



		
		
uint8 UpGetXMLEnd(uint8 XmlIndex,uint8 dev, uint32 OutTime)
{
	uint8 err = 0x00;
	int8  XMLEnd[] ="</root>";	// </test>
	uint8 DataTemp = 0x00;
	uint8 Flag = 0x01;
	uint32 i;
	int j = 0;
	uint32 lu32tmp = 0;
	FILE *fp;

	lu32tmp = strlen(XMLEnd);
	while(Flag){		
		for(i=0;i<lu32tmp;i++){
			while(QueueRead(&DataTemp, (void*)pQueues[dev]) != QUEUE_OK){
				OSSemPend(dev, OutTime, &err);
				if(err != OS_ERR_NONE){
					printf("[%s][%s][%d]OSSemPend err=%d\n",FILE_LINE,err);
					return err;
				}
			}
			
			*(pRecvTempBuf[dev] + j++) = DataTemp;
			
			if(j == (RECV_TEMPBUF_SIZE - 1)){
				fp = fopen(gXML_File[XmlIndex].pXMLFile,"a+");
				debug_info(gDebugModule[XML_MODULE], "xml name is %s\n", gXML_File[XmlIndex].pXMLFile);
				sem_wait(&gXML_File[XmlIndex].sem_write);
				fwrite(pRecvTempBuf[dev],1,RECV_TEMPBUF_SIZE,fp);
				sem_post(&gXML_File[XmlIndex].sem_write);
				debug_err(gDebugModule[XML_MODULE], "[%s][%s][%d] fwrite Lens=%d\n",FILE_LINE,RECV_TEMPBUF_SIZE);
				memset(pRecvTempBuf[dev],0,RECV_TEMPBUF_SIZE);
				j=0;
				fclose(fp);
			}
			
			if(DataTemp == XMLEnd[i])
			{;}
			else{
				i=0;
				continue;
			}
		   
		}
		
		Flag = 0;
	}
	
	if(j != 0){
		fp=fopen(gXML_File[XmlIndex].pXMLFile,"a+");
		debug_info(gDebugModule[XML_MODULE], "xml name is %s\n", gXML_File[XmlIndex].pXMLFile);
		sem_wait(&gXML_File[XmlIndex].sem_write);
		fwrite(pRecvTempBuf[dev],1,j,fp);
		sem_post(&gXML_File[XmlIndex].sem_write);
		debug_err(gDebugModule[XML_MODULE], "[%s][%s][%d] fwrite Lens=%d\n",FILE_LINE,RECV_TEMPBUF_SIZE);
		memset(pRecvTempBuf[dev],0,RECV_TEMPBUF_SIZE);
		j=0;
		fclose(fp);
	}

	
	return NO_ERR;

	
}




/*
  ******************************************************************************
  * 函数名称： makexml
  * 说    明： 生成xml文件
  * 参    数： 
  *      		optype--操作类型
  ******************************************************************************
*/
uint8 makexml(XmlInfoRecord *xmlInfo,uint8 xmlIndex)
{
	FILE *fp;
	int nRel;
	//定义文档和指针
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	//新建节点
    	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");
    	//设置新节点为根节点
    	xmlDocSetRootElement(doc,root_node);
    	//创建一个新节点common,添加到根节点
    	xmlNodePtr node = xmlNewNode(NULL,BAD_CAST "common");
	xmlAddChild(root_node,node);

	fp = fopen(gXML_File[xmlIndex].pXMLFile,"w+");
	
	
	switch(xmlInfo->FuncType){
		case ID_VALIDATE:
			//在common节点直接创建文本节点
    			xmlNewTextChild(node,NULL,BAD_CAST "sadd",(xmlChar *)"3706825001");
    			xmlNewTextChild(node,NULL,BAD_CAST "oadd",(xmlChar *)"3706820001");
			xmlNewTextChild(node,NULL,BAD_CAST "func_type",(xmlChar *)"id_validate");
			xmlNewTextChild(node,NULL,BAD_CAST "oper_type",(xmlChar *)"read");

    			//debug_info(gDebugModule[XML_MODULE], "[%s][%s][%d] optype is %d\n",FILE_LINE, optype);

			nRel = xmlSaveFileEnc(gXML_File[xmlIndex].pXMLFile,doc,"utf-8");
			fclose(fp);
			if(nRel != -1){
				xmlFreeDoc(doc);
				printf("make xml success.xml Index = %d.\n",xmlIndex);
				return NO_ERR;
			} 
			
			break;

		case HEART_BEAT:
			//在common节点直接创建文本节点
    			xmlNewTextChild(node,NULL,BAD_CAST "sadd",(xmlChar *)"3706825001");
    			xmlNewTextChild(node,NULL,BAD_CAST "oadd",(xmlChar *)"3706820001");
			xmlNewTextChild(node,NULL,BAD_CAST "func_type",(xmlChar *)"heart_beat");
			xmlNewTextChild(node,NULL,BAD_CAST "oper_type",(xmlChar *)"read");

    			//debug_info(gDebugModule[XML_MODULE], "[%s][%s][%d] optype is %d\n",FILE_LINE, optype);

			nRel = xmlSaveFileEnc(gXML_File[xmlIndex].pXMLFile,doc,"utf-8");
			fclose(fp);
			if(nRel != -1){
				xmlFreeDoc(doc);
				printf("make xml success.xml Index = %d.\n",xmlIndex);
				return NO_ERR;
			} 


			break;

		default:
			fclose(fp);
			break;
			


	}



  	return ERR_1;
}



/*
  ******************************************************************************
  * 函数名称： uint8 XmlInfo_Analyze(uint8 Dev, uint8 XmlIndex)
  * 说    明： 解析接收到的xml信息。
  * 参    数： uint8 Dev 此xml信息来自哪个设备。
  *      		uint8 XmlIndex 此xml信息存放索引号。
  ******************************************************************************
*/

uint8 XmlInfo_Analyze(uint8 Dev, uint8 XmlIndex)
{
	if(Dev == UP_COMMU_DEV_GPRS)
		UpdGprsRunSta_FeedSndDog();  //有任何网络数据收到，都认为网络正常，清空GPRS心跳计数。如果连续几次心跳都不清空，则GPRS重启。




	//正常应该，先检查是不是发送给本集中器的数据，不是则舍弃，是的话才继续。


	//下面只是测试用，正常sem_post(&Validate_sem);应该在检测到登录回应时才执行。
	sem_post(&Validate_sem);





	return NO_ERR;
}



/*
  ******************************************************************************
  * 函数名称： uint8 XmlInfo_Exec(uint8 Dev, uint8 XmlIndex)
  * 说    明： 根据解析到的信息，执行xml请求。
  * 参    数： uint8 Dev 此xml信息去往哪个设备。
  *      		uint8 XmlIndex 此xml信息存放索引号。
  ******************************************************************************
*/

uint8 XmlInfo_Exec(uint8 Dev, uint8 XmlIndex)
{
	uint8 err = 0;
	FILE *fp;
	XmlInfoRecord l_xmlInfo;
	



	err = getXmlInfo(Dev,&l_xmlInfo);
	if(err != NO_ERR){
		printf("XmlInfo_Exec getXmlInfo Err.\n");
		return err;
	}

	switch(l_xmlInfo.FuncType){
		case ID_VALIDATE:
			err = makexml(&l_xmlInfo,XmlIndex);
			if(err == NO_ERR){
				fp = fopen(gXML_File[XmlIndex].pXMLFile,"r");
				FileSend(Dev, fp);
				fclose(fp);
			}

			break;

		case HEART_BEAT:
			err = makexml(&l_xmlInfo,XmlIndex);
			if(err == NO_ERR){
				fp = fopen(gXML_File[XmlIndex].pXMLFile,"r");
				FileSend(Dev, fp);
				fclose(fp);
			}

			break;


		default:
			break;
			
	}
	


	return NO_ERR;
}


/*
 ******************************************************************************
 * 函数名称： parse_xml
 * 说	  明： 解析并执行xml文件
 ******************************************************************************
 */

uint8 func_id(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		//下面只是测试用，正常sem_post(&Validate_sem);应该在检测到登录回应时才执行。
		//sem_post(&Validate_sem);
	
		return retErr;
}

uint8 func_heart_beat(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;

		return retErr;
}

uint8 func_sysconfig(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_rqdata(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_tnode(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_minfo(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_rptup(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}


uint8 func_rdstat(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_swip(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_dbmani(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_syscmd(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_codeup(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_prototrs(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}


uint8 (*xml_exec_array[])(uint8) = {
func_id,//登录
func_heart_beat,//心跳
func_sysconfig,//系统参数配置
func_rqdata,//仪表的数据项配置
func_tnode,//抄表与上报时间点配置
func_minfo,//表地址配置
func_rptup,//上传历史数据
func_rdstat,//读取集中器状态
func_swip,//切换ip
func_dbmani,//数据库透传
func_syscmd,//本地shell命令透传
func_codeup,//程序更新
func_prototrs//协议透传
};
xml_info_str g_xml_info;

uint8 parse_common(xmlNodePtr root_node);

uint8 parse_xml(char *docname, int Dev)
{
	if(Dev == UP_COMMU_DEV_GPRS)
			UpdGprsRunSta_FeedSndDog();  //有任何网络数据收到，都认为网络正常，清空GPRS心跳计数。如果连续几次心跳都不清空，则GPRS重启。
	

	//sem_wait(fd's xml_info_str read semaphore)
	xmlDocPtr doc;
	xmlNodePtr cur;
	uint8 retErr;
	doc = xmlParseFile(docname);
	if(doc == NULL) {
		fprintf(stderr, "Document not open successfully. \n");
		//sem_post(fd's xml_info_str read semaphore)
		//Put_XMLBuf
		return ERR_FF;
	}
	g_xml_info.doc = doc;

	cur = xmlDocGetRootElement(doc);
	if(cur == NULL) {
		fprintf(stderr, "empty document\n");
		xmlFreeDoc(doc);
		//sem_post(fd's xml_info_str read semaphore)
		//Put_XMLBuf
		return ERR_FF;
	}

	if(!xmlStrEqual(cur->name, CONST_CAST NODE_ROOT)) {
		fprintf(stderr, "root node not found");
		xmlFreeDoc(doc);
		//sem_post(fd's xml_info_str read semaphore)
		//Put_XMLBuf
		return ERR_1;
	}

	cur = cur->xmlChildrenNode;
	while(cur != NULL){
		if(xmlStrEqual(cur->name, CONST_CAST NODE_COMMON)) {
			retErr = parse_common(cur);
			break;
		}
		cur = cur->next;
	}
	//sem_post(fd's xml_info_str read semaphore)
	//Put_XMLBuf
	xmlFreeDoc(doc);
	
	return retErr;
}

uint8 parse_common(xmlNodePtr common_node)
{
	uint8 retErr = NO_ERR;
	xmlNodePtr cur_node;
	xmlChar* pValue;
	cur_node = common_node->xmlChildrenNode;
	while(cur_node != NULL){
		printf("node name: %s\n", BAD_CAST cur_node->name);
		if(xmlStrEqual(cur_node->name, CONST_CAST NODE_SRC_ADDR)){
			pValue = xmlNodeGetContent(cur_node->xmlChildrenNode);
			strcpy((char*)g_xml_info.sadd, (char*)pValue);
			xmlFree(pValue);
		}
		else if(xmlStrEqual(cur_node->name, CONST_CAST NODE_OBJ_ADDR)){
			pValue = xmlNodeGetContent(cur_node->xmlChildrenNode);
			strcpy((char*)g_xml_info.oadd, (char*)pValue);
			xmlFree(pValue);
		}
		else if(xmlStrEqual(cur_node->name, CONST_CAST NODE_FUNC_TYPE)){
			pValue = xmlNodeGetContent(cur_node->xmlChildrenNode);
			g_xml_info.func_type = atoi((char*)pValue);
			xmlFree(pValue);
		}
		else if(xmlStrEqual(cur_node->name, CONST_CAST NODE_OPER_TYPE)){
			pValue = xmlNodeGetContent(cur_node->xmlChildrenNode);
			g_xml_info.oper_type = atoi((char*)pValue);
			xmlFree(pValue);
		}
		else{//异常情况
			retErr = NO_ERR;
		}
		cur_node=cur_node->next;
	}
	//正常应该，先检查是不是发送给本集中器的数据，不是则舍弃，是的话才继续。
	retErr = (*xml_exec_array[g_xml_info.func_type])(g_xml_info.oper_type);
	printf((retErr==NO_ERR)?"success\n":"fail\n");
	return retErr;
}
