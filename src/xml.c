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
#include "bsp.h"
#include "uart_set.h"

char *pXMLFileName[XML_BUF_FILE_NUM]={"buff0.xml","buff1.xml","buff2.xml","buff3.xml","buff4.xml","buff5.xml",
 		"buff6.xml","buff7.xml","buff8.xml","buff9.xml","buff10.xml","buff11.xml","buff12.xml",
 		"buff13.xml","buff14.xml"};

XML_FILE gXML_File[XML_BUF_FILE_NUM];  //定义xml文件相关变量。
static xml_info_str g_xml_info[UP_COMMU_DEV_ARRAY];//定义时初始化
char *colname_heat[] = {"f_cur_cold_E", "f_cur_heat_E", "f_heat_power", "f_flowrate", \
    "f_accum_flow", "f_in_temp", "f_out_temp", "f_accum_work_time", "f_cur_time", \
    "f_state"};
char *colname_water[] = {"f_cur_accum_flow", "f_flowrate", "f_settle_date_accum_flow", \
    "f_settle_date", "f_accum_work_time"};
char *colname_elec[] = {"f_pact_tot_elec", "f_nact_tot_elec", "f_preact_tot_elec", \
    "f_nreact_tot_elec", "f_act_tot_elec", "f_react_tot_elec", "f_ovr_pow_fac", \
    "f_pmax_dem"};
char *colname_gas[] = {'\0'};

static uint8 (*xml_exec_array[])(uint8 dev, uint8 xml_idx) = {
func_id,        // -0-登录
func_heart_beat,// -1-心跳
func_sysconfig, // -2-系统参数配置
func_rqdata,    // -3-仪表的数据项配置
func_tnode,     // -4-抄表与上报时间点配置
func_minfo,     // -5-表地址配置
func_rptup,     // -6-上传历史数据
func_rdstat,    // -7-读取集中器状态
func_swip,      // -8-切换ip
func_dbmani,    // -9-数据库透传
func_syscmd,    // -10-本地shell命令透传
func_codeup,    // -11-程序更新
func_prototrs,  // -12-协议透传
func_clock_set, // -13-设置集中器时钟
func_collect_set// -14-集中器抄表模式和抄表周期设置
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
        char log[256];
        sprintf(log, "[%s][%s][%d]i=%d.\n", FILE_LINE, i);
        write_log_file(log, strlen(log));
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

/*
  ******************************************************************************
  * 函数名称： uint8 UpGetXMLStart(uint8 lu8BufIndex)
  * 说    明： 用于检测xml文件的开头"<?xml version"
  ******************************************************************************
*/
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
    
    char log[256];
    sprintf(log, "[%s][%s][%d]start fopen %s . \n", FILE_LINE, gXML_File[XmlIndex].pXMLFile);
    write_log_file(log, strlen(log));
    
	fp=fopen(gXML_File[XmlIndex].pXMLFile,"w+");
	
	sem_wait(&gXML_File[XmlIndex].sem_write);
	fwrite(XMLHead,1,strlen(XMLHead),fp);
	sem_post(&gXML_File[XmlIndex].sem_write);
	fclose(fp);
	
	return NO_ERR;
	
}

/*
  ******************************************************************************
  * 函数名称： uint8 UpGetXMLEnd(uint8 lu8BufIndex)
  * 说    明： 用于检测xml文件的结尾"</root>"
  ******************************************************************************
*/
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
                    char log[256];
                    sprintf(log, "[%s][%s][%d]OSSemPend err=%d\n", FILE_LINE, err);
                    write_log_file(log, strlen(log));
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
             get_sys_config(CONFIG_SVR_NUM, &sysConfig);
    			xmlNewTextChild(node,NULL,BAD_CAST "oadd", (xmlChar *)sysConfig.f_config_value);  //目的地址以后改成服务器编号
			xmlNewTextChild(node,NULL,BAD_CAST "func_type",(xmlChar *)"0");
			xmlNewTextChild(node,NULL,BAD_CAST "oper_type",(xmlChar *)"0");
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
            get_sys_config(CONFIG_SVR_NUM, &sysConfig);
            xmlNewTextChild(node,NULL,BAD_CAST "oadd", (xmlChar *)sysConfig.f_config_value);  //目的地址以后改成服务器编号

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
		default:
			fclose(fp);
			break;
	}

  	return ERR_1;
}

void init_xml_infos()
{
    memset(&g_xml_info, 0, UP_COMMU_DEV_ARRAY*sizeof(xml_info_str));
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
             FileSend(UP_COMMU_DEV_485, fp);
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
            FileSend(UP_COMMU_DEV_485, fp);
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

//使用同一个函数处理应答, 以便以后维护的时候方便
uint8 send_answer(uint8 dev, char* resNode, char* res, void* otherptr)
{
    uint8 err=NO_ERR;
    FILE *fp;
    int nRel;
    uint8 lu8xmlIndex;
    sys_config_str sysConfig;
    char str[100];

    g_xml_info[dev].xmldoc_wr = xmlNewDoc(BAD_CAST"1.0");
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");
    xmlDocSetRootElement(g_xml_info[dev].xmldoc_wr,root_node);
    xmlNodePtr common_node = xmlNewNode(NULL,BAD_CAST "common");
    xmlAddChild(root_node,common_node);
    get_sys_config(CONFIG_GATEWAY_ID,&sysConfig);
    xmlNewTextChild(common_node,NULL,BAD_CAST "sadd",BAD_CAST sysConfig.f_config_value);
    xmlNewTextChild(common_node,NULL,BAD_CAST "oadd",(xmlChar *)g_xml_info[dev].sadd);

    sprintf(str, "%d", g_xml_info[dev].func_type);
    printf("[%s][%s][%d] func_type: %d\n", FILE_LINE, g_xml_info[dev].func_type);
    xmlNewTextChild(common_node,NULL,BAD_CAST "func_type",(xmlChar *)str);
    sprintf(str, "%d", em_OPER_ASW);
    printf("[%s][%s][%d] oper_type: %s\n", FILE_LINE, str);
    xmlNewTextChild(common_node, NULL, BAD_CAST "oper_type", BAD_CAST str);
    xmlNewTextChild(root_node, NULL, BAD_CAST resNode, BAD_CAST res);
    
    switch (g_xml_info[dev].func_type) {
    case em_FUNC_MINFO:
        sprintf(str, "%d", g_xml_info[dev].cur_frame_indep);
        xmlNewTextChild(root_node, NULL, BAD_CAST "frame_idx", BAD_CAST "res");
        break;
    default:
        break;
    }
    
    do{//获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
        lu8xmlIndex = Get_XMLBuf();
    }while(lu8xmlIndex == ERR_FF);
    g_xml_info[dev].xml_wr_file_idx = lu8xmlIndex;

    fp = fopen(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile,"w+");
    nRel = xmlSaveFileEnc(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile, g_xml_info[dev].xmldoc_wr,"utf-8");
    fclose(fp);
    if(nRel != -1){
        xmlFreeDoc(g_xml_info[dev].xmldoc_wr);
        printf("make xml %s Index = %d.\n", gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile, g_xml_info[dev].xml_wr_file_idx);
    }

    if(err == NO_ERR) {//发送文件
        fp = fopen(gXML_File[lu8xmlIndex].pXMLFile,"r");
        FileSend(dev, fp);
        FileSend(UP_COMMU_DEV_485, fp);
        printf("[%s][%s][%d]xml file have been sent\n", FILE_LINE);
        fclose(fp);
    }
    Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存.
    return err;
}

uint8 write_sysconfig(uint8 dev)
{
    printf("[%s][%s][%d]now in write_sysconfig()\n", FILE_LINE);
    uint8 err = NO_ERR;
    char pErr[100] = {0};
    pSys_config pConfig;//db模块中, set_config() 每次更新完成后都会清空临时配置表, 所以不必在此函数中free(pConfig)

    xmlDocPtr pDoc = g_xml_info[dev].xmldoc_rd;
    printf("[%s][%s][%d] pDoc: %p\n", FILE_LINE, pDoc);
    xmlNodePtr rootNode = xmlDocGetRootElement(pDoc);
    if(NULL == rootNode)
        return ERR_1;
    
    printf("[%s][%s][%d] rootNode: %p\n", FILE_LINE, rootNode);
    xmlNodePtr curNode;
    curNode = rootNode->children;
    printf("[%s][%s][%d] curNode: %p\n", FILE_LINE, curNode);
    uint8 needReboot = 0;
    while(curNode) {
        if(xmlStrEqual(curNode->name, CONST_CAST "primary_server")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_PRIMARY_SERVER;
            strcpy(pConfig->f_config_name, "primary_server");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
            needReboot = 1;
        } else if(xmlStrEqual(curNode->name, CONST_CAST "primary_dns")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_PRIMARY_DNS;
            strcpy(pConfig->f_config_name, "primary_dns");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
            needReboot = 1;
        } else if(xmlStrEqual(curNode->name, CONST_CAST "primary_port")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_PRIMARY_PORT;
            strcpy(pConfig->f_config_name, "primary_port");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
            needReboot = 1;
        } else if(xmlStrEqual(curNode->name, CONST_CAST "second_server")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_SECOND_SERVER;
            strcpy(pConfig->f_config_name, "second_server");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "second_dns")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_SECOND_DNS;
            strcpy(pConfig->f_config_name, "second_dns");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "second_port")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_SECOND_PORT;
            strcpy(pConfig->f_config_name, "second_port");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "gateway_id")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_GATEWAY_ID;
            strcpy(pConfig->f_config_name, "gateway_id");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "net_type")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_NET_TYPE;
            strcpy(pConfig->f_config_name, "net_type");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "md5_key")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_MD5_KEY;
            strcpy(pConfig->f_config_name, "md5_key");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "aes_key")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_AES_KEY;
            strcpy(pConfig->f_config_name, "aes_key");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "report_mode")){
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_REPORT_MODE;
            strcpy(pConfig->f_config_name, "report_mode");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "beat_cycle")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_BEAT_CYCLE;
            strcpy(pConfig->f_config_name, "beat_cycle");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "collect_mode")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_COLLECT_MODE;
            strcpy(pConfig->f_config_name, "collect_mode");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "collect_cycle")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_COLLECT_CYCLE;
            strcpy(pConfig->f_config_name, "collect_cycle");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        }else if(xmlStrEqual(curNode->name, CONST_CAST "svr_num")) {
            pConfig = malloc(sizeof(sys_config_str));
            pConfig->f_id = CONFIG_SVR_NUM;
            strcpy(pConfig->f_config_name, "svr_num");
            strcpy(pConfig->f_config_value, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
            printf("[%s][%s][%d] f_id: %d, f_config_name: %s, f_config_value: %s\n", \
            FILE_LINE, pConfig->f_id, pConfig->f_config_name, pConfig->f_config_value);
            err = insert_sysconf(pConfig);
        }
        curNode = curNode->next;
    }
    printf("[%s][%s][%d]\n", FILE_LINE);
    err = set_sysconf(pErr);
    if (err == NO_ERR)
    {
        err = send_answer(dev, "result", "success", NULL);
        if(needReboot) {
            printf("[%s][%s][%d] reboot system after 10sec!\n", FILE_LINE);
            OSTimeDly(10000);
            system("reboot");
        }
    }
    return err;
}

//3．配置信息(sys_config)
uint8 func_sysconfig(uint8 dev, uint8 xml_idx)
{
    uint8 err = NO_ERR;
    printf("[%s][%s][%d]now in func_sysconfig()\n", FILE_LINE);
    switch(g_xml_info[dev].oper_type){
    case em_OPER_RD:
        break;
    case em_OPER_WR:
        err = write_sysconfig(dev);
        break;
    case em_OPER_ASW:
        
        break;
    default:
        err = ERR_FF;
        break;
    }
    return err;
}

uint8 get_request_col_name(pRequest_data pRqData)
{
    uint8 err = NO_ERR;
    if(NULL == pRqData)
        return ERR_1;
    if(pRqData->f_item_index<0 || pRqData->f_item_index>sizeof(colname_heat))
        return ERR_1;
    switch(pRqData->f_meter_type) {
    case HEATMETER:
        strcpy(pRqData->f_col_name, colname_heat[pRqData->f_item_index-1]);
        break;
    case WATERMETER:
        strcpy(pRqData->f_col_name, colname_water[pRqData->f_item_index-1]);
        break;
    case GASMETER:
        strcpy(pRqData->f_col_name, colname_gas[pRqData->f_item_index-1]);
        break;
    case ELECTMETER:
        strcpy(pRqData->f_col_name, colname_elec[pRqData->f_item_index-1]);
        break;
    default:
        break;
    }
    strcpy(pRqData->f_col_type, "varchar(50)");
    return err;
}

uint8 write_request_data(uint8 dev)
{
    uint8 err = NO_ERR;
    char pErr[100] = {0};
    pRequest_data pRqData;
    xmlDocPtr pDoc = g_xml_info[dev].xmldoc_rd;
    printf("[%s][%s][%d] pDoc: %p\n", FILE_LINE, pDoc);
    xmlNodePtr rootNode = xmlDocGetRootElement(pDoc);
    if(NULL == rootNode)
        return ERR_1;
    printf("[%s][%s][%d] rootNode: %p\n", FILE_LINE, rootNode);
    xmlNodePtr curNode;
    curNode = rootNode->children;
    printf("[%s][%s][%d] curNode: %p\n", FILE_LINE, curNode);
    xmlNodePtr rowNode;
    
    xmlChar* pValue;
    while(curNode) {//逐个遍历row节点
        if (xmlStrEqual(curNode->name, CONST_CAST "row")) {
            rowNode = curNode->children;
            printf("rowNode->name: %s\n", rowNode->name);
            pRqData = malloc(sizeof(struct request_data_str));
            while(rowNode) {//获取配置项的详细信息
                if(xmlStrEqual(rowNode->name, CONST_CAST "f_meter_type")) {
                    pValue = xmlNodeGetContent(rowNode->xmlChildrenNode);
                    pRqData->f_meter_type = (Ascii2Hex(pValue[0])<<LEN_HALF_BYTE|Ascii2Hex(pValue[1]));
                }else if(xmlStrEqual(rowNode->name, CONST_CAST "f_item_index")) {
                    pValue = xmlNodeGetContent(rowNode->xmlChildrenNode);
                    pRqData->f_item_index = atoi((char*)pValue);
                }
                rowNode = rowNode->next;
            }
            get_request_col_name(pRqData);
            err = insert_one_request_node(pRqData);
        }
        curNode = curNode->next;
    }
    err = set_request_data(pErr);
    if(err == NO_ERR) {
        err = send_answer(dev, "result", "success", NULL);
    }
    return err;
}

//4. 配置需要哪些物理量(request_data)
uint8 func_rqdata(uint8 dev, uint8 xml_idx)
{
	uint8 err = NO_ERR;
	printf("[%s][%s][%d]now in func_sysconfig()\n", FILE_LINE);
    switch(g_xml_info[dev].oper_type){
    case em_OPER_RD:
        break;
    case em_OPER_WR:
        err = write_request_data(dev);
        break;
    case em_OPER_ASW:
        
        break;
    default:
        err = ERR_FF;
        break;
    }
	return err;
}
//5. 配置抄表时间点(time_node)
uint8 func_tnode(uint8 dev, uint8 xml_idx)
{
    uint8 retErr = NO_ERR;

    return retErr;
}

uint8 update_meter_info(uint8 dev)
{
    uint8 err = NO_ERR;
    char pErr[100];
    pMeter_info pMInfo;
    xmlDocPtr pDoc = g_xml_info[dev].xmldoc_rd;
    if(NULL == pDoc) {
        printf("[%s][%s][%d] pDoc: %p\n", FILE_LINE, pDoc);
        return ERR_1;
    }
    xmlNodePtr rootNode = xmlDocGetRootElement(pDoc);
    if(NULL == rootNode) {
        printf("[%s][%s][%d] rootNode: %p\n", FILE_LINE, rootNode);
        return ERR_1;
    }
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
    err = send_answer(dev, "result", "success", NULL);
    printf("send_upmeter_answer OK\n");
    if(total_rows == g_xml_info[dev].meter_info_row_idx) {//如果上位机发完, 更新数据库
        printf("now insert_into_meter_info_table():\n");        
        empty_meter_info_table(pErr);
        insert_into_meter_info_table(pErr);
        err = (strlen(pErr)>0 ? ERR_1 : NO_ERR);
        printf("[%s][%s][%d]insert_into_meter_info_table done\n", FILE_LINE);
    }
    return err;
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
        break;
    }
    return err;
}

uint8 get_tm_node(uint8 dev, char* timenode) {
    xmlDocPtr pDoc = g_xml_info[dev].xmldoc_rd;
    if(NULL == pDoc) {
        printf("[%s][%s][%d] pDoc: %p\n", FILE_LINE, pDoc);
        return ERR_1;
    }
    //xmlDocPtr pDoc = xmlParseFile(gXML_File[g_xml_info[dev].xml_rd_file_idx].pXMLFile);
    xmlNodePtr curNode = xmlDocGetRootElement(pDoc);//<root>node
    if(NULL == curNode) {
        printf("[%s][%s][%d] curNode: %p\n", FILE_LINE, curNode);
        return ERR_1;
    }
    curNode = curNode->children;//<common>node
    while(curNode) {
        if(xmlStrEqual(curNode->name, CONST_CAST "time")) {
            strcpy(timenode, (char*)xmlNodeGetContent(curNode->xmlChildrenNode));
        }
        curNode = curNode->next;
    }
    return NO_ERR;
}

//向xmldoc中添加一行的数据节点
int wr_his_xml(pHis_data pHis, uint8 dev)
{
    printf("[%s][%s][%d]pHis : %p\n", FILE_LINE, pHis);
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
        xmlNewTextChild(common_node,NULL,BAD_CAST "sadd",BAD_CAST sysConfig.f_config_value);
        xmlNewTextChild(common_node,NULL,BAD_CAST "oadd",(xmlChar *)g_xml_info[dev].sadd);
        sprintf(str, "%d", em_FUNC_RPTUP);
        xmlNewTextChild(common_node,NULL,BAD_CAST "func_type",(xmlChar *)str);
        sprintf(str, "%d", em_OPER_ASW);//统一改为应答
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
        printf("~~~~~~~~~[%s][%s][%d] cur_frame_indep: %d\n", FILE_LINE, g_xml_info[dev].cur_frame_indep);
        xmlNewTextChild(trans_node, NULL, BAD_CAST "frame_idx", (xmlChar *)str);//本帧的索引
        sprintf(str, "%d", g_xml_info[dev].cur_cnt);
        xmlNewTextChild(trans_node, NULL, BAD_CAST "meter_num", (xmlChar *)str);//本帧共包含多少条抄表数据
        xmlNewTextChild(trans_node, NULL, BAD_CAST "time_node", (xmlChar *)(g_xml_info[dev].timenode));//上报的是哪个时间点的数据
        g_xml_info[dev].cur_wr_state = stat_his_data;
        break;
    case stat_his_data:
        printf("[%s][%s][%d] now enter state: stat_his_data\n", FILE_LINE);
        if(NULL == pHis) {
            printf("[%s][%s][%d]pHis : %p\n", FILE_LINE, pHis);
            g_xml_info[dev].cur_wr_state = stat_his_end;
            return ERR_1;
        }
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
        printf("$$$$[%s][%s][%d], f_meter_type: %s",FILE_LINE,pHis->f_meter_type);//表类型
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
        printf("[%s][%s][%d]insert one row over!!!\n", FILE_LINE);
        if(g_xml_info[dev].cur_row_idx == g_xml_info[dev].cur_cnt) {
            g_xml_info[dev].cur_wr_state = stat_his_end;
            printf(">>>>[%s][%s][%d]<<<<insert all row over!!!\n", FILE_LINE);
            printf(">>>>[%s][%s][%d]<<<<point to next state stat_his_end\n", FILE_LINE);
        }
        break;
    case stat_his_end:
        printf("now in stat_his_end, and write to xmlfile: %s\n", gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile);
        fp = fopen(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile,"w+");
        nRel = xmlSaveFileEnc(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile, g_xml_info[dev].xmldoc_wr,"utf-8");
        fclose(fp);
        if(nRel != -1){
            printf("[%s][%s][%d] make xml %s Index = %d.\n", FILE_LINE, \
                gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile, g_xml_info[dev].xml_wr_file_idx);
            nRel = NO_ERR;
        }else {
            printf("[%s][%s][%d] write history xml failed!!\n", FILE_LINE);
            nRel = ERR_1;
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

    int retSem = 0;
    uint8 lu8times = 0;
    uint8 err = NO_ERR;
    FILE *fp;
    char pErr[100];
    uint8 type_idx;//仪表类型的索引
    
    printf("g_xml_info[dev].timenode: %s\n", g_xml_info[dev].timenode);
    read_all_his_data(g_xml_info[dev].timenode, pErr);
    g_xml_info[dev].cur_frame_indep = 0;
    g_xml_info[dev].total_rows = 0;
    g_xml_info[dev].cur_rows = 0;
    for(type_idx=0;type_idx<MTYPE_CNT;type_idx++) {
        printf("--------------current meter type: %d--------------\n", type_idx);
        printf("--------------current meter get_his_cnt: %d--------------\n", get_his_cnt(type_idx));
        g_xml_info[dev].total_row[type_idx] = get_his_cnt(type_idx);
        g_xml_info[dev].total_rows += g_xml_info[dev].total_row[type_idx];
        g_xml_info[dev].mod[type_idx] = g_xml_info[dev].total_row[type_idx]%ROW_PER_FRAME;
        printf("##########[%s][%s][%d]##########total_row[%d]: %d.\n", FILE_LINE, type_idx, g_xml_info[dev].total_row[type_idx]);
        printf("##########[%s][%s][%d]##########mod[%d]: %d.\n", FILE_LINE, type_idx, g_xml_info[dev].mod[type_idx]);
        g_xml_info[dev].cur_frame[type_idx] = 0;
        g_xml_info[dev].total_frame[type_idx] = (g_xml_info[dev].total_row[type_idx]/ROW_PER_FRAME+((g_xml_info[dev].mod[type_idx])?1:0));
        printf("[%s][%s][%d]total_frame is: >>>%d<<<\n", FILE_LINE, g_xml_info[dev].total_frame[type_idx]);
    }
    if(0 == g_xml_info[dev].total_rows){//如果没有查询到相应时间点的历史数据, 返回0行信息
        g_xml_info[dev].cur_wr_state = stat_his_init;//每次组帧之前都是初始状态
        wr_his_xml(NULL, dev);//写root节点和common节点
        wr_his_xml(NULL, dev);//写trans节点
        wr_his_xml(NULL, dev);//写空row节点, 并将状态指向下一状态
        do{//获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
            g_xml_info[dev].xml_wr_file_idx = Get_XMLBuf();
        }while(g_xml_info[dev].xml_wr_file_idx == ERR_FF);
        wr_his_xml(NULL, dev);//将xmldoc写到文件        
        if(err == NO_ERR) {//发送文件            
            fp = fopen(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile,"r");
            FileSend(dev, fp);
            fclose(fp);
            err = Put_XMLBuf(g_xml_info[dev].xml_wr_file_idx);
        }
        return err;
    }
    
    for(type_idx=0;type_idx<MTYPE_CNT;type_idx++) {        
        printf("--------------[%s][%s][%d]current meter type: %d--------------\n", FILE_LINE, type_idx);
        printf("--------------[%s][%s][%d]current meter get_his_cnt: %d--------------\n", FILE_LINE, get_his_cnt(type_idx));
        printf("--------------[%s][%s][%d]total_row[%d]: %d--------------\n", FILE_LINE, type_idx,  g_xml_info[dev].total_row[type_idx]);
        while(g_xml_info[dev].cur_frame[type_idx]<g_xml_info[dev].total_frame[type_idx]){
            g_xml_info[dev].cur_frame[type_idx]++;
            g_xml_info[dev].cur_frame_indep++;
            g_xml_info[dev].cur_wr_state = stat_his_init;//每次组帧之前都是初始状态
            wr_his_xml(NULL, dev);//写root节点和common节点
            printf("##########[%s][%s][%d]cur_frame[%d]: %d, total_frame[%d]: %d##########\n",FILE_LINE,\
                type_idx, g_xml_info[dev].cur_frame[type_idx], type_idx, g_xml_info[dev].total_frame[type_idx]);
            printf("##########[%s][%s][%d]cur_frame_indep: %d\n",FILE_LINE,\
                g_xml_info[dev].cur_frame_indep);
            
            if((g_xml_info[dev].cur_frame[type_idx] == g_xml_info[dev].total_frame[type_idx]) && (g_xml_info[dev].mod[type_idx]>0))
               g_xml_info[dev].cur_cnt = g_xml_info[dev].mod[type_idx];//如果当前帧是最后一帧, 且余数大于0, 那么等于余数
            else
               g_xml_info[dev].cur_cnt = ROW_PER_FRAME;//否则等于约定的行数

            printf("[%s][%s][%d]current frameidx: %d, cur_cnt: %d, mod: %d\n", FILE_LINE, g_xml_info[dev].cur_frame_indep, \
                g_xml_info[dev].cur_cnt, g_xml_info[dev].mod[type_idx]);
		
            wr_his_xml(NULL, dev);//写trans节点
            g_xml_info[dev].cur_row_idx = 0;
            printf("[%s][%s][%d]current row count: %d\n", FILE_LINE, g_xml_info[dev].cur_cnt);
            retrieve_and_del_his_data(type_idx, g_xml_info[dev].cur_cnt, wr_his_xml, dev);//写row节点, 查询完就删除, 以免下一帧从头开始查

            do{//获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
                g_xml_info[dev].xml_wr_file_idx = Get_XMLBuf();
            }while(g_xml_info[dev].xml_wr_file_idx == ERR_FF);
            printf("[%s][%s][%d]write xml to file: %s\n", FILE_LINE, gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile);
            wr_his_xml(NULL, dev);//将xmldoc写到文件
            
            for(lu8times=0;lu8times<5;lu8times++){
            		if(err == NO_ERR) {//发送文件
            		    printf("[%s][%s][%d] xml_wr_file_idx'name: %s\n", FILE_LINE, gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile);
                		fp = fopen(gXML_File[g_xml_info[dev].xml_wr_file_idx].pXMLFile,"r");
                		FileSend(dev, fp);
                        FileSend(UP_COMMU_DEV_485, fp);
                		fclose(fp);
            		}
		  
		  		retSem = OSSem_timedwait(&His_asw_sem,10*OS_TICKS_PER_SEC);
		  		if(retSem != 0){
					printf("data report fail,times=%d .\n",lu8times+1);
					continue;
				}
				else				
					break; //如果发送后收到应答，则跳出继续发送下一帧。
            }
            Put_XMLBuf(g_xml_info[dev].xml_wr_file_idx);//发送完成, 释放占用的xml文件
            xmlFreeDoc(g_xml_info[dev].xmldoc_wr);//发送完成, 释放占用的xml内存
            printf("[%s][%s][%d] free write xml succeed!!\n", FILE_LINE);
            g_xml_info[dev].cur_rows += g_xml_info[dev].cur_cnt;//当前已发送的行数
            printf("[%s][%s][%d] cur_frame %d cur_frame_indep %d, make xml over!!!!!!!!\n", FILE_LINE,\
            g_xml_info[dev].cur_frame[type_idx], g_xml_info[dev].cur_frame_indep);
            printf("[%s][%s][%d] cur_meter_type: %d, cur_frame: ^^^^^>>%d^^^^^<<, total_frame: ^^^^^>>%d^^^^^<<\n", FILE_LINE, \
                type_idx, g_xml_info[dev].cur_frame[type_idx], g_xml_info[dev].total_frame[type_idx]);
            if(retSem != 0){  //如果连续发送不成功，则退出，不再上推。
                printf("data report fail, send quit .\n");
                empty_all_hisdata();
                break;
            }
        }
    }
    printf("[%s][%s][%d] all frame make xml over!!!!!!!!\n", FILE_LINE);
    err = empty_all_hisdata();
    return err;
}

uint8 set_xml_timenode(uint8 dev, char* timenode)
{
    uint8 err = NO_ERR;
    strcpy(g_xml_info[dev].timenode, timenode);
    return err;
}


//7. 抄表历史数据上传
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

//8. 读取当前状态
uint8 func_rdstat(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

//9. 切换IP
uint8 func_swip(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

//10. 数据库sql语句执行
uint8 func_dbmani(uint8 dev, uint8 xml_idx)
{
		uint8 retErr = NO_ERR;
		
		return retErr;
}

uint8 do_shell_cmd(uint8 dev)
{
    uint8 err = NO_ERR;
    xmlNodePtr rootNode = xmlDocGetRootElement(g_xml_info[dev].xmldoc_rd);
    if(rootNode == NULL)
        return ERR_1;
    xmlNodePtr cmdNode = rootNode->children;
    char result[LENGTH_SYS_ANSWER] = {0};
    char cmd[50] = {0};
    xmlChar* pValue;
    while(cmdNode) {
        if(xmlStrEqual(cmdNode->name, CONST_CAST "local_command")) {
            pValue = xmlNodeGetContent(cmdNode->xmlChildrenNode);
            strcpy(cmd, (char*)pValue);
            break;
        }
        cmdNode = cmdNode->next;
    }
    
    FILE *fstream=NULL;
    if(NULL==(fstream=popen(cmd,"r"))) {
        fprintf(stderr,"execute command failed: %s",strerror(errno));
        return ERR_1;
    }
    if(NULL!=fgets(result, sizeof(result), fstream)) { 
        printf("%s\n",result);
    } else
    {
        err = ERR_1;
    }
    pclose(fstream);
    err = send_answer(dev, "exec_result", result, NULL);
    return err;
}
//11. 执行shell命令
uint8 func_syscmd(uint8 dev, uint8 xml_idx)
{
	uint8 retErr = NO_ERR;
    switch(g_xml_info[dev].oper_type) {
    case em_OPER_RD:
        break;    
    case em_OPER_WR:
        break;    
    case em_OPER_DO:
        retErr = do_shell_cmd(dev);
        break;    
    case em_OPER_ASW:
        break;
    default:
        break;
    }
	return retErr;
}

void empty_update_list(uint8 dev)
{
    int idx;//上次异常更新时的总帧数
    if(g_xml_info[dev].pDataList != NULL){
        //先释放每帧的数据空间
        for(idx=0; idx<g_xml_info[dev].up_total_frm;idx++) {
            if(g_xml_info[dev].pDataList[idx]) {
                free(g_xml_info[dev].pDataList[idx]);
            }
        }
        //最后释放指针数组的空间
        free(g_xml_info[dev].pDataList);
        g_xml_info[dev].pDataList = NULL;
    }
}

uint8 malloc_space(uint8 dev)
{
    int listLen = g_xml_info[dev].up_total_frm*sizeof(uint8*);
    g_xml_info[dev].pDataList = malloc(listLen);
    if(g_xml_info[dev].pDataList == NULL)
        return ERR_1;
    memset(g_xml_info[dev].pDataList, 0, listLen);
    listLen = g_xml_info[dev].up_total_frm*sizeof(int);
    printf("[%s][%s][%d]pDataLen's length: %d\n", FILE_LINE, listLen);
    g_xml_info[dev].pDataLen = malloc(listLen);
    if(g_xml_info[dev].pDataLen == NULL)
        return ERR_1;
    memset(g_xml_info[dev].pDataLen, 0, listLen);
    return NO_ERR;
}

uint8 update_bin(uint8 dev)
{
    uint8 err = NO_ERR;
    xmlNodePtr rootNode      = NULL;
    xmlNodePtr curNode       = NULL;
    xmlNodePtr transInfoNode = NULL;
    xmlNodePtr binNode       = NULL;
    sys_config_str sysconfig;
    uint16 crc;//上位机发来的是低位在前, 赋值时要交换为高位在前, 便于比较
    uint16 localcrc;
    xmlChar* pValue;
    char binValue[2048]={0};//初始化为{\0}.临时存储base64字符串, 以防上位机发来的字符串中有换行符等
    int binValueLen=0;
    char* pChar = binValue;
    int i=0;
    uint32 enLen=0;//当前帧编码后的字符串长度
    uint32 deLen=0;//当前帧解码后的字节串长度
    rootNode = xmlDocGetRootElement(g_xml_info[dev].xmldoc_rd);
    if(NULL == rootNode)
        return ERR_1;
    curNode = rootNode->children;//commonNode
    while(curNode) {//找到trans节点
        if(xmlStrEqual(curNode->name, CONST_CAST "trans")) {
            transInfoNode = curNode;
            break;
        }
        curNode = curNode->next;
    }
    printf("[%s][%s][%d]transInfoNode: %p\n",FILE_LINE, transInfoNode);
    if(transInfoNode == NULL)
        return ERR_1;
    
    printf("[%s][%s][%d]transInfoNode->name: %s\n",FILE_LINE, transInfoNode->name);
    transInfoNode = transInfoNode->children;
    while(transInfoNode) {
        printf("[%s][%s][%d]transNode->name: %s\n",FILE_LINE, transInfoNode->name);
        if(xmlStrEqual(transInfoNode->name, CONST_CAST "fb")) {//总共要下发的文件字节数量
            pValue = xmlNodeGetContent(transInfoNode->xmlChildrenNode);
            g_xml_info[dev].up_total_bytes = atoi((char*)pValue);
        } else if(xmlStrEqual(transInfoNode->name, CONST_CAST "frmc")) {//一共下发多少帧数
            pValue = xmlNodeGetContent(transInfoNode->xmlChildrenNode);
            g_xml_info[dev].up_total_frm = atoi((char*)pValue);
        } else if(xmlStrEqual(transInfoNode->name, CONST_CAST "md5")) {//本次更新的程序的md5值
            pValue = xmlNodeGetContent(transInfoNode->xmlChildrenNode);
            printf("[%s][%s][%d]server's md5: %s\n",FILE_LINE, (char*)pValue);
            get_sys_config(CONFIG_APP_MD5, &sysconfig);
            printf("[%s][%s][%d]local's md5: %s\n",FILE_LINE, sysconfig.f_config_value);
            if(strcmp(sysconfig.f_config_value, (char*)pValue) == 0) {//如果md5值一样, 那么返回"md5same"应答, 终止升级过程
                send_answer(dev, "md5", "same", NULL);
                return NO_ERR;
            }
            strcpy(g_xml_info[dev].up_md5, (char*)pValue);
        } else if(xmlStrEqual(transInfoNode->name, CONST_CAST "frmid")) {//本帧的索引号
            pValue = xmlNodeGetContent(transInfoNode->xmlChildrenNode);
            g_xml_info[dev].up_cur_frm_idx = (atoi((char*)pValue)-1);
            printf("[%s][%s][%d]up_cur_frm_idx: %d\n",FILE_LINE, g_xml_info[dev].up_cur_frm_idx);
        } else if(xmlStrEqual(transInfoNode->name, CONST_CAST "bc")) {//本帧的字节数
            pValue = xmlNodeGetContent(transInfoNode->xmlChildrenNode);
            g_xml_info[dev].up_cur_bytes = atoi((char*)pValue);
        } else if(xmlStrEqual(transInfoNode->name, CONST_CAST "ck")) {//本帧的字节串的CRC校验值
            pValue = xmlNodeGetContent(transInfoNode->xmlChildrenNode);
            crc = (Ascii2Hex(pValue[3])<<3*LEN_HALF_BYTE|\
                    Ascii2Hex(pValue[4])<<2*LEN_HALF_BYTE|\
                    Ascii2Hex(pValue[0])<<LEN_HALF_BYTE|\
                    Ascii2Hex(pValue[1]));
            printf("[%s][%s][%d]server's crc is: %04X\n",FILE_LINE, crc);
        }
        transInfoNode = transInfoNode->next;
    }

    if(g_xml_info[dev].up_cur_frm_idx == -1) {//如果是第0帧, 则初始化暂存空间, 完成后就退出
        empty_update_list(dev);
        if((err = malloc_space(dev)) ==ERR_1) {//任意时刻, 如果集中器申请不到内存, 向上位机发送终止升级的消息
            send_answer(dev, "malloc", "fail", NULL);
        } else {
            send_answer(dev, "md5", "diff", NULL);
        }
        return err;
    }

    if(g_xml_info[dev].pDataList == NULL)//每次都要检测数据列表是否申请到了内存, 或者是否接收过第0帧, 因为第0帧时会申请内存
        return ERR_1;
    printf("[%s][%s][%d]pDataList is not NULL\n",FILE_LINE);

    //得到本帧的数据
    curNode = rootNode->children;//commonNode
    while(curNode) {//找到trans节点
        if(xmlStrEqual(curNode->name, CONST_CAST "bin")) {
            binNode= curNode;
            break;
        }
        curNode = curNode->next;
    }
    pValue = xmlNodeGetContent(binNode->xmlChildrenNode);
    enLen = strlen((char*)pValue);
    for(i=0;i<enLen;i++) {
        if(idx_of_base64(pValue[i]) < 65) {//如果是base64编码以内的字符, 追加到binValue
            *pChar++ = pValue[i];
            binValueLen++;
        }
    }
    printf("[%s][%s][%d]binValueLen: %d", FILE_LINE, binValueLen);
    if((binValueLen%4)==0) {
        deLen = (3*binValueLen/4 - cnt_of_pad(binValue, binValueLen));
    } else {
        goto lenErr;
    }
    
    printf("[%s][%s][%d]deLen: %d, up_cur_bytes: %d\n",FILE_LINE, deLen, \
        g_xml_info[dev].up_cur_bytes);
    if(deLen != g_xml_info[dev].up_cur_bytes) {//如果长度不一致, 清空数据缓存, 返回错误
lenErr:
        free(g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx]);
        g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx] = NULL;
        g_xml_info[dev].pDataLen[g_xml_info[dev].up_cur_frm_idx] = 0;
        return ERR_1;
    }
    g_xml_info[dev].pDataLen[g_xml_info[dev].up_cur_frm_idx] = deLen;
    //当下发的字节长度与计算出的字节长度一致时, 申请暂存空间
    if(g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx]!=NULL)
        free(g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx]);
    g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx] = malloc(deLen);
    printf("[%s][%s][%d]\n",FILE_LINE);
    //任意时刻, 如果集中器申请不到内存, 向上位机发送终止升级的消息
    if(g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx] == NULL) {
        send_answer(dev, "malloc", "fail", NULL);
        return ERR_1;
    }
    err = decode_base64(binValue, binValueLen, \
        g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx]);
    printf("[%s][%s][%d]\n",FILE_LINE);
    //如果crc校验不通过则释放内存; 通过则等待下一帧传输
    localcrc=crc16ModRtu(g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx], deLen);
    printf("[%s][%s][%d]crc: %04X, localcrc: %04X\n", FILE_LINE, crc, localcrc);
    if(crc != localcrc) {
        free(g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx]);        
        g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx] = NULL;
        return ERR_1;
    }
    for(crc=0;crc<deLen;crc++) {
        printf("%02X ",g_xml_info[dev].pDataList[g_xml_info[dev].up_cur_frm_idx][crc]);
    }
    printf("\n");
    printf("[%s][%s][%d]\n",FILE_LINE);
    return err;
}

uint8 send_lack_frame(uint8 dev)
{
    uint8 err = NO_ERR;
    int multi_times = (g_xml_info[dev].up_total_frm/LENGHTH_LACK_STRING+1);
    int lackLen = multi_times*LENGHTH_LACK_STRING*sizeof(char);
    char* lackStr = malloc(lackLen);//缺帧列表
    memset(lackStr, 0, lackLen);
    int lack_idx=0;
    char tmpstr[6];

    for(lack_idx=0;lack_idx<g_xml_info[dev].up_total_frm;lack_idx++) {
        if(g_xml_info[dev].pDataList[lack_idx]==NULL){
            sprintf(tmpstr, "%d,", lack_idx+1);
            strcat(lackStr, tmpstr);
        }
    }
    send_answer(dev, "lack", lackStr, NULL);
    free(lackStr);
    return err;
}

uint8 merge_update_file(uint8 dev)
{
    uint8 err = NO_ERR;
    FILE *fp;
    int idx;
    int wLen;//要写入的字节数
    int acwLen;//实际写入的字节数
    char cmd[100]={0};//调用shell命令
    char filename[50]={0};
    char result[100]={0};//shell命令的结果
    char* sp;
    sys_config_str sys_config;
    int lack_idx=0;

    for(lack_idx=0;lack_idx<g_xml_info[dev].up_total_frm;lack_idx++) {
        if(g_xml_info[dev].pDataList[lack_idx]==NULL){
            send_lack_frame(dev);//如果有漏掉的帧, 向上位机返回缺帧列表
            return ERR_1;
        }
    }

    strcpy(filename, APP_TMPNAME);
    strcat(filename, ".tar.bz2");
    fp = fopen(filename, "wb+");
    for(idx=0;idx<g_xml_info[dev].up_total_frm;idx++) {
        if(g_xml_info[dev].pDataList[idx] != NULL) {
            wLen = g_xml_info[dev].pDataLen[idx];
            acwLen = fwrite(g_xml_info[dev].pDataList[idx], 1, wLen, fp);
            printf("[%s][%s][%d]wLen: %d, acwLen: %d\n",FILE_LINE, wLen, acwLen);
            
            if(acwLen != wLen) {
                //send_answer(dev, "merge", "fail", NULL);
                //empty_update_list(dev);
                fclose(fp);
                //删除压缩包
                strcpy(cmd, "rm ");
                strcat(cmd, filename);
                system(cmd);
                send_lack_frame(dev);//向上位机重新要数据
                return ERR_1;
            }
            free(g_xml_info[dev].pDataList[idx]);//写一帧就释放一帧
            g_xml_info[dev].pDataList[idx] = NULL;
        } else {//有缺帧的时候, 重新要数据
            send_lack_frame(dev);//向上位机重新要数据
            return ERR_1;
        }
    }
    fclose(fp);
    //解压缩临时文件
    strcpy(cmd, "tar xjvf ");
    strcat(cmd, filename);
    system(cmd);    
    //删除压缩包
    strcpy(cmd, "rm ");
    strcat(cmd, filename);
    system(cmd);

    //对比临时文件的MD5值
    strcpy(cmd, "md5sum ");
    strcat(cmd, APP_TMPNAME);
    if(NULL==(fp=popen(cmd, "r"))) {
        fprintf(stderr, "execute command failed: %s", strerror(errno));
        send_answer(dev, "merge", "fail", NULL);
        return ERR_1;
    }
    if(NULL!=fgets(result, sizeof(result), fp)) {
        printf("%s\n",result);
    } else {
        send_answer(dev, "merge", "fail", NULL);
        return ERR_1;
    }
    pclose(fp);
    printf("[%s][%s][%d]\n",FILE_LINE);
    strtok_r(result, " ", &sp);//把md5值从得到的执行结果中分离出来
    printf("[%s][%s][%d]\n",FILE_LINE);
    if(strcmp(g_xml_info[dev].up_md5, result) != 0) {//MD5值不一致, 返回错误
        send_answer(dev, "merge", "fail", NULL);
        return ERR_1;
    }
    //删除老程序
    strcpy(cmd, "rm ");
    strcat(cmd, APP_NAME);
    system(cmd);
    //将新升级的临时程序改名为原程序名
    strcpy(cmd, "mv ");
    strcat(cmd, APP_TMPNAME);
    strcat(cmd, " ");
    strcat(cmd, APP_NAME);
    system(cmd);
    //将新程序的MD5值存入数据库
    sys_config.f_id = CONFIG_APP_MD5;
    get_sys_config(CONFIG_APP_MD5, &sys_config);
    strcpy(sys_config.f_config_value, g_xml_info[dev].up_md5);
    err = add_one_config(&sys_config, NULL);

    g_xml_info[dev].pDataList = NULL;
    send_answer(dev, "merge", "success", NULL);
    //重启Linux
    system("reboot");
    return err;
}

//12. 远程升级
uint8 func_codeup(uint8 dev, uint8 xml_idx)
{
	uint8 retErr = NO_ERR;    
    switch(g_xml_info[dev].oper_type) {
    case em_OPER_RD:
        send_lack_frame(dev);
        break;    
    case em_OPER_WR:
        update_bin(dev);
        break;
    case em_OPER_DO:
        merge_update_file(dev);
        break;    
    case em_OPER_ASW:
        break;
    default:
        break;
    }
	return retErr;
}

uint8 split_com(char* comstr, pProto_trans pProtoTrs)//comstr = "9600,n,8,1"
{
    int i=0;
    char *p[5]={NULL};
    char *buf=comstr;
    char *sp=NULL;
    printf("[%s][%s][%d]comstr: %s\n", FILE_LINE, comstr);
    while((p[i] = strtok_r(buf, ",", &sp)))
    {   
        printf("[%s][%s][%d] p[i]: %s \n", FILE_LINE, p[i]);
        i++;
        buf=NULL;
    }
    pProtoTrs->baud = atoi(p[0]);
    pProtoTrs->parity = p[1][0];
    pProtoTrs->databits = atoi(p[2]);
    pProtoTrs->stop = atoi(p[3]);

    printf("[%s][%s][%d]baud: %d, parity: %c, databits: %d, stop: %d, \n", FILE_LINE, \
        pProtoTrs->baud, pProtoTrs->parity, pProtoTrs->databits, pProtoTrs->stop);
    return NO_ERR;
}

uint8 split_minfo(char* info, pProto_trans pProtoTrs)
{
    int i=0;
    char *p[3]={NULL};
    char *buf=info;
    char *sp=NULL;
    printf("[%s][%s][%d]info: %s\n", FILE_LINE, info);
    while((p[i] = strtok_r(buf, ",", &sp)))
    {   
        printf("[%s][%s][%d] p[i]: %s \n", FILE_LINE, p[i]);
        i++;
        buf=NULL;
    }
    pProtoTrs->meter_type = (Ascii2Hex(p[0][0]) << LEN_HALF_BYTE | Ascii2Hex(p[0][1]));
    pProtoTrs->proto_type = atoi(p[1]);

    printf("[%s][%s][%d]meter_type: %02x, proto_type: %d\n", FILE_LINE, \
        pProtoTrs->meter_type, pProtoTrs->proto_type);
    return NO_ERR;
}

uint8 split_cmd(char* cmd, pProto_trans pProtoTrs)
{
    int i=0, j;
    char *p[512]={NULL};
    char *buf=cmd;
    char *sp=NULL;
    printf("[%s][%s][%d]cmd: %s\n", FILE_LINE, cmd);
    while((p[i] = strtok_r(buf, " ", &sp)))
    {
        printf("[%s][%s][%d] p[i]: %s \n", FILE_LINE, p[i]);
        i++;
        buf=NULL;
    }
    pProtoTrs->cmdlen = i;
    printf("[%s][%s][%d]cmdlen: %d\n", FILE_LINE, pProtoTrs->cmdlen);
    for(j=0;j<i;j++) {
         pProtoTrs->cmd[j] = (Ascii2Hex(p[j][0]) << LEN_HALF_BYTE | Ascii2Hex(p[j][1]));
         printf("%02x \n", pProtoTrs->cmd[j]);
    }
    
    return NO_ERR;
}


uint8 parse_proto_trs(uint8 dev, pProto_trans pProtoTrs)
{
    uint8 retErr = NO_ERR;
    xmlChar* pValue;
    if(g_xml_info[dev].xmldoc_rd == NULL)
        return ERR_1;
    
    xmlNodePtr rootNode = xmlDocGetRootElement(g_xml_info[dev].xmldoc_rd);
    if(rootNode == NULL)
        return ERR_1;

    xmlNodePtr curNode = rootNode->children;
    while(curNode) {
        if(xmlStrEqual(curNode->name, CONST_CAST "channel")) {
            pValue = xmlNodeGetContent(curNode->xmlChildrenNode);
            pProtoTrs->channel = atoi((char*)pValue);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "com")) {
            pValue = xmlNodeGetContent(curNode->xmlChildrenNode);
            retErr = split_com((char*)pValue, pProtoTrs);
            
        } else if(xmlStrEqual(curNode->name, CONST_CAST "cmd")) {
            pValue = xmlNodeGetContent(curNode->xmlChildrenNode);
            retErr = split_cmd((char*)pValue, pProtoTrs);
        } else if(xmlStrEqual(curNode->name, CONST_CAST "meterinfo")) {
            pValue = xmlNodeGetContent(curNode->xmlChildrenNode);
            retErr = split_minfo((char*)pValue, pProtoTrs);
        }
        curNode = curNode->next;
    }
    return retErr;
}

uint8 send_cmd_and_rcv(uint8 dev, proto_trans_str* pProtoTrs)
{    
    if(pProtoTrs->channel<0 || pProtoTrs->channel>RS485_DOWN_CHANNEL) {
        return ERR_1;
    }
    uint8 retErr = NO_ERR;
    uint8 down_dev;
    uint8 lu8data = 0;
    int32 fd;
    
    if(pProtoTrs->channel == RS485_DOWN_CHANNEL){//操作一个设备，先请求信号量,谨防冲突。
        down_dev = DOWN_COMMU_DEV_485;
        fd = g_uiRS4852Fd;
        sem_wait(&Opetate485Down_sem);
        gu8485DownComSetIndex = 0;
        gu32Down485BaudRate = pProtoTrs->baud;
    } else {
        down_dev = DOWN_COMMU_DEV_MBUS;
        fd = g_uiMbusFd;
        sem_wait(&OperateMBUS_sem);
        gu8MbusComSetIndex = 0;
        gu32MbusBaudRate = pProtoTrs->baud;
    }
    printf("\n[%s][%s][%d] g_uiRS4852Fd: %d, g_uiMbusFd: %d, fd: %d \n",FILE_LINE,g_uiRS4852Fd, g_uiMbusFd, fd);
    METER_ChangeChannel(pProtoTrs->channel);  //先确保在对应通道上。
    uint8 lu8retrytimes = 3;
    do{//设置串口参数
        retErr = set_com_para(fd,pProtoTrs->baud,pProtoTrs->databits,pProtoTrs->stop,pProtoTrs->parity);
        lu8retrytimes--;
    }while((retErr != TRUE) && (lu8retrytimes > 0));
    QueueFlush((void*)pQueues[down_dev]);
    /*printf("[%s][%s][%d] after flush queue: \n", FILE_LINE);
    int i=0;    
    DataQueue *Queue = (DataQueue *)pQueues[down_dev];
    printf("[%s][%s][%d] queue max length: %d\n", FILE_LINE, Queue->MaxData);
    for(i=0;i<Queue->MaxData; i++)
        printf("i: %d, data: %02x; ", i, Queue->Buf[i]);
    printf("\n");*/
    DownDevSend(down_dev, pProtoTrs->cmd, pProtoTrs->cmdlen);
    memset(pProtoTrs->res, 0, sizeof(pProtoTrs->res));
    pProtoTrs->resLen = 0;
    while(((retErr=DownDevGetch(down_dev, &lu8data, REC_TIMEOUT_SEC*500)) == NO_ERR) \
        && (pProtoTrs->resLen < sizeof(pProtoTrs->res))){
        pProtoTrs->res[pProtoTrs->resLen] = lu8data;
        pProtoTrs->resLen++;
    }

    if(pProtoTrs->channel == RS485_DOWN_CHANNEL){//操作一个设备，先请求信号量,谨防冲突。
        sem_post(&Opetate485Down_sem);
    } else {
        sem_post(&OperateMBUS_sem);
    }
    return retErr;
}

uint8 do_proto_trs(uint8 dev)
{    
    uint8 retErr = NO_ERR;
    proto_trans_str proto_trs;
    char resData[3*PROTO_RES_LEN+1] = {0};//用于返回抄表结果, 由于1个字节对应2个字符, 再加上1个空格, 所以用3倍
    char tmpStr[3];
    int i;

    retErr = parse_proto_trs(dev, &proto_trs);
    retErr = send_cmd_and_rcv(dev, &proto_trs);
    int readidx;
    printf("[%s][%s][%d]proto_trs.resLen: %d \n", FILE_LINE, proto_trs.resLen);
    for(readidx=0;readidx<proto_trs.resLen;readidx++)
      printf("0x%02X ",proto_trs.res[readidx]);
    printf("[%s][%s][%d]",FILE_LINE);

    for(i=0;i<proto_trs.resLen;i++){
        sprintf(tmpStr, "%02X ", proto_trs.res[i]);
        strcat(resData, tmpStr);
    }
    retErr = send_answer(dev, "result", resData, NULL);
    return retErr;
}

//13. 命令透传
uint8 func_prototrs(uint8 dev, uint8 xml_idx)
{
    uint8 retErr = NO_ERR;
    switch(g_xml_info[dev].oper_type) {
    case em_OPER_RD:
        break;    
    case em_OPER_WR:
        break;    
    case em_OPER_DO:
        retErr = do_proto_trs(dev);
        break;    
    case em_OPER_ASW:
        break;
    default:
        break;
    }
    
    return retErr;
}

uint8 write_systime(uint8 dev)
{
    uint8 err = NO_ERR;
    xmlChar* pValue;
    char timestr[50];
    strcpy(timestr, "date -s ");
    if(g_xml_info[dev].xmldoc_rd == NULL)
        return ERR_1;
    
    xmlNodePtr rootNode = xmlDocGetRootElement(g_xml_info[dev].xmldoc_rd);
    if(rootNode == NULL)
        return ERR_1;
        
    xmlNodePtr curNode = rootNode->children;
    while(curNode) {
        if(xmlStrEqual(curNode->name, CONST_CAST "time")) {
            pValue = xmlNodeGetContent(curNode->xmlChildrenNode);
            strcat(timestr, "\"");
            strcat(timestr, (char*)pValue);
            strcat(timestr, "\"");
        }
        curNode = curNode->next;
    }
    printf("[%s][%s][%d]timestr: %s\n", FILE_LINE, timestr);
    system(timestr);
    system("hwclock -w");
    system("date");
    err = send_answer(dev, "result", "success", NULL);
    return err;    
}
//14. 设置集中器时钟(clock_set)
uint8 func_clock_set(uint8 dev, uint8 xml_idx)
{
    uint8 err = NO_ERR;
    switch(g_xml_info[dev].oper_type){
    case em_OPER_RD:
        break;
    case em_OPER_WR:
        err = write_systime(dev);
        break;
    case em_OPER_ASW:
        break;
    default:
        err = ERR_FF;
        break;
    }
    return err;

}

//15. 集中器抄表模式和抄表周期设置
uint8 func_collect_set(uint8 dev, uint8 xml_idx)
{
    uint8 err = NO_ERR;
    switch(g_xml_info[dev].oper_type){
    case em_OPER_RD:
        break;
    case em_OPER_WR:
        err = write_sysconfig(dev);
        break;        
    case em_OPER_ASW:
        break;
    default:
        err = ERR_FF;
        break;
    }
    return err;
}

uint8 xml_exec(uint8 dev, uint8 xml_idx)
{
	if(dev<0 || dev >= UP_COMMU_DEV_ARRAY){
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
             printf("[%s][%s][%d] func_type: %d\n", FILE_LINE, g_xml_info[dev].func_type);
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
        printf("[%s][%s][%d]CONFIG_GATEWAY_ID : %s\n", FILE_LINE, sysConfig.f_config_value);
        if(0 == strcmp((char *)sysConfig.f_config_value, (char *)g_xml_info[dev].oadd)){//只有是发给本集中器的数据才处理。
            printf("[%s][%s][%d] start  xml_exec().\n", FILE_LINE);
            g_xml_info[dev].xml_rd_file_idx = xml_idx;
            retErr = xml_exec(dev, xml_idx);
            printf((retErr==NO_ERR)?"parse xml success\n":"parse xml fail\n");
            //在此处解析xml文件已经是在执行完相应的函数后了, 不起到任何作用, g_xml_info[dev].xmldoc_rd在此前一直为NULL
            //g_xml_info[dev].xmldoc_rd = xmlParseFile(gXML_File[xml_idx].pXMLFile);
        }
        else{
            //不是给本集中器的, 舍弃。
            printf("[%s][%s][%d]not this gateway!\n", FILE_LINE);
            memset(&g_xml_info[dev], 0, sizeof(xml_info_str));
        }
    }
    xmlFreeDoc(g_xml_info[dev].xmldoc_rd);//释放xml树结构
    printf("[%s][%s][%d]current file name : %s\n", FILE_LINE, gXML_File[g_xml_info[dev].xml_rd_file_idx].pXMLFile);
    return retErr;
}

uint8 parse_xml(uint8 dev, uint8 xml_idx)
{
    
	if(dev >= UP_COMMU_DEV_ARRAY){
		printf("[%s][%s][%d] dev num error.\n", FILE_LINE);
		return ERR_1;
	}
	
	if(dev == UP_COMMU_DEV_GPRS)
			UpdGprsRunSta_FeedSndDog();  //有任何GPRS网络数据收到，都认为网络正常，清空GPRS心跳计数。如果连续几次心跳都不清空，则GPRS重启。
	
	//sem_wait(fd's xml_info_str read semaphore)
	printf("[%s][%s][%d]current file name : %s\n", FILE_LINE, gXML_File[xml_idx].pXMLFile);
	xmlDocPtr doc;
	xmlNodePtr cur;
	uint8 retErr;
	doc = xmlParseFile(gXML_File[xml_idx].pXMLFile);//简单的xml文件用xmlParseFile()
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", gXML_File[xml_idx].pXMLFile);
        return ERR_1;
    }
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
