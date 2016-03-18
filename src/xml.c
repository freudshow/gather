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

XML_FILE gXML_File[XML_BUF_FILE_NUM];  //����xml�ļ���ر�����




/*
  ******************************************************************************
  * �������ƣ� uint8 XMLBuf_Init(void)
  * ˵    ���� ��ʼ��XML  BUF��ֻ������ϵͳ��ʼ��ʱ��ʼ��һ�Σ������в�������á�
  * ��    ���� ��
  ******************************************************************************
*/
static uint8 gsu8XmlBufInitFlag = 0;//���ڱ��XMLBuf_Init�Ƿ��Ѿ������ã���Ϊ�ú���ֻ�������һ�Ρ�
uint8 XMLBuf_Init(void)
{
	uint8 i = 0;
	int ret = 0; 

	if(gsu8XmlBufInitFlag != 0)  //����Ѿ������ã��ٴε����򷵻ش���
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

		//printf("i=%d.\n",i);  //���Ը����á�

	}

	gsu8XmlBufInitFlag = 1;

	return NO_ERR;

}

/*
  ******************************************************************************
  * �������ƣ� uint8 Get_XMLBuf(void)
  * ˵    ���� ���ڻ�ȡһ�����õ�XML Buf��š�
  * ��    ���� ���ؿ��õ�XML Buf��š�
  			�������0xff��˵����ǰû�п��õ�buf.
  ******************************************************************************
*/
uint8 Get_XMLBuf(void)
{
	uint8 i = 0;
	uint8 ret = ERR_FF;

	OS_ENTER_CRITICAL();
	for(i=0;i<XML_BUF_FILE_NUM;i++){
		if(gXML_File[i].usedflag == 0){
			gXML_File[i].usedflag = 1;  //���Ϊռ�á�
			ret = i;	
			break;
		}
	}
	OS_EXIT_CRITICAL();

	return ret;
}


/*
  ******************************************************************************
  * �������ƣ� uint8 Put_XMLBuf(uint8 lu8BufIndex)
  * ˵    ���� �����ͷ�һ���ѱ�ռ�õ�XML Buf��š�
  * ��    ���� �����ͷ��ͷųɹ���
  			�������0xff��˵������������ޡ�
  ******************************************************************************
*/
uint8 Put_XMLBuf(uint8 lu8BufIndex)
{

	if(lu8BufIndex >= XML_BUF_FILE_NUM) //���޼�飬��ֹ����
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
  * �������ƣ� makexml
  * ˵    ���� ����xml�ļ�
  * ��    ���� 
  *      		optype--��������
  ******************************************************************************
*/
uint8 makexml(XmlInfoRecord *xmlInfo,uint8 xmlIndex)
{
	FILE *fp;
	int nRel;
	//�����ĵ���ָ��
	xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");
	//�½��ڵ�
    	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");
    	//�����½ڵ�Ϊ���ڵ�
    	xmlDocSetRootElement(doc,root_node);
    	//����һ���½ڵ�common,��ӵ����ڵ�
    	xmlNodePtr node = xmlNewNode(NULL,BAD_CAST "common");
	xmlAddChild(root_node,node);

	fp = fopen(gXML_File[xmlIndex].pXMLFile,"w+");
	
	
	switch(xmlInfo->FuncType){
		case ID_VALIDATE:
			//��common�ڵ�ֱ�Ӵ����ı��ڵ�
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
			//��common�ڵ�ֱ�Ӵ����ı��ڵ�
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
  * �������ƣ� uint8 XmlInfo_Analyze(uint8 Dev, uint8 XmlIndex)
  * ˵    ���� �������յ���xml��Ϣ��
  * ��    ���� uint8 Dev ��xml��Ϣ�����ĸ��豸��
  *      		uint8 XmlIndex ��xml��Ϣ��������š�
  ******************************************************************************
*/

uint8 XmlInfo_Analyze(uint8 Dev, uint8 XmlIndex)
{
	if(Dev == UP_COMMU_DEV_GPRS)
		UpdGprsRunSta_FeedSndDog();  //���κ����������յ�������Ϊ�������������GPRS�������������������������������գ���GPRS������




	//����Ӧ�ã��ȼ���ǲ��Ƿ��͸��������������ݣ��������������ǵĻ��ż�����


	//����ֻ�ǲ����ã�����sem_post(&Validate_sem);Ӧ���ڼ�⵽��¼��Ӧʱ��ִ�С�
	sem_post(&Validate_sem);





	return NO_ERR;
}



/*
  ******************************************************************************
  * �������ƣ� uint8 XmlInfo_Exec(uint8 Dev, uint8 XmlIndex)
  * ˵    ���� ���ݽ���������Ϣ��ִ��xml����
  * ��    ���� uint8 Dev ��xml��Ϣȥ���ĸ��豸��
  *      		uint8 XmlIndex ��xml��Ϣ��������š�
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
 * �������ƣ� parse_xml
 * ˵	  ���� ������ִ��xml�ļ�
 ******************************************************************************
 */

uint8 func_id(uint8 oper_idx)
{
		uint8 retErr = NO_ERR;
		//����ֻ�ǲ����ã�����sem_post(&Validate_sem);Ӧ���ڼ�⵽��¼��Ӧʱ��ִ�С�
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
func_id,//��¼
func_heart_beat,//����
func_sysconfig,//ϵͳ��������
func_rqdata,//�Ǳ������������
func_tnode,//�������ϱ�ʱ�������
func_minfo,//���ַ����
func_rptup,//�ϴ���ʷ����
func_rdstat,//��ȡ������״̬
func_swip,//�л�ip
func_dbmani,//���ݿ�͸��
func_syscmd,//����shell����͸��
func_codeup,//�������
func_prototrs//Э��͸��
};
xml_info_str g_xml_info;

uint8 parse_common(xmlNodePtr root_node);

uint8 parse_xml(char *docname, int Dev)
{
	if(Dev == UP_COMMU_DEV_GPRS)
			UpdGprsRunSta_FeedSndDog();  //���κ����������յ�������Ϊ�������������GPRS�������������������������������գ���GPRS������
	

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
		else{//�쳣���
			retErr = NO_ERR;
		}
		cur_node=cur_node->next;
	}
	//����Ӧ�ã��ȼ���ǲ��Ƿ��͸��������������ݣ��������������ǵĻ��ż�����
	retErr = (*xml_exec_array[g_xml_info.func_type])(g_xml_info.oper_type);
	printf((retErr==NO_ERR)?"success\n":"fail\n");
	return retErr;
}
