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
static xml_info_str g_xml_info[UP_COMMU_DEV_ARRAY];

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
			
			*(pRecvTempBuf[dev] + j) = DataTemp;
			
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
             else{
                 j++;
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
uint8 makexml(xml_info_str *xmlInfo,uint8 xmlIndex)
{
	sys_config_str sysConfig;
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
	
	
	switch(xmlInfo->func_type){
		case em_FUNC_ID:
			//在common节点直接创建文本节点
			get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);
    			xmlNewTextChild(node,NULL,BAD_CAST "sadd",(xmlChar *)sysConfig.f_config_value);
    			xmlNewTextChild(node,NULL,BAD_CAST "oadd",(xmlChar *)sysConfig.f_config_value);  //目的地址以后改成服务器地址。
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
			//在common节点直接创建文本节点
			get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);
            xmlNewTextChild(node,NULL,BAD_CAST "sadd",(xmlChar *)sysConfig.f_config_value);
            xmlNewTextChild(node,NULL,BAD_CAST "oadd",(xmlChar *)sysConfig.f_config_value);//目的地址以后改成服务器地址。
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
  * 函数名称： uint8 setXmlInfo()
  * 说    明： 根据设备号，设置相应的gXmlInfo信息。
  * 参    数： 
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
  * 函数名称： uint8 getXmlInfo(uint8 dev,xml_info_str *xmlInfo)
  * 说    明： 根据设备编号，获取对应的xmlInfo信息。
  * 参    数： 
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
 * 函数名称： parse_xml
 * 说	  明： 解析并执行xml文件
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
		printf("have read func_id answer from server.\n");//接收到应答, 登录成功
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
		printf("have read heart_beat answer from server.\n");//接收到应答, 本次心跳成功
		//清空看门狗
		UpdGprsRunSta_FeedSndDog();
		break;
	default:
		err = ERR_FF;
	}
	return err;
}

//3．配置信息(sys_config)
uint8 func_sysconfig(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}
//4. 配置需要哪些物理量(request_data)
uint8 func_rqdata(uint8 dev, uint8 xml_idx)
{
	uint8 err = NO_ERR;
	
	return err;
}
//5. 配置抄表时间点(time_node)
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
    printf("now send_upmeter_answer()\n");
    g_xml_info[dev].xmldoc_wr = xmlNewDoc(BAD_CAST"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");
    xmlDocSetRootElement(g_xml_info[dev].xmldoc_wr,root_node);
    xmlNodePtr common_node = xmlNewNode(NULL,BAD_CAST "common");
    xmlAddChild(root_node,common_node);
    sys_config_str sysConfig;
    get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);
    xmlNewTextChild(common_node,NULL,BAD_CAST "sadd",(xmlChar *)sysConfig.f_config_value);
    get_sys_config(CONFIG_PRIMARY_SERVER,&sysConfig);
    xmlNewTextChild(common_node,NULL,BAD_CAST "oadd",(xmlChar *)sysConfig.f_config_value);  //目的地址以后改成服务器地址。
    char str[100];
    sprintf(str, "%d", em_FUNC_MINFO);
    xmlNewTextChild(common_node,NULL,BAD_CAST "func_type",(xmlChar *)str);
    sprintf(str, "%d", em_OPER_ASW);
    xmlNewTextChild(common_node,NULL,BAD_CAST "oper_type",(xmlChar *)str);

    xmlNewTextChild(root_node,NULL,BAD_CAST "result",BAD_CAST "success");
    sprintf(str, "%d", g_xml_info[dev].cur_frame_indep);
    xmlNewTextChild(root_node,NULL,BAD_CAST "frame_idx",BAD_CAST str);
    printf("now send_upmeter_answer(), write to file\n");
    do{//获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
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
    }
    printf("now send_upmeter_answer(), send to server answer\n");
    if(err == NO_ERR) {//发送文件
        fp = fopen(gXML_File[lu8xmlIndex].pXMLFile,"r");
        FileSend(dev, fp);
        fclose(fp);
    }    
    Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存.
    return err;
}

uint8 update_meter_info(uint8 dev)
{
    char pErr[100];
    pMeter_info pMInfo;
    xmlDocPtr pDoc = g_xml_info[dev].xmldoc_rd;
    printf("[%s][%s][%d] pDoc\n", FILE_LINE);
    xmlNodePtr rootNode = xmlDocGetRootElement(pDoc);
    xmlNodePtr curNode;
    xmlNodePtr transNode;
    xmlNodePtr rowNode;
    xmlChar* pValue;
    int total_rows;//上位机一共要下发多少行数据
    printf("[%s][%s][%d] rootNode:　%p\n", FILE_LINE, rootNode);
    curNode = rootNode->children;
    while(curNode) {//得到trans节点
        printf("curNode->name: %s\n", curNode->name);
        if(xmlStrEqual(curNode->name, CONST_CAST "trans")) {
            transNode = curNode;
            break;
        }
        curNode = curNode->next;
    }
    printf("read trans done\n");
    //得到帧索引和总行数
    curNode = transNode->children;
    while(curNode) {
        printf("curNode->name: %s\n", curNode->name);
        if(xmlStrEqual(curNode->name, CONST_CAST "frame_idx")) {
            pValue = xmlNodeGetContent(curNode->xmlChildrenNode);
            printf("pValue: %s\n", pValue);
            if(xmlStrEqual(CONST_CAST"1", CONST_CAST pValue)) {
                empty_meter_info_list();
                g_xml_info[dev].meter_info_row_idx = 0;//如果是第一帧, 清空行数计数
            }
            g_xml_info[dev].cur_frame_indep = atoi((char*)pValue);
        }else if(xmlStrEqual(curNode->name, CONST_CAST "total_meter_num")) {
            total_rows = atoi((char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("total_rows: %d\n", total_rows);
        }
        curNode = curNode->next;
    }
    printf("read total meter number and frame_index done\n");
    //读取仪表地址信息
    int length, j, low_idx;
    curNode = rootNode->children;
    while(curNode) {//逐个遍历row节点
        printf("now we find row node, curNode->name: %s\n", curNode->name);
        if (xmlStrEqual(curNode->name, CONST_CAST "row")) {
            printf("curNode->name: %s\n", curNode->name);
            rowNode = curNode->children;
            printf("rowNode->name: %s\n", rowNode->name);
            pMInfo = malloc(sizeof(struct meter_info_str));
            while(rowNode) {//获取仪表的详细信息
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
                    printf("meter info : %02x%02x%02x%02x%02x%02x%02x\n", pMInfo->f_meter_address[6], \
        pMInfo->f_meter_address[5], pMInfo->f_meter_address[4], pMInfo->f_meter_address[3], \
        pMInfo->f_meter_address[2], pMInfo->f_meter_address[1], pMInfo->f_meter_address[0]);
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
            printf("g_xml_info[dev].meter_info_row_idx: %d\n", g_xml_info[dev].meter_info_row_idx);
        }
        curNode = curNode->next;
    }
    printf("read row info done\n");
    printf("now free pDoc\n");
    xmlFreeDoc(pDoc);//解析完毕, 释放pDoc
    send_upmeter_answer(dev);
    printf("send_upmeter_answer OK\n");
    if(total_rows == g_xml_info[dev].meter_info_row_idx) {//如果上位机发完, 更新数据库
        printf("now insert_into_meter_info_table():\n");        
        empty_meter_info_table(pErr);
        insert_into_meter_info_table(pErr);
        printf("insert_into_meter_info_table done\n");
        //read_meter_info(pErr);//实际没有必要重新读取表地址
    }
    return NO_ERR;
}

//6. 操作表地址信息(meter_info)
uint8 func_minfo(uint8 dev, uint8 xml_idx)
{
	uint8 err = NO_ERR;
    printf("now in func_minfo()\n");
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
    //printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
    //xmlDocPtr pDoc = g_xml_info[dev].xmldoc_rd;
    //printf("[%s][%s][%d] pXMLFile: %s\n", FILE_LINE, gXML_File[g_xml_info[dev].xml_rd_file_idx].pXMLFile);
    xmlDocPtr pDoc = xmlParseFile(gXML_File[g_xml_info[dev].xml_rd_file_idx].pXMLFile);
    //printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d] pDoc %p<<<<<<<<<<<<<<<<<<<\n", FILE_LINE, pDoc);
    //printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
    xmlNodePtr curNode = xmlDocGetRootElement(pDoc);//<root>node
    //printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
    curNode = curNode->children;//<common>node
    while(curNode) {
        //printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
        if(xmlStrEqual(curNode->name, CONST_CAST "time")) {
            printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
            strcpy(timenode, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
        }
        //printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
        curNode = curNode->next;
    }
    //printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
	xmlFreeDoc(pDoc);//when have getted timenode, free server's xmlDoc
    //printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
    return NO_ERR;
}

//向xmldoc中添加一行的数据节点
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
    int val_idx=0;//历史数据项的索引
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
        xmlNewTextChild(common_node,NULL,BAD_CAST "oadd",(xmlChar *)sysConfig.f_config_value);  //目的地址以后改成服务器地址。
        sprintf(str, "%d", em_FUNC_RPTUP);
        xmlNewTextChild(common_node,NULL,BAD_CAST "func_type",(xmlChar *)str);
        get_sys_config(CONFIG_REPORT_MODE,&sysConfig);//数据上报模式,0-主动上报, 1被动请求
        sprintf(str, "%d", (atoi(sysConfig.f_config_value) ? em_OPER_ASW : em_OPER_WR));//主动上报, 就是写入; 被动请求, 应答
        xmlNewTextChild(common_node,NULL,BAD_CAST "oper_type",(xmlChar *)str);
        g_xml_info[dev].cur_wr_state = stat_his_trans;
        break;
    case stat_his_trans:
        root_node = xmlDocGetRootElement(g_xml_info[dev].xmldoc_wr);
        trans_node = xmlNewNode(NULL,BAD_CAST "trans");
        xmlAddChild(root_node,trans_node);
        sprintf(str, "%d", g_xml_info[dev].total_rows);
        xmlNewTextChild(trans_node, NULL, BAD_CAST "total_meter_num", (xmlChar *)str);//总共要上报的抄表数据条数
        sprintf(str, "%d", g_xml_info[dev].cur_frame_indep);
        xmlNewTextChild(trans_node, NULL, BAD_CAST "frame_idx", (xmlChar *)str);//本帧的索引
        sprintf(str, "%d", g_xml_info[dev].cur_cnt);
        xmlNewTextChild(trans_node, NULL, BAD_CAST "meter_num", (xmlChar *)str);//本帧共包含多少条抄表数据
        xmlNewTextChild(trans_node, NULL, BAD_CAST "time_node", (xmlChar *)(g_xml_info[dev].timenode));//上报的是哪个时间点的数据
        g_xml_info[dev].cur_wr_state = stat_his_data;
        break;
    case stat_his_data:
        printf("[%s][%s][%d] now enter state: stat_his_data\n", FILE_LINE);
        g_xml_info[dev].cur_row_idx++;
        root_node = xmlDocGetRootElement(g_xml_info[dev].xmldoc_wr);
        printf("[%s][%s][%d] xmldoc_wr: %p\n", FILE_LINE, g_xml_info[dev].xmldoc_wr);
        printf("[%s][%s][%d] root_node: %p\n", FILE_LINE, root_node);
        row_node = xmlNewNode(NULL,BAD_CAST "row");
        sprintf(str, "%d", g_xml_info[dev].cur_row_idx);
        xmlNewProp(row_node, BAD_CAST "id", BAD_CAST str);//当前帧的第几行
        xmlAddChild(root_node,row_node);
        //固定项
        printf("[%s][%s][%d] befor insert constant, pHis: %p\n", FILE_LINE, pHis);
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_id", CONST_CAST pHis->f_id);//row_id
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_meter_type", CONST_CAST pHis->f_meter_type);//表类型
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_device_id", CONST_CAST pHis->f_device_id);//设备编号
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_meter_address", CONST_CAST pHis->f_meter_address);//表地址
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_timestamp", CONST_CAST pHis->f_timestamp);//时间戳
        xmlNewTextChild(row_node, NULL, CONST_CAST "f_time", CONST_CAST pHis->f_time);//抄表时间点
        printf("[%s][%s][%d] insert constant fields over!!!!!!\n", FILE_LINE);
        for(val_idx=0;val_idx<pHis->value_cnt;val_idx++) {//数据项
            xmlNewTextChild(row_node, NULL, CONST_CAST pHis->value_list[val_idx].field_name, \
            CONST_CAST pHis->value_list[val_idx].field_value);
        }
        printf("[%s][%s][%d] insert variant fields over!!!!!!\n", FILE_LINE);
        printf("[%s][%s][%d] g_xml_info[dev].cur_row_idx: %d, \
            g_xml_info[dev].cur_cnt: %d\n", FILE_LINE, g_xml_info[dev].cur_row_idx, g_xml_info[dev].cur_cnt);
        if(g_xml_info[dev].cur_row_idx == g_xml_info[dev].cur_cnt) {
            g_xml_info[dev].cur_wr_state = stat_his_end;
            printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<insert all row over!!!\n", FILE_LINE);
            printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<point to next state stat_his_end\n", FILE_LINE);
        }
        printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<\n", FILE_LINE);
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
    if(dev<0 || dev>=UP_COMMU_DEV_ARRAY)
        return ERR_1;

    int ret = 0;
    uint8 lu8times = 0;
    uint8 err = NO_ERR;
    FILE *fp;
    char pErr[100];
    uint8 lu8xmlIndex;
    uint8 type_idx;//仪表类型的索引
    
    printf("g_xml_info[dev].timenode: %s\n", g_xml_info[dev].timenode);
    read_all_his_data(g_xml_info[dev].timenode, pErr);
    g_xml_info[dev].cur_frame_indep = 0;
    g_xml_info[dev].total_rows = 0;
    g_xml_info[dev].cur_rows = 0;
    for(type_idx=0;type_idx<MTYPE_CNT;type_idx++) {
        //printf("--------------current meter type: %d--------------\n", type_idx);
        //printf("--------------current meter get_his_cnt: %d--------------\n", get_his_cnt(type_idx));
        g_xml_info[dev].total_row[type_idx] = get_his_cnt(type_idx);
        g_xml_info[dev].total_rows += g_xml_info[dev].total_row[type_idx];
        g_xml_info[dev].mod[type_idx] = g_xml_info[dev].total_row[type_idx]%ROW_PER_FRAME;
        printf("##########[%s][%s][%d]##########total_row[%d]: %d.\n", FILE_LINE, type_idx, g_xml_info[dev].total_row[type_idx]);
        printf("##########[%s][%s][%d]##########mod[%d]: %d.\n", FILE_LINE, type_idx, g_xml_info[dev].mod[type_idx]);
        g_xml_info[dev].cur_frame[type_idx] = 0;
        g_xml_info[dev].total_frame[type_idx] = (g_xml_info[dev].total_row[type_idx]/ROW_PER_FRAME+((g_xml_info[dev].mod[type_idx])?1:0));
    }
    
    for(type_idx=0;type_idx<MTYPE_CNT;type_idx++) {
        printf("--------------total_row[%d]: %d--------------\n", type_idx,  g_xml_info[dev].total_row[type_idx]);
        while(g_xml_info[dev].cur_frame[type_idx]<g_xml_info[dev].total_frame[type_idx]){
            g_xml_info[dev].cur_frame[type_idx]++;
            g_xml_info[dev].cur_frame_indep++;
            g_xml_info[dev].cur_wr_state = stat_his_init;//每次组帧之前都是初始状态
            wr_his_xml(NULL, dev);//写root节点和common节点
            printf("##########[%s][%s][%d]cur_frame[%d]: %d, total_frame[%d]: %d##########\n",FILE_LINE,\
                type_idx, g_xml_info[dev].cur_frame[type_idx], type_idx, g_xml_info[dev].total_frame[type_idx]);

            if(g_xml_info[dev].cur_frame[type_idx] == g_xml_info[dev].total_frame[type_idx])
               g_xml_info[dev].cur_cnt = g_xml_info[dev].mod[type_idx];//如果当前帧是最后一帧, 那么等于余数
            else
               g_xml_info[dev].cur_cnt = ROW_PER_FRAME;//否则等于约定的行数

            printf("[%s][%s][%d]current frameidx: %d, cur_cnt: %d, mod: %d\n", FILE_LINE, g_xml_info[dev].cur_frame_indep, \
                g_xml_info[dev].cur_cnt, g_xml_info[dev].mod[type_idx]);
		
		  wr_his_xml(NULL, dev);//写trans节点
            g_xml_info[dev].cur_row_idx = 0;
            retrieve_and_del_his_data(type_idx, g_xml_info[dev].cur_cnt, wr_his_xml, dev);//写row节点, 查询完就删除, 以免下一帧从头开始查

            do{//获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
                lu8xmlIndex = Get_XMLBuf();
            }while(lu8xmlIndex == ERR_FF);
            g_xml_info[dev].xml_wr_file_idx = lu8xmlIndex;
            printf(">>>>>>>>>>>>>>>>>>>>>[%s][%s][%d]<<<<<<<<<<<<<<<<<<<call wr_his_xml to write xml to file.\n", FILE_LINE);
            wr_his_xml(NULL, dev);//将xmldoc写到文件

            for(lu8times=0;lu8times<5;lu8times++){
            		if(err == NO_ERR) {//发送文件
                		fp = fopen(gXML_File[lu8xmlIndex].pXMLFile,"r");
                		FileSend(dev, fp);
                		fclose(fp);
            		}
		  
		  		ret = OSSem_timedwait(&His_asw_sem,10*OS_TICKS_PER_SEC);
		  		if(ret != 0){
					printf("data report fail,times=%d .\n",lu8times+1);
					continue;
				}
				else				
					break; //如果发送后收到应答，则跳出继续发送下一帧。
            }

            g_xml_info[dev].cur_rows += g_xml_info[dev].cur_cnt;//当前已发送的行数
            printf("[%s][%s][%d] cur_frame %d cur_frame_indep %d, make xml over!!!!!!!!\n", FILE_LINE,\
            g_xml_info[dev].cur_frame[type_idx], g_xml_info[dev].cur_frame_indep);
            printf("make xml over!!!!!!!!\n");

		  if(ret != 0){  //如果连续发送不成功，则退出，不再上推。
			  printf("data report fail, send quit .\n");
			break;
		  }

        }
    }
    printf("[%s][%s][%d] all frame make xml over!!!!!!!!\n", FILE_LINE);
    return err;
}

uint8 set_xml_timenode(uint8 dev, char* timenode)
{
    uint8 err = NO_ERR;
    strcpy(g_xml_info[dev].timenode, timenode);
    return err;
}


//抄表数据上传
uint8 func_rptup(uint8 dev, uint8 xml_idx)
{
    uint8 err = NO_ERR;
    QmsgType Qmsg;
    printf("[%s][%s][%d] func_rptup g_xml_info[%d].oper_type: %d.\n", FILE_LINE, dev, g_xml_info[dev].oper_type);
	switch(g_xml_info[dev].oper_type){
	case em_OPER_RD:
        printf("[%s][%s][%d] em_OPER_RD.\n", FILE_LINE);
        Qmsg.mtype = 1;  //不要写0，其他都可以。
        Qmsg.dev = dev;
        Qmsg.functype = em_FUNC_RPTUP;
        get_tm_node(dev, Qmsg.timenode);
        msgsnd(g_uiQmsgFd,&Qmsg,sizeof(QmsgType),0);
        break;
	case em_OPER_WR:
        break;
	case em_OPER_ASW:
        printf("[%s][%s][%d]have read history report answer from server.\n", FILE_LINE);
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
	
	printf("[%s][%s][%d] g_xml_info[dev].func_type: %u\n", FILE_LINE, g_xml_info[dev].func_type);
	
	return (*xml_exec_array[g_xml_info[dev].func_type])(dev, xml_idx);
}

uint8 parse_common(uint8 dev, uint8 xml_idx, xmlNodePtr common_node)
{
	uint8 retErr = NO_ERR;
	xmlNodePtr cur_node;
	xmlChar* pValue;
	sys_config_str sysConfig;
    printf("[%s][%s][%d]current file name : %s\n", FILE_LINE, gXML_File[g_xml_info[dev].xml_rd_file_idx].pXMLFile);
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
		else{//异常情况
			retErr = NO_ERR;
		}
		cur_node=cur_node->next;
	}
	//正常应该先检查是不是发送给本集中器的数据，不是则舍弃，是的话才继续。
	//if (oadd is not me){return ERR_NOT_ME;}
	retErr = get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);

    if(retErr == NO_ERR){
        if(0 == strcmp((char *)sysConfig.f_config_value, (char *)g_xml_info[dev].oadd)){  //只有是发给本集中器的数据才处理。
            printf("start  xml_exec().\n");
            retErr = xml_exec(dev, xml_idx);
            printf((retErr==NO_ERR)?"parse xml success\n":"parse xml fail\n");
            g_xml_info[dev].xml_rd_file_idx = xml_idx;

            //在此处解析xml文件已经是在执行完相应的函数后了, 不起到任何作用, g_xml_info[dev].xmldoc_rd在此前一直为NULL
            //g_xml_info[dev].xmldoc_rd = xmlParseFile(gXML_File[xml_idx].pXMLFile);
        }
        else{
            //不是给本集中器的, 舍弃。
            free(g_xml_info[dev].xmldoc_rd);//当上位机发送的目标不是是本集中器, 则释放xml树结构
            memset(&g_xml_info[dev], 0, sizeof(xml_info_str));
        }
    }
    printf("[%s][%s][%d]current file name : %s\n", FILE_LINE, gXML_File[g_xml_info[dev].xml_rd_file_idx].pXMLFile);
	return retErr;
}


uint8 parse_xml(uint8 dev, uint8 xml_idx)
{
    
	if(dev >= UP_COMMU_DEV_ARRAY){
		printf("dev num error.\n");
		return ERR_1;
	}
	
	if(dev == UP_COMMU_DEV_GPRS)
			UpdGprsRunSta_FeedSndDog();  //有任何GPRS网络数据收到，都认为网络正常，清空GPRS心跳计数。如果连续几次心跳都不清空，则GPRS重启。
	

	//sem_wait(fd's xml_info_str read semaphore)
	printf("[%s][%s][%d]current file name : %s\n", FILE_LINE, gXML_File[xml_idx].pXMLFile);
	xmlDocPtr doc;
	xmlNodePtr cur;
	uint8 retErr;
	doc = xmlParseFile(gXML_File[xml_idx].pXMLFile);
    g_xml_info[dev].xmldoc_rd = doc;
	if(doc == NULL) {
		fprintf(stderr, "Document not open successfully. \n");
		//sem_post(fd's xml_info_str read semaphore)
		//Put_XMLBuf
		return ERR_FF;
	}

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


