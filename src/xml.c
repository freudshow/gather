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
static xml_info_str g_xml_info[UP_COMMU_DEV_ARRAY];
uint8 gu8Dev = -1;//�տ�ʼ��ָ���κ��豸��

uint8 func_id(uint8 dev, uint8 xml_idx);
uint8 func_heart_beat(uint8 dev, uint8 xml_idx);
uint8 func_sysconfig(uint8 dev, uint8 xml_idx);
uint8 func_rqdata(uint8 dev, uint8 xml_idx);
uint8 func_tnode(uint8 dev, uint8 xml_idx);
uint8 func_minfo(uint8 dev, uint8 xml_idx);
uint8 func_rptup(uint8 dev, uint8 xml_idx);
uint8 func_rdstat(uint8 dev, uint8 xml_idx);
uint8 func_swip(uint8 dev, uint8 xml_idx);
uint8 func_dbmani(uint8 dev, uint8 xml_idx);
uint8 func_syscmd(uint8 dev, uint8 xml_idx);
uint8 func_codeup(uint8 dev, uint8 xml_idx);
uint8 func_prototrs(uint8 dev, uint8 xml_idx);

static uint8 (*xml_exec_array[])(uint8 dev, uint8 xml_idx) = {
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
uint8 makexml(xml_info_str *xmlInfo,uint8 xmlIndex)
{
	sys_config_str sysConfig;
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
	
	
	switch(xmlInfo->func_type){
		case em_FUNC_ID:
			//��common�ڵ�ֱ�Ӵ����ı��ڵ�
			get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);
    			xmlNewTextChild(node,NULL,BAD_CAST "sadd",(xmlChar *)sysConfig.f_config_value);
    			xmlNewTextChild(node,NULL,BAD_CAST "oadd",(xmlChar *)sysConfig.f_config_value);  //Ŀ�ĵ�ַ�Ժ�ĳɷ�������ַ��
			xmlNewTextChild(node,NULL,BAD_CAST "func_type",(xmlChar *)"0");
			xmlNewTextChild(node,NULL,BAD_CAST "oper_type",(xmlChar *)"0");

    			//debug_info(gDebugModule[XML_MODULE], "[%s][%s][%d] optype is %d\n",FILE_LINE, optype);

			nRel = xmlSaveFileEnc(gXML_File[xmlIndex].pXMLFile,doc,"utf-8");
			fclose(fp);
			if(nRel != -1){
				xmlFreeDoc(doc);
				printf("make xml success.xml Index = %d.\n",xmlIndex);
				return NO_ERR;
			} 
			
			break;

		case em_FUNC_HEATBT:
			//��common�ڵ�ֱ�Ӵ����ı��ڵ�
			get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);
            xmlNewTextChild(node,NULL,BAD_CAST "sadd",(xmlChar *)sysConfig.f_config_value);
            xmlNewTextChild(node,NULL,BAD_CAST "oadd",(xmlChar *)sysConfig.f_config_value);//Ŀ�ĵ�ַ�Ժ�ĳɷ�������ַ��
			xmlNewTextChild(node,NULL,BAD_CAST "func_type",(xmlChar *)"1");
			xmlNewTextChild(node,NULL,BAD_CAST "oper_type",(xmlChar *)"0");

            //debug_info(gDebugModule[XML_MODULE], "[%s][%s][%d] optype is %d\n",FILE_LINE, optype);

			nRel = xmlSaveFileEnc(gXML_File[xmlIndex].pXMLFile,doc,"utf-8");
			fclose(fp);
			if(nRel != -1){
				xmlFreeDoc(doc);
				printf("make xml success.xml Index = %d.\n",xmlIndex);
				return NO_ERR;
			} 

			break;
        case em_FUNC_RPTUP:
            
		default:
			fclose(fp);
			break;
			


	}



  	return ERR_1;
}

/*
  ******************************************************************************
  * �������ƣ� uint8 setXmlInfo()
  * ˵    ���� �����豸�ţ�������Ӧ��gXmlInfo��Ϣ��
  * ��    ���� 
  ******************************************************************************
*/

uint8 setXmlInfo(uint8 dev, pXml_info pXml_info)
{
	if(dev >= UP_COMMU_DEV_ARRAY){
		printf("dev num error.\n");
		return ERR_1;
	}
	if(NULL == pXml_info) {
		printf("source info is NULL.\n");
		return ERR_1;
	}
	memcpy((uint8 *)&g_xml_info[dev],(uint8 *)pXml_info,sizeof(xml_info_str));
	return NO_ERR;
}

/*
  ******************************************************************************
  * �������ƣ� uint8 getXmlInfo(uint8 dev,xml_info_str *xmlInfo)
  * ˵    ���� �����豸��ţ���ȡ��Ӧ��xmlInfo��Ϣ��
  * ��    ���� 
  ******************************************************************************
*/

uint8 getXmlInfo(uint8 dev,xml_info_str *pXml_info)
{

	if(dev >= UP_COMMU_DEV_ARRAY){
		printf("dev num error.\n");
		return ERR_1;
	}

	memcpy((uint8 *)pXml_info,(uint8 *)&g_xml_info[dev],sizeof(xml_info_str));

	return NO_ERR;

}



/*
 ******************************************************************************
 * �������ƣ� parse_xml
 * ˵	  ���� ������ִ��xml�ļ�
 ******************************************************************************
 */

uint8 func_id(uint8 dev, uint8 xml_idx)
{
	uint8 err = NO_ERR;
	FILE *fp;
	xml_info_str l_xmlInfo;
	err = getXmlInfo(dev,&l_xmlInfo);
	if(err != NO_ERR){
		printf("XmlInfo_Exec getXmlInfo Err.\n");
		return err;
	}
	printf("now in func_id().\n");
	printf("g_xml_info[%d].func_type: %u, g_xml_info[%d].oper_type: %u\n", dev, g_xml_info[dev].func_type, dev, g_xml_info[dev].oper_type);
	switch(g_xml_info[dev].oper_type){
	case em_OPER_RD:
		err = makexml(&l_xmlInfo,xml_idx);
		if(err == NO_ERR){
			fp = fopen(gXML_File[xml_idx].pXMLFile,"r");
			FileSend(dev, fp);
			fclose(fp);
		}		
	case em_OPER_WR:
		break;
	case em_OPER_ASW:
		printf("have read func_id answer from server.\n");//���յ�Ӧ��, ��¼�ɹ�
		sem_post(&Validate_sem);
		break;
	default:
		err = ERR_FF;
	}
	return err;
}

uint8 func_heart_beat(uint8 dev, uint8 xml_idx)
{
	uint8 err = NO_ERR;
	FILE *fp;
	xml_info_str l_xmlInfo;
	
	err = getXmlInfo(dev,&l_xmlInfo);
	if(err != NO_ERR){
		printf("XmlInfo_Exec getXmlInfo Err.\n");
		return err;
	}
	printf("now in func_heart_beat().\n");
	printf("l_xmlInfo.func_type: %u, l_xmlInfo.oper_type: %u\n", l_xmlInfo.func_type, l_xmlInfo.oper_type);
	switch(l_xmlInfo.oper_type){
	case em_OPER_RD:
		printf("now make heart_beat xml.\n");
		err = makexml(&l_xmlInfo,xml_idx);
		if(err == NO_ERR){
			fp = fopen(gXML_File[xml_idx].pXMLFile,"r");
			FileSend(dev, fp);
			fclose(fp);
		}		
	case em_OPER_WR:
		break;
	case em_OPER_ASW:
		printf("have read heart_beat answer from server.\n");//���յ�Ӧ��, ���������ɹ�
		//��տ��Ź�
		UpdGprsRunSta_FeedSndDog();
		break;
	default:
		err = ERR_FF;
	}
	return err;
}

//3��������Ϣ(sys_config)
uint8 func_sysconfig(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}
//4. ������Ҫ��Щ������(request_data)
uint8 func_rqdata(uint8 dev, uint8 xml_idx)
{
	uint8 err = NO_ERR;
	
	return err;
}
//5. ���ó���ʱ���(time_node)
uint8 func_tnode(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 send_upmeter_answer(uint8 dev)
{
    uint8 err=NO_ERR;
    FILE *fp;
    int nRel;
    uint8 lu8xmlIndex;
    g_xml_info[dev].xmldoc_wr = xmlNewDoc(BAD_CAST"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");
    xmlDocSetRootElement(g_xml_info[dev].xmldoc_wr,root_node);
    xmlNodePtr common_node = xmlNewNode(NULL,BAD_CAST "common");
    xmlAddChild(root_node,common_node);
    sys_config_str sysConfig;
    get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);
    xmlNewTextChild(common_node,NULL,BAD_CAST "sadd",(xmlChar *)sysConfig.f_config_value);
    get_sys_config(CONFIG_PRIMARY_SERVER,&sysConfig);
    xmlNewTextChild(common_node,NULL,BAD_CAST "oadd",(xmlChar *)sysConfig.f_config_value);  //Ŀ�ĵ�ַ�Ժ�ĳɷ�������ַ��
    char str[100];
    sprintf(str, "%d", em_FUNC_MINFO);
    xmlNewTextChild(common_node,NULL,BAD_CAST "func_type",(xmlChar *)str);
    sprintf(str, "%d", em_OPER_ASW);
    xmlNewTextChild(common_node,NULL,BAD_CAST "oper_type",(xmlChar *)str);

    xmlNewTextChild(root_node,NULL,BAD_CAST "result",BAD_CAST "success");
    sprintf(str, "%d", g_xml_info[dev].cur_frame_indep);
    xmlNewTextChild(root_node,NULL,BAD_CAST "frame_idx",BAD_CAST str);

    do{//��ȡһ��xml�ݴ�ռ�,���һ��Ҫ�ͷŸÿռ䣬��ȡ-ʹ��-�ͷš�
        lu8xmlIndex = Get_XMLBuf();
    }while(lu8xmlIndex == ERR_FF);
    g_xml_info[dev].xml_wr_file_idx = lu8xmlIndex;
    printf("write send_upmeter_answer xml to file.\n");
    
    fp = fopen(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile,"w+");
    nRel = xmlSaveFileEnc(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile, g_xml_info[dev].xmldoc_wr,"utf-8");
    fclose(fp);
    if(nRel != -1){
        xmlFreeDoc(g_xml_info[dev].xmldoc_wr);
        printf("make xml %s Index = %d.\n", gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile, g_xml_info[dev].xml_wr_file_idx);
        return NO_ERR;
    }

    if(err == NO_ERR) {//�����ļ�
        fp = fopen(gXML_File[lu8xmlIndex].pXMLFile,"r");
        FileSend(dev, fp);
        fclose(fp);
    }    
    Put_XMLBuf(lu8xmlIndex);  //�ͷű�ռ�õ�xml�ݴ�.
    return err;
}

uint8 update_meter_info(uint8 dev)
{
    char pErr[100];
    pMeter_info pMInfo;
    xmlDocPtr pDoc = g_xml_info[dev].xmldoc_rd;
    xmlNodePtr rootNode = xmlDocGetRootElement(pDoc);
    xmlNodePtr curNode;
    xmlNodePtr transNode;
    xmlNodePtr rowNode;
    xmlChar* pValue;
    int total_rows;//��λ��һ��Ҫ�·�����������
    empty_meter_info_list();
    empty_meter_info_table(pErr);
    curNode = rootNode->children;
    while(curNode) {//�õ�trans�ڵ�
        if(xmlStrEqual(curNode->name, CONST_CAST "trans")) {
            transNode = curNode;
        }
        curNode = curNode->next;
    }

    curNode = transNode->children;
    while(curNode) {
        if(xmlStrEqual(curNode->name, CONST_CAST "frame_idx")) {
            pValue = xmlNodeGetContent(curNode->xmlChildrenNode);
            if(xmlStrEqual(CONST_CAST"1", CONST_CAST pValue)) {
                g_xml_info[dev].meter_info_row_idx = 0;//����ǵ�һ֡, �����������
            }
            g_xml_info[dev].cur_frame_indep = atoi((char*)pValue);
        }else if(xmlStrEqual(curNode->name, CONST_CAST "total_meter_num")) {
            total_rows = atoi((char*)xmlNodeGetContent(curNode->xmlChildrenNode));
        }
        curNode = curNode->next;
    }
    int length, j, low_idx;
    curNode = rootNode->children;
    while(curNode) {//�������row�ڵ�
        if (xmlStrEqual(curNode->name, CONST_CAST "row")) {
            rowNode = curNode->children;
            pMInfo = malloc(sizeof(struct meter_info_str));
            while(rowNode) {//��ȡ���ַ����ϸ��Ϣ
                if(xmlStrEqual(rowNode->name, CONST_CAST "f_meter_type")) {
                    pValue = xmlNodeGetContent(rowNode->xmlChildrenNode);
                    pMInfo->f_meter_type = (Ascii2Hex(pValue[0])<<LEN_HALF_BYTE|Ascii2Hex(pValue[1]));
                }else if(xmlStrEqual(rowNode->name, CONST_CAST "f_device_id")) {
                    pValue = xmlNodeGetContent(rowNode->xmlChildrenNode);
                    pMInfo->f_device_id = atoi((char*)pValue);
                }else if(xmlStrEqual(rowNode->name, CONST_CAST "f_meter_address")) {
                    pValue = xmlNodeGetContent(rowNode->xmlChildrenNode);
                    length = strlen((char*)pValue);
                    for (j=0; j<(length+1)/BYTE_BCD_CNT;j++) {
                    	low_idx = length-BYTE_BCD_CNT*j-2;
                    	pMInfo->f_meter_address[j] = \
                    		(((low_idx < 0) ? 0: Ascii2Hex(pValue[low_idx])) << LEN_HALF_BYTE | Ascii2Hex(pValue[low_idx+1]));
                    }
                }else if(xmlStrEqual(rowNode->name, CONST_CAST "f_meter_proto_type")) {
                    pValue = xmlNodeGetContent(rowNode->xmlChildrenNode);
                    pMInfo->f_meter_proto_type = atoi((char*)pValue);
                }else if(xmlStrEqual(rowNode->name, CONST_CAST "f_meter_channel")) {
                    pValue = xmlNodeGetContent(rowNode->xmlChildrenNode);
                    pMInfo->f_meter_channel = atoi((char*)pValue);
                }else if(xmlStrEqual(rowNode->name, CONST_CAST "f_install_pos")) {
                    pValue = xmlNodeGetContent(rowNode->xmlChildrenNode);
                    strcpy(pMInfo->f_install_pos, (char*)pValue);
                }
                rowNode = rowNode->next;
            }
            insert_one_meter_info(pMInfo);
            g_xml_info[dev].meter_info_row_idx++;
        }
    }
    xmlFreeDoc(pDoc);//�������, �ͷ�pDoc
    send_upmeter_answer(dev);
    if(total_rows == g_xml_info[dev].meter_info_row_idx) {//�����λ������, �������ݿ�
        insert_into_meter_info_table(pErr);
        read_meter_info(pErr);
    }
    return NO_ERR;
}

//6. �������ַ��Ϣ(meter_info)
uint8 func_minfo(uint8 dev, uint8 xml_idx)
{
	uint8 err = NO_ERR;

	switch(g_xml_info[dev].oper_type){
	case em_OPER_RD:
        break;
	case em_OPER_WR:
        update_meter_info(dev);
        break;
	case em_OPER_ASW:
        //printf("have read history report answer from server.\n");
        //sem_post(&minfo_asw_sem);
        UpdGprsRunSta_FeedSndDog();
        break;
	default:
		err = ERR_FF;
	}
	return err;
}

uint8 get_tm_node(uint8 dev, char* timenode) {
    xmlDocPtr pDoc = g_xml_info[dev].xmldoc_rd;
    xmlNodePtr curNode = xmlDocGetRootElement(pDoc);//<root>node
    curNode = curNode->children;//<common>node
    while(curNode) {
        if(xmlStrEqual(curNode->name, CONST_CAST "time")) {
            strcpy(timenode, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
        }
        curNode = curNode->next;
    }
	xmlFreeDoc(pDoc);//when have getted timenode, free server's xmlDoc
    return NO_ERR;
}

//��xmldoc�����һ�е����ݽڵ�
int wr_his_xml(pHis_data pHis, uint8 dev)
{
    printf("now in wr_his_xml()\n");
    FILE *fp;
    int nRel;
    sys_config_str sysConfig;
    char str[100];
    xmlNodePtr root_node;
    xmlNodePtr common_node;
    xmlNodePtr trans_node;
    xmlNodePtr row_node;
    int val_idx=0;//��ʷ�����������
    printf("current state: %d\n", g_xml_info[dev].cur_wr_state);
    switch (g_xml_info[dev].cur_wr_state) {
    case stat_his_init:
        g_xml_info[dev].xmldoc_wr = xmlNewDoc(BAD_CAST"1.0");
        root_node = xmlNewNode(NULL,BAD_CAST"root");
        xmlDocSetRootElement(g_xml_info[dev].xmldoc_wr,root_node);
        common_node = xmlNewNode(NULL,BAD_CAST "common");
        xmlAddChild(root_node,common_node);
        get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);
        xmlNewTextChild(common_node,NULL,BAD_CAST "sadd",(xmlChar *)sysConfig.f_config_value);
        get_sys_config(CONFIG_PRIMARY_SERVER,&sysConfig);
        xmlNewTextChild(common_node,NULL,BAD_CAST "oadd",(xmlChar *)sysConfig.f_config_value);  //Ŀ�ĵ�ַ�Ժ�ĳɷ�������ַ��
        sprintf(str, "%d", em_FUNC_RPTUP);
        xmlNewTextChild(common_node,NULL,BAD_CAST "func_type",(xmlChar *)str);
        get_sys_config(CONFIG_REPORT_MODE,&sysConfig);//�����ϱ�ģʽ,0-�����ϱ�, 1��������
        sprintf(str, "%d", (atoi(sysConfig.f_config_value) ? em_OPER_ASW : em_OPER_WR));//�����ϱ�, ����д��; ��������, Ӧ��
        xmlNewTextChild(common_node,NULL,BAD_CAST "oper_type",(xmlChar *)str);
        g_xml_info[dev].cur_wr_state = stat_his_trans;
        break;
    case stat_his_trans:
        root_node = xmlDocGetRootElement(g_xml_info[dev].xmldoc_wr);
        trans_node = xmlNewNode(NULL,BAD_CAST "trans");
        xmlAddChild(root_node,trans_node);
        sprintf(str, "%d", g_xml_info[dev].total_rows);
        xmlNewTextChild(trans_node, NULL, BAD_CAST "total_meter_num", (xmlChar *)str);//�ܹ�Ҫ�ϱ��ĳ�����������
        sprintf(str, "%d", g_xml_info[dev].cur_frame_indep);
        xmlNewTextChild(trans_node, NULL, BAD_CAST "frame_idx", (xmlChar *)str);//��֡������
        sprintf(str, "%d", g_xml_info[dev].cur_cnt);
        xmlNewTextChild(trans_node, NULL, BAD_CAST "meter_num", (xmlChar *)str);//��֡��������������������
        xmlNewTextChild(trans_node, NULL, BAD_CAST "time_node", (xmlChar *)(g_xml_info[dev].timenode));//�ϱ������ĸ�ʱ��������
        g_xml_info[dev].cur_wr_state = stat_his_data;
        break;
    case stat_his_data:
        printf("now state: stat_his_data\n");
        g_xml_info[dev].cur_row_idx++;
        root_node = xmlDocGetRootElement(g_xml_info[dev].xmldoc_wr);
        row_node = xmlNewNode(NULL,BAD_CAST "row");
        sprintf(str, "%d", g_xml_info[dev].cur_row_idx);
        xmlNewProp(row_node, BAD_CAST "id", BAD_CAST str);//��ǰ֡�ĵڼ���
        xmlAddChild(root_node,row_node);
        //�̶���
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_id", CONST_CAST pHis->f_id);//row_id
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_meter_type", CONST_CAST pHis->f_meter_type);//������
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_device_id", CONST_CAST pHis->f_device_id);//�豸���
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_meter_address", CONST_CAST pHis->f_meter_address);//���ַ
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_timestamp", CONST_CAST pHis->f_timestamp);//ʱ���
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_time", CONST_CAST pHis->f_time);//����ʱ���
        printf("insert constant fields over!!!!!!\n");
        for(val_idx=0;val_idx<pHis->value_cnt;val_idx++) {//������
            xmlNewTextChild(row_node, NULL, CONST_CAST pHis->value_list[val_idx].field_name, \
            CONST_CAST pHis->value_list[val_idx].field_value);
        }
        printf("insert variant fields over!!!!!!\n");
        printf("g_xml_info[dev].cur_row_idx: %d,  g_xml_info[dev].cur_cnt: %d\n", g_xml_info[dev].cur_row_idx, g_xml_info[dev].cur_cnt);
        if(g_xml_info[dev].cur_row_idx == g_xml_info[dev].cur_cnt) {
            g_xml_info[dev].cur_wr_state = stat_his_end;
            printf("insert all row over!!!\n");
            printf("point to next state stat_his_end\n");
        }
        printf("insert one row over!!!\n");
        break;
    case stat_his_end:
        printf("now in stat_his_end, and write to xmlfile: %s\n", gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile);
        fp = fopen(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile,"w+");
        nRel = xmlSaveFileEnc(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile, g_xml_info[dev].xmldoc_wr,"utf-8");
        fclose(fp);
        if(nRel != -1){
            xmlFreeDoc(g_xml_info[dev].xmldoc_wr);
            printf("make xml %s Index = %d.\n", gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile, g_xml_info[dev].xml_wr_file_idx);
            return NO_ERR;
        }
        break;
    default:
        return ERR_1;
        break;
    }
    return nRel;
}

uint8 up_his_data(uint8 dev)
{
    if(dev<0 || dev>UP_COMMU_DEV_ARRAY)
        return ERR_1;
    
    uint8 err = NO_ERR;
    FILE *fp;
    char pErr[100];
    uint8 lu8xmlIndex;
    uint8 type_idx;//�Ǳ����͵�����
    err = get_tm_node(dev, g_xml_info[dev].timenode);
    printf("g_xml_info[dev].timenode: %s\n", g_xml_info[dev].timenode);
    read_all_his_data(g_xml_info[dev].timenode, pErr);
    for(type_idx=0;type_idx<MTYPE_CNT;type_idx++)
    {
        printf("--------------current meter type: %d--------------\n", type_idx);
        printf("--------------current meter get_his_cnt: %d--------------\n", get_his_cnt(type_idx));
        g_xml_info[dev].total_row[type_idx] += get_his_cnt(type_idx);
        g_xml_info[dev].total_rows += g_xml_info[dev].total_row[type_idx];
        g_xml_info[dev].mod[type_idx] = g_xml_info[dev].total_row[type_idx]%ROW_PER_FRAME;
        g_xml_info[dev].cur_frame[type_idx] = 0;
        g_xml_info[dev].total_frame[type_idx] = (g_xml_info[dev].total_row[type_idx]/ROW_PER_FRAME+((g_xml_info[dev].mod[type_idx])?1:0));
    }
    
    for(type_idx=0;type_idx<MTYPE_CNT;type_idx++)
    {
        printf("--------------current meter type: %d--------------\n", type_idx);
        printf("--------------current total_frame[type_idx]: %d--------------\n", g_xml_info[dev].total_frame[type_idx]);
        while(g_xml_info[dev].cur_frame[type_idx]<g_xml_info[dev].total_frame[type_idx]){
            g_xml_info[dev].cur_frame[type_idx]++;
            g_xml_info[dev].cur_frame_indep++;
            g_xml_info[dev].cur_wr_state = stat_his_init;//ÿ����֮֡ǰ���ǳ�ʼ״̬
            wr_his_xml(NULL, dev);//дroot�ڵ��common�ڵ�
            g_xml_info[dev].cur_cnt = (g_xml_info[dev].cur_frame[type_idx]<=(g_xml_info[dev].total_frame[type_idx]-1) ? ROW_PER_FRAME:g_xml_info[dev].mod[type_idx]);
            wr_his_xml(NULL, dev);//дtrans�ڵ�
            g_xml_info[dev].cur_row_idx = 0;
            retrieve_and_del_his_data(type_idx, g_xml_info[dev].cur_cnt, wr_his_xml, dev);//дrow�ڵ�

            do{//��ȡһ��xml�ݴ�ռ�,���һ��Ҫ�ͷŸÿռ䣬��ȡ-ʹ��-�ͷš�
                lu8xmlIndex = Get_XMLBuf();
            }while(lu8xmlIndex == ERR_FF);
            g_xml_info[dev].xml_wr_file_idx = lu8xmlIndex;
            printf("call wr_his_xml to write xml to file.\n");
            wr_his_xml(NULL, dev);//��xmldocд���ļ�

            
            if(err == NO_ERR) {//�����ļ�
                fp = fopen(gXML_File[lu8xmlIndex].pXMLFile,"r");
                FileSend(dev, fp);
                fclose(fp);
            }
            sem_wait(&His_asw_sem);//wait answer from server
            
            Put_XMLBuf(lu8xmlIndex);  //�ͷű�ռ�õ�xml�ݴ�.
            g_xml_info[dev].cur_rows += g_xml_info[dev].cur_cnt;//��ǰ�ѷ��͵�����
            printf("make xml over!!!!!!!!\n");
        }
    }
    return err;
}

//���������ϴ�
uint8 func_rptup(uint8 dev, uint8 xml_idx)
{
    uint8 err = NO_ERR;

	switch(g_xml_info[dev].oper_type){
	case em_OPER_RD:
        OS_ENTER_CRITICAL();
        gu8Dev = dev;
        sem_post(&His_up_sem);//�����ϴ������ź���
        OS_EXIT_CRITICAL();
        break;
	case em_OPER_WR:
        break;
	case em_OPER_ASW:
        printf("have read history report answer from server.\n");
        sem_post(&His_asw_sem);
        UpdGprsRunSta_FeedSndDog();
        break;
	default:
		err = ERR_FF;
	}
	return err;
}



uint8 func_rdstat(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_swip(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_dbmani(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_syscmd(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_codeup(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 func_prototrs(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}



uint8 xml_exec(uint8 dev, uint8 xml_idx)
{
	if(dev >= UP_COMMU_DEV_ARRAY){
		printf("dev num error.\n");
		return ERR_1;
	}
	
	printf("g_xml_info[dev].func_type: %u\n", g_xml_info[dev].func_type);
	
	return (*xml_exec_array[g_xml_info[dev].func_type])(dev, xml_idx);
}

uint8 parse_common(uint8 dev, uint8 xml_idx, xmlNodePtr common_node)
{
	uint8 retErr = NO_ERR;
	xmlNodePtr cur_node;
	xmlChar* pValue;
	sys_config_str sysConfig;
		
	cur_node = common_node->xmlChildrenNode;
	while(cur_node != NULL){
		if(xmlStrEqual(cur_node->name, CONST_CAST NODE_SRC_ADDR)){
			pValue = xmlNodeGetContent(cur_node->xmlChildrenNode);
			strcpy((char*)g_xml_info[dev].sadd, (char*)pValue);
			xmlFree(pValue);
		}
		else if(xmlStrEqual(cur_node->name, CONST_CAST NODE_OBJ_ADDR)){
			pValue = xmlNodeGetContent(cur_node->xmlChildrenNode);
			strcpy((char*)g_xml_info[dev].oadd, (char*)pValue);
			xmlFree(pValue);
		}
		else if(xmlStrEqual(cur_node->name, CONST_CAST NODE_FUNC_TYPE)){
			pValue = xmlNodeGetContent(cur_node->xmlChildrenNode);
			g_xml_info[dev].func_type = atoi((char*)pValue);
			xmlFree(pValue);
		}
		else if(xmlStrEqual(cur_node->name, CONST_CAST NODE_OPER_TYPE)){
			pValue = xmlNodeGetContent(cur_node->xmlChildrenNode);
			g_xml_info[dev].oper_type = atoi((char*)pValue);
			xmlFree(pValue);
		}
		else{//�쳣���
			retErr = NO_ERR;
		}
		cur_node=cur_node->next;
	}
	//����Ӧ���ȼ���ǲ��Ƿ��͸��������������ݣ��������������ǵĻ��ż�����
	//if (oadd is not me){return ERR_NOT_ME;}
	retErr = get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);

    if(retErr == NO_ERR){
        if(0 == strcmp((char *)sysConfig.f_config_value, (char *)g_xml_info[dev].oadd)){  //ֻ���Ƿ����������������ݲŴ���
            printf("start  xml_exec().\n");
            retErr = xml_exec(dev, xml_idx);
            printf((retErr==NO_ERR)?"parse xml success\n":"parse xml fail\n");
            g_xml_info[dev].xml_rd_file_idx = xml_idx;
        }
        else{
            //���Ǹ����������ģ���մ����ɣ�������
        }
    }

	return retErr;
}


uint8 parse_xml(uint8 dev, uint8 xml_idx)
{
    
	if(dev >= UP_COMMU_DEV_ARRAY){
		printf("dev num error.\n");
		return ERR_1;
	}
	
	if(dev == UP_COMMU_DEV_GPRS)
			UpdGprsRunSta_FeedSndDog();  //���κ�GPRS���������յ�������Ϊ�������������GPRS�������������������������������գ���GPRS������
	

	//sem_wait(fd's xml_info_str read semaphore)
	printf("current file name : %s\n", gXML_File[xml_idx].pXMLFile);
	xmlDocPtr doc;
	xmlNodePtr cur;
	uint8 retErr;
	doc = xmlParseFile(gXML_File[xml_idx].pXMLFile);
	if(doc == NULL) {
		fprintf(stderr, "Document not open successfully. \n");
		//sem_post(fd's xml_info_str read semaphore)
		//Put_XMLBuf
		return ERR_FF;
	}
	g_xml_info[dev].xmldoc_rd = doc;

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
			retErr = parse_common(dev, xml_idx, cur);
			break;
		}
		cur = cur->next;
	}
	
	return retErr;
}


