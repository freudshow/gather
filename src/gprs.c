/*
  *******************************************************************************
  * @file    gprs.c 
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

#include "gprs.h"


//定义说明：Ats_: AT发送命令,Ata_: AT应答命令
char Ats_CGMM[]="AT+CGMM\r";
/*begin:yangfei modified 2013-01-15*/
char Ata_CGMM_xmz_R3[]="\r\nMC52iR3\r\n\r\nOK\r\n";
char Ata_CGMM_xmz[]="\r\nMC52i\r\n\r\nOK\r\n";
/*end:yangfei modified 2013-01-15*/
char Ats_SMSO[]="AT^SMSO\r";
char Ata_SMSO[]="\r\n^SMSO: MS OFF\r\n";

//创建connecting profile:
//at^sics=0,conType,GPRS0
//at^sics=0,apn,cmnet
char Ats_SICS_conType[]="at^sics=0,conType,GPRS0\r";
char Ats_SICS_apn[256]="at^sics=0,apn,cmnet\r";
 // char Ats_SICS_apn[256]="at^sics=0,apn,uninet\r";

///创建tcp service profile
//at^siss=1,srvType,socket
char Ats_SISS_srvType[]="at^siss=1,srvType,socket\r";
 
//at^siss=1,conId,0
char Ats_SISS_conId[]="at^siss=1,conId,0\r";

//at^siss=1,address,"socktcp://117.22.67.183:5100"
char Ats_SISS_address_h[]="at^siss=1,address,\"socktcp://";//117.22.67.183:5100\r\"";
char Ats_SISS_address[96];
char IpAddrXmz[64]="113.132.156.2";//117.22.0.121  
//char IpAddrXmz[64]="1.85.185.244";
char IpPort[16]="1147";
char colon[]=":";

//打开TCP连接
//AT^SISO=1 
char Ats_SISO[]="AT^SISO=1\r";//AT^SISO=1 
char Ats_SISI[]="AT^SISI=1\r";//AT^SISI=1 
char Ata_SISI[]="\r\n^SISI: 1,4,0,0,0,0\r\n\r\nOK\r\n";
char Ata_SISI_1[]="\r\n^SISI: 1,4,";//"\r\n^SISI: 1,4,0,0,0,0\r\n\r\nOK\r\n";

char Ats_SISW_h[]="AT^SISW=1,";//at^sisw=1,30\r
char Ats_SISW[64];
char Ata_SISW_m[]="\r\n^SISW: 1, ";//^SISW: 1, 20, 20

char Ata_URC_SISR[]="\r\n^SISR: 1, 1\r\n";//^SISW: 1, 1\r\n
char Ats_SISR[]="AT^SISR=1,1500\r";
char Ata_SISR_m[]="\r\n^SISR: 1, ";//


char Ats_AT[]="AT\r";
char Ata_OK[]="\r\nOK\r\n";
char Ata_OK123[]="\r\n\r\nOK\r\n";
//char Ata_OK125[]="\r\r\n\r\nOK\r\n";
char Ats_SSYNC[]="AT^SSYNC=1\r";

char Ats_SSYNC_c[]="AT^SSYNC?\r";
char Ata_SSYNC_c[]="\r\n^SSYNC: 1\r\n\r\nOK\r\n";

//char Ats_IPR[]="AT+IPR=9600\r";
//char Ats_IPR[]="AT+IPR=57600\r";
char Ats_IPR[]="AT+IPR=115200\r";
char Ats_IPR_0[]="AT+IPR=0\r";

char Ats_IPR_c[]="AT+IPR?\r";
//char Ata_IPR_c[]="\r\n+IPR: 9600\r\n\r\nOK\r\n";
//char Ata_IPR_c[]="\r\n+IPR: 57600\r\n\r\nOK\r\n";
char Ata_IPR_c[]="\r\n+IPR: 115200\r\n\r\nOK\r\n";

char Ats_CPIN[]="AT+CPIN\?\r";////检查 SIM 工作是否正常
char Ata_CPIN[]="\r\n+CPIN: READY\r\n\r\nOK\r\n";

char Ats_CREG[]="AT+CREG\?\r";////检查 GSM 网络注册情况
char Ata_CREG[]="\r\n+CREG: 0,1\r\n\r\nOK\r\n"; //本地
char Ata_CREG_Or[]="\r\n+CREG: 0,5\r\n\r\nOK\r\n";//漫游

char Ats_CGREG_1[]="AT+CGREG=1\r"; //


char Ats_CGATT_1[]="AT+CGATT\?\r";
char Ata_CGATT_1[]="+CGATT: 1\r\n\r\nOK\r\n";
char Ats_CGATT[]="AT+CGATT=1\r";
//char Ats_CGAUTO[]="AT+CGAUTO=1\r";

char Ats_CGREG[]="AT+CGREG\?\r"; //检查 GPRS 网络注册情况
char Ata_CGREG[]="\r\n+CGREG: 0,1\r\n\r\nOK\r\n";//本地
char Ata_CGREG_Or[]="\r\n+CGREG: 0,5\r\n\r\nOK\r\n";//漫游

char Ata_CGREG_yf[]="\r\n+CGREG: 1,1\r\n\r\nOK\r\n";//本地
char Ata_CGREG_Or_yf[]="\r\n+CGREG: 1,5\r\n\r\nOK\r\n";//漫游

char Ats_CGDCONT[256]="AT+CGDCONT=1,\"ip\",\"cmnet\"\r";  //设置 APN无线接入点 //HGGDGS.HB

//char Ats_CGDCONT[256]="AT+CGDCONT=1,\"ip\",\"uninet\"\r";  

//Ata_OK

//char Ats_ETCPIP[]="AT%ETCPIP=\"csdytq\",\"csdytq\"\r";   //进行 ppp 拨号
char Ats_ETCPIP[]="AT%ETCPIP=\"\",\"\"\r";   //进行 ppp 拨号
//Ata_OK
//============建立连接
//char At_IPOPEN[]="AT%IPOPEN=\"UDP\",";//AT%IPOPEN="TCP","222.91.64.151",5100
char At_IPOPEN[]="AT%IPOPEN=\"TCP\",";//AT%IPOPEN="TCP","222.91.64.151",5100
char comma[]=",";
char LcIpPort[16];
char over123[]="\r";
char over124[]="\"\r";
char Ats_IPOPEN[128];
char Ata_IPOPEN[]="\r\nCONNECT\r\n";
//=============

char Ats_ATE0[]="ATE0\r";
//Ata_OK
char Ats_CSQ[]="AT+CSQ\r"; //检查当地的网络信号强度
//char Ata_CSQ[]="\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
char Ats_IPCLOSE[]="AT%IPCLOSE?\r";
char Ata_IPCLOSE[]="\r\n%IPCLOSE: 1,0,0\r\n\r\nOK\r\n";
//========================
char AtaHead_CSQ[]="\r\n+CSQ:";//"\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
char AtaHead_IPSEND[]="\r\n%IPSEND:";//"\r\n%IPSEND:15\r\n\r\nOK\r\n" //900c: \r\n%IPSEND:1,15 \r\nOK\r\n
char AtaHead_IPDATA[]="%IPDATA:";
//%IPDATA:8,"3132333435363636"
//%IPDATA:29,"3234323334323334323334333234323334333234323334323334323334"
//%IPDATA:144,"333234353334353334353334353334353335333435333435333435343335333435333435343335343335333435333435333435333435333435333435333435333435333435333435333234353334353334353334353334353335333435333435333435343335333435333435343335343335333435333435333435333435333435333435333435333435333435333435"
//%IPDATA:25,"68450045006841FFFFFFFFFF84F10000801E3117070006A416"\r\n
//========================
char Ats_IPSEND[]="AT%IPSEND=\"";
char over_isnd[]="\"\r";
///r/nERROR: 9 //当前没有 TCP或 UDP 连接存在(针对 IPSEND)
///r/nERROR: 20//TCP发送缓冲区满






GPRS_RUN_STA gGprsRunSta;
uint8 gGPRSManaBusy = 0;  //GPRS管理正忙中，0-不忙，1-忙。
UP_COMM_MSG gUpCommMsg;
static char gRecBuf_xmz[MAX_REC_MC52i_BYTENUM+100];//这里+100是冗余









void GPRS_POW_ON(void)
{
  	int stat;
  	stat = ioctl(g_uiIoControl,GPRS_SWITCH_C28,1);
   	if(stat){
    		printf("[%s][%s][%d] stat = %d \n",FILE_LINE,stat);  
   	}
}

void GPRS_POW_OFF(void)
{
  	int stat;
  	stat = ioctl(g_uiIoControl,GPRS_SWITCH_C28,0);
  	if(stat){
    		printf("[%s][%s][%d] stat = %d \n",FILE_LINE,stat);  
    	}
}

void GPRS_RESET_DISABLE(void)
{
  	int stat;
  	stat = ioctl(g_uiIoControl,GPRS_RESET_C29,0);
  	if(stat){
    		printf("[%s][%s][%d] stat = %d \n",FILE_LINE,stat);  
    	}
  
}

void GPRS_RESET_ENABLE(void)
{
  	int stat;
  	stat =  ioctl(g_uiIoControl,GPRS_RESET_C29,1);
  	if(stat){
    		printf("[%s][%s][%d] stat = %d \n",FILE_LINE,stat);  
    	}
	
}

/*
  ******************************************************************************
  * 函数名称：void GprsOff_xmz(void)
  * 说    明：关闭西门子GPRS模块。
  * 参    数：
  * 备    注: 
  ******************************************************************************
*/
void GprsOff_xmz(void)
{
	GPRS_POW_ON();
	OSTimeDly(OS_TICKS_PER_SEC/2);
	GPRS_POW_OFF();
}

/*
  ******************************************************************************
  * 函数名称：void GprsOn_xmz(void)
  * 说    明：开启西门子GPRS模块。
  * 参    数：
  * 备    注: 
  ******************************************************************************
*/
void GprsOn_xmz(void)
{
    	debug_debug(gDebugModule[GPRS_MODULE], "[%s][%s][%d] \n",FILE_LINE);
    
    	GPRS_POW_OFF();
	usleep(2000);
	GPRS_POW_ON();
    	usleep(2000);
    
	GPRS_RESET_DISABLE();
	OSTimeDly(OS_TICKS_PER_SEC);
    	GPRS_RESET_ENABLE();
    	OSTimeDly(OS_TICKS_PER_SEC);
    	GPRS_RESET_DISABLE();
    	OSTimeDly(OS_TICKS_PER_SEC/100);
    
}







/*
  ******************************************************************************
  * 函数名称：void UGprsWriteStr(char *Str)
  * 说    明： 
  * 参    数：
  * 备    注: 
  ******************************************************************************
*/

void UGprsWriteStr(char *Str)
{             		
	UpDevSend(UP_COMMU_DEV_GPRS,(uint8*)Str,strlen(Str));
	
}



/*
  ******************************************************************************
  * 函数名称：uint8 IfByte(uint8 data,char* HeadStr,char* OrHeadStr,uint8 p)
  * 说    明：比较字符,和HeadStr，或OrHeadStr 都比较。
  * 参    数：返回0，说明和HeadStr匹配成功，1说明和OrHeadStr匹配成功，2说明匹配失败。
  			
  * 备    注: 
  ******************************************************************************
*/
uint8 IfByte(uint8 data,char* HeadStr,char* OrHeadStr,uint8 p)
{
	if(data == (uint8)(HeadStr[p])){
		return 0;//匹配首选回应成功
	}
	else if(OrHeadStr != NULL){//匹配备选回应
		if(data == (uint8)(OrHeadStr[p])){
			return 1;//匹配备选回应成功
		}
		else{
			return 2;//匹配失败
		}
	}
	else{
		return 2;//匹配失败
	}
	
}


/*
  ******************************************************************************
  * 函数名称：uint8 GprsGetHead(uint8 device,char* HeadStr,char* OrHeadStr,uint16 OutTime,uint8 Find)
  * 说    明： 
  * 参    数：Find: 接收方式, TRUE: 一直查找正确的HeadStr,直到超时或接收字符数超过某个很大的值;
  					FALSE: 按OutTime 收到第一个字符后，连续接收HeadStr的长度后即返回成功或失败.
  		    
  			
  * 备    注: 此函数在HeadStr和OrHeadStr不等长时不对,还需修改
  ******************************************************************************
*/

//关于在收中间字节时的超时，注意需要考虑这样一种情况：收到的第一个字节不匹配时（C-D），（如在收IPCLOSE时，收到IPDATA），
//那么在后面的匹配过程中队IPCLOSE的第一个字符的匹配，就变成了对中间字符的匹配，而这与在收到真正的IPCLOSE 后再匹配
//中间字符（如匹配'P'）是有区别的。这个等待时间应该考虑GPRS模块在发完IPDATA再响应IPCLOSE时可能存在的区别于中间字符
//之间的时间间隔的时间间隔。因此此值要适当设置大一点
uint8 GprsGetHead(uint8 device,char* HeadStr,char* OrHeadStr,uint16 OutTime,uint8 Find)//可重入
{
	uint8 err,flag;
	uint8 data,headlen;
	uint16 p;
	uint32 n;
  		
	headlen = strlen(HeadStr);
	if(headlen==0 || headlen>HEAD_MAX_LEN){//headlen 必须大于0 ，即HeadStr必须非空;HeadStr最多允许HEAD_MAX_LEN个字符
		return 0xff;
	}

	err = UpGetch(device,&data,OutTime);
	if(err){	//超时
        debug_err(gDebugModule[MSIC_MODULE],"[%s][%s][%d] \n",FILE_LINE);
		return err;	
	}

	n = 5000;///查找情况下最多接收的字符数
	flag = 0;
	p = 0;
	while(n>1){
		n--;
		if(IfByte(data,HeadStr,OrHeadStr,p) >= 2){
	 		if(Find == TRUE){
	 			if(p > 0){//正在匹配HeadStr的第1个字节以后的字节（已匹配p个字节 ，匹配第p+1个字节出错）
	 				p = 0;     //
	 				continue;//重新开始匹配HeadStr的第1个字节
	 			}
	 		}
	 		else{
	 			flag = 0xfe;//表示失败，但还是要接收完headlen个字符，所以并不返回
	 			p++;
	 		}
	 	}
	 	else{
	 		p++;	
	 	}	
		
		if(p == headlen){//匹配完成
			break;
		}
		
		err=UpGetch(device,&data,OutTime);
		if(err){//
			return 0xfe;	//接收中间字符时超时  //ncq080819 这里也可能是正在匹配第1个字符！所以上面超时时间应怎么给还不好办，现在还不合理
		}
		
	}
	if(n == 1){//查找次数超限制，应返回失败
		flag=0xfd;
	}
	
	return flag;
}




/*
  ******************************************************************************
  * 函数名称：uint8 CMD_AT(char* AtComd,char* AtRightAnsw,char* OrAtRightAnsw,uint16 OutTime,uint8 ATEMode)
  * 说    明： 
  * 参    数：ATEMode:是否的带回显，TRUE：带回显； FLASE：不带回显
  * 备    注: 使用设备UP_COMMU_DEV_AT，故不可重入，其他使用设备UP_COMMU_DEV_AT的函数不能与其并发！
  ******************************************************************************
*/
uint8 CMD_AT(char* AtComd,char* AtRightAnsw,char* OrAtRightAnsw,uint16 OutTime,uint8 ATEMode)
{
	uint8 err;	
	
	UGprsWriteStr(AtComd);//注意函数返回后硬件发送并没有完成

	OSTimeDly(OS_TICKS_PER_SEC/8);
    	err=GprsGetHead(UP_COMMU_DEV_AT,AtRightAnsw,OrAtRightAnsw,OutTime,TRUE);/*yangfei modified */
    	if(err){
      	debug_err(gDebugModule[MSIC_MODULE], "[%s][%s][%d] AtRightAnsw err=%d\n",FILE_LINE,err);
     }
	
  	return err;
	
}


/*
  ******************************************************************************
  * 函数名称：uint8 CMD_AT_RP(char* AtComd,char* AtRightAnsw,char* OrAtRightAnsw,uint16 OutTime,uint32 rp,uint8 ATEMode)
  * 说    明： 
  * 参    数：ATEMode:是否的带回显，TRUE：带回显； FLASE：不带回显
  * 备    注: 使用设备UP_COMMU_DEV_AT，故不可重入，其他使用设备UP_COMMU_DEV_AT的函数不能与其并发！
  ******************************************************************************
*/
uint8 CMD_AT_RP(char* AtComd,char* AtRightAnsw,char* OrAtRightAnsw,uint16 OutTime,uint32 rp,uint8 ATEMode)
{
	uint32 i;
	uint8 err;
	
	for(i=0;i<1+rp;i++){//rp:重试次数，0 为不重试，只发一次
		err=CMD_AT(AtComd,AtRightAnsw,OrAtRightAnsw,OutTime,ATEMode);
		if(err==0){//成功
			break;
		}
		else{
			debug_err(gDebugModule[MSIC_MODULE], "[%s][%s][%d] CMD_AT err\n",FILE_LINE);
      		OSTimeDly(2*OS_TICKS_PER_SEC);//失败重试前加延时
    		}
	}
	
	return err;	
}



/*
  ******************************************************************************
  * 函数名称： void GprsRestart_xmz(void)
  * 说    明： GPRS模块重启，西门子模块。
  * 参    数： 无
  ******************************************************************************
*/
void GprsRestart_xmz(void)
{
	uint8 err;
	
	err=CMD_AT_RP(Ats_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,3,TRUE);
	printf("[%s][%s][%d] \n",FILE_LINE);
    	debug_debug(gDebugModule[GPRS_MODULE],"WARNING: <GprsRestart_xmz> GPRS Modul Shutdown!\n");
	OSTimeDly(OS_TICKS_PER_SEC);
	if(err == 0){//认为现在GPRS模块处于开机状态
		GprsOff_xmz();//紧急关机
		printf("WARNING: <GprsRestart_xmz> GPRS Modul Power Off!\n");
		OSTimeDly(15*OS_TICKS_PER_SEC);
	}
    	debug_debug(gDebugModule[GPRS_MODULE],"INFO: <GprsRestart_xmz> GPRS Modul Power On, And ReStart!\n");
   	GprsOn_xmz();
	OSTimeDly(2*OS_TICKS_PER_SEC);
	
}


/*
  ******************************************************************************
  * 函数名称： void InitGprsRunSta(void)
  * 说    明： 初始化GPRS运行状态。
  * 参    数： 无
  ******************************************************************************
*/
void InitGprsRunSta(void)
{	
	OS_ENTER_CRITICAL();
	//gGprsRunSta.ModuId = MODU_ID_UNKNOWN;
	gGprsRunSta.Ready = FALSE;
	gGprsRunSta.Connect = FALSE;
	gGprsRunSta.SocketType = 0;//0:TCP;1:UDP  //在Ready为TRUE后其含义才有效
	gGprsRunSta.IpCloseNum = 0;
	gGprsRunSta.IpSendFailNum = 0;
	gGprsRunSta.IpSendRemnBufNum = 0;
	gGprsRunSta.Csq = 0;
	//gGprsRunSta.FrmRecTimes=0;
	//gGprsRunSta.FrmSndTimes=0;
	gGprsRunSta.RecDog = 0;
	gGprsRunSta.SndDog = 0;
	OS_EXIT_CRITICAL();
}


//读取GPRS运行状态
void ReadGprsRunSta(GPRS_RUN_STA* Sta)
{	
	OS_ENTER_CRITICAL();
	memcpy((uint8*)Sta,(uint8*)(&gGprsRunSta),sizeof(GPRS_RUN_STA));
	OS_EXIT_CRITICAL();
}

//更新GPRS运行状态
void UpdGprsRunSta(GPRS_RUN_STA* Sta)
{	
	OS_ENTER_CRITICAL();
	memcpy((uint8*)(&gGprsRunSta),(uint8*)Sta,sizeof(GPRS_RUN_STA));
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_ModuId(uint8 val)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.ModuId = val;
	OS_EXIT_CRITICAL();
}


void UpdGprsRunSta_Stype(uint8 val)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.SocketType = val;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_Connect(uint8 val)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.Connect = val;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_Ready(uint8 val)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.Ready = val;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_IpCloseNum(uint8 val)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.IpCloseNum = val;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_IpSendRemnBufNum(uint8 val)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.IpSendRemnBufNum = val;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_Csq(uint8 val)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.Csq = val;
	OS_EXIT_CRITICAL();
}


void UpdGprsRunSta_AddIpCloseNum(void)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.IpCloseNum++;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_AddIpSendFailNum(void)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.IpSendFailNum++;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_FeedIpSendFailNum(void)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.IpSendFailNum = 0;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_FeedRecDog(void)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.RecDog = 0;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_AddRecDog(void)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.RecDog++;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_FeedSndDog(void)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.SndDog = 0;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_AddSndDog(void)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.SndDog++;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_AddFrmRecTimes(uint32 n)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.FrmRecTimes++;
	gGprsRunSta.FrmRecBs += n;
	OS_EXIT_CRITICAL();
}

void UpdGprsRunSta_AddFrmSndTimes(uint32 n)
{	
	OS_ENTER_CRITICAL();
	gGprsRunSta.FrmSndTimes++;
	gGprsRunSta.FrmSndBs += n;
	OS_EXIT_CRITICAL();
}

void GetSocketType(void)
{
	UpdGprsRunSta_Stype(SOCKET_TCP);  //当前固定用TCP。
}

uint8 GetGprsRunSta_ModuId(void)
{	
	uint8 val;
	
	OS_ENTER_CRITICAL();
	val=gGprsRunSta.ModuId;
	OS_EXIT_CRITICAL();
	
	return val;
}





/*
  ******************************************************************************
  * 函数名称： uint8 GprsTalk(uint32 rp)
  * 说    明： 
  * 参    数： 无
  ******************************************************************************
*/

uint8 GprsTalk(uint32 rp)
{
  	uint8 err;
  
  	debug_debug(gDebugModule[GPRS_MODULE], "[%s][%s][%d] \n",FILE_LINE);
  	err = CMD_AT_RP(Ats_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,rp,TRUE);
 	 if(err){
    		debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] Ats_AT CMD Err Module donot exist!\n",FILE_LINE);
    		OSTimeDly(OS_TICKS_PER_SEC/2);
    	}
	 
  	return err;
}

/*
  ******************************************************************************
  * 函数名称： uint8 ModelTalkIn(void)
  * 说    明： 和GPRS模块会话通讯
  * 参    数： 无
  ******************************************************************************
*/

uint8 ModelTalkIn(void)
{
	uint8 err;
	uint8 n;
	
	OSTimeDly(2*OS_TICKS_PER_SEC);
	
	err = GprsTalk(5);
	if(err == 0){
		return 0;
	}
	
	//会话失败,按西门子模块进行开机处理并重新会话
	n = 3;
	do{
		GprsOff_xmz();
		OSTimeDly(10*OS_TICKS_PER_SEC);
		
		GprsOn_xmz();
		OSTimeDly(5*OS_TICKS_PER_SEC);
		err = GprsTalk(3);
		if(err == 0){
			return 0;
		}
		if(n-- == 0){
			break;
		}
		
	}while(err);
	
	return 0xff;
}



/*
  ******************************************************************************
  * 函数名称： uint8 ModelIdentify(void)
  * 说    明： GPRS模块识别,目前只支持西门子MC52I模块。
  * 参    数： 无
  ******************************************************************************
*/

uint8 ModelIdentify(void)
{
	uint8 err;
	uint8 n;
	uint8 nn;
	
	printf("[%s][%s][%d] \n",FILE_LINE);
	n = 5;
gplp:
	nn=10;
	while(ModelTalkIn()){
		nn--;
 		if(nn == 0){
			return MODU_ID_UNKNOWN;
 		}
		OSTimeDly(25*OS_TICKS_PER_SEC);
		
	}
	
	OSTimeDly(OS_TICKS_PER_SEC/2);
    	printf("[%s][%s][%d] \n",FILE_LINE);
	
	//会话已经成功,读取模块ID====
	err = CMD_AT_RP(Ats_CGMM,Ata_CGMM_xmz,NULL,OS_TICKS_PER_SEC,2,TRUE);
	if(err == 0){
		OSTimeDly(OS_TICKS_PER_SEC);
		return MODU_ID_XMZ;
	}
    
    err = CMD_AT_RP(Ats_CGMM,Ata_CGMM_xmz_R3,NULL,OS_TICKS_PER_SEC,2,TRUE);
	if(err==0){
		OSTimeDly(OS_TICKS_PER_SEC);
		return MODU_ID_XMZ;
	}
	
	
	//====意外的失败
	OSTimeDly(5*OS_TICKS_PER_SEC);
	n--;
 	if(n == 0){
		return MODU_ID_UNKNOWN;
 	}
	
 	goto gplp;
	
}



/********************************************************************************************************
**  函 数  名 称: ConnectConfirm       									                               **
**	函 数  功 能: 登录确认，失败并重试						                                           **
**	输 入  参 数: none												                                   **
**	输 出  参 数: none											                                       **
**  返   回   值: none																			   	   **
**	备		  注:    					                                                               **
********************************************************************************************************/
uint8 ConnectConfirm(void)
{
	uint8 err = 0;
	int ret = 0;
   uint8 TryT = 0;
	uint8 lu8xmlIndex = 0;
	uint8 lu8Dev = UP_COMMU_DEV_AT;  //固定使用UP_COMMU_DEV_AT设备对应数据。
	pXml_info pXml_info = malloc(sizeof(xml_info_str));
	memset(pXml_info, 0x00, sizeof(xml_info_str));
    
	while(1){ 
		if(TryT++ >= 5){
			debug_err(gDebugModule[GPRS_MODULE], "ERROR: <ConnectConfirm> Retry five Times! GPRS UpLand Failure!\n");
			return FALSE; //5次没有收到登陆帧回应
		}

		OSTimeDly(OS_TICKS_PER_SEC);
		debug_err(gDebugModule[GPRS_MODULE],"GPRS Start Send Confirm frame !!!!\n");

		//组建登录帧。

		pXml_info->func_type = em_FUNC_ID;
		pXml_info->func_type = em_OPER_RD;
		err = setXmlInfo(lu8Dev,pXml_info);
		if(err != NO_ERR){
			printf("ConnectConfirm setXmlInfo ERR.\n");
			return FALSE;
		}
		
		do{
			lu8xmlIndex = Get_XMLBuf();  //获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
		}while(lu8xmlIndex == ERR_FF);

		err = xml_exec(lu8Dev,lu8xmlIndex);
		Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存。

		//等待登录应答信号量,等待时间10秒。
		ret = OSSem_timedwait(&Validate_sem,10*OS_TICKS_PER_SEC);
		if(ret != 0){
			printf("ConnectConfirm sem_timedwait timeover.\n");
			continue;
		}
		else{
			break;
		}
		
	}

	printf("ConnectConfirm success.\n");
	
	return TRUE;
}




/********************************************************************************************************
**  函 数  名 称: TaskGprsIpd       									                               **
**	函 数  功 能: GPRS延时检测							                                           **
**	输 入  参 数: time_s 延时的秒数											                                   **
**	输 出  参 数: none											                                       **
**  返   回   值: none																			   	   **
**	备		  注: 						                                                               **
********************************************************************************************************/
void DlyGprsCheck(uint8 time_s)
{
	uint8 n,m;
	
	n = time_s /10;
	m = time_s % 10;
	
	debug_debug(gDebugModule[GPRS_MODULE], "INFO: <DlyGprsCheck> Wait for GPRS Check Time Delay!\n");
	while(n--){	
        OSTimeDly(10*OS_TICKS_PER_SEC);
	}
	if(m){
		OSTimeDly(m*OS_TICKS_PER_SEC);
	}
}




//============================================================================
//用于接收中间部分长度不固定的AT命令应答的不固定长度部分，匹配尾字符后退出，并根据最小的可能长度和最大的可能长度返回err
//由于总是用于接收中间部分，接收字符的超时时间固定为OS_TICKS_PER_SEC/5
uint16 GprsGetViaTail(uint8 device,char* data,char Tail,char MinLen,uint16 MaxLen,uint8* err)
{
	uint16 n;
	uint8 tmp;
	n=0;
	do{
		if( n>= MaxLen){
			*err = 0xff;
			return n;
		}
		*err = UpGetch(device,&tmp,4*OS_TICKS_PER_SEC);//接收字符的超时时间固定为
		if(*err != OS_ERR_NONE){//超时
			return n;
		}
		data[n++] = tmp;
		data[n] = '\0';//便于调试观察
		
	}while(tmp != Tail);
  
	if(n < MinLen){
		*err=0xfe;
	}
	
	return n;
	
}




//char AtaHead_CSQ[]="\r\n+CSQ: ";
//char Ata_CSQ[]="\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
//使用设备UP_COMMU_DEV_AT，故不可重入，其他使用设备UP_COMMU_DEV_ATIPD的函数不能与其并发！
//关闭回显方式下使用
uint8 GprsGetCSQ(uint16 OutTime,char* pCsq)
{
	uint8 err;
	char  CsqStr[12];
	uint16 len;
	err = GprsGetHead(UP_COMMU_DEV_AT,AtaHead_CSQ,NULL,OS_TICKS_PER_SEC,TRUE);
	if(err == 0){
		len = GprsGetViaTail(UP_COMMU_DEV_AT,CsqStr,'\n',7,9,&err);
		if(err){
            printf("[%s][%s][%d] err=%d \n",FILE_LINE,err);
			return err;
		}
		
		//err=GprsGetHead(UP_COMMU_DEV_AT,Ata_OK,NULL,OS_TICKS_PER_SEC/5,FALSE);/*yf modified*/
       	err = GprsGetHead(UP_COMMU_DEV_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,FALSE);
		if(err){
            printf("[%s][%s][%d] err = %d \n",FILE_LINE,err);
			return err;
		}
		if(CsqStr[len-2]!='\r'){
			return 0xff;
		}
		memcpy(pCsq,&CsqStr[1],7);
		//'\r'后面的空格就不判断了
		//CsqStr[len-3]='\0';
		//strcpy(pCsq,&CsqStr[1]);
	}
	else{
		return err;
	}
	return 0;
}



//将最多4个Ascii表示的10进制数转换成 uint16型HEX数，如"256"转换为0x100
//输入参数为字符串
uint8 AsciiDec(char* str,uint16* result)
{
	uint8 i,len;
	uint16 val,pow;
	len=strlen(str);
	
   if(len>4){
		return 0xff;//长度超限
	}
	val=0;
	pow=1;
	for(i=0;i<len;i++){
		if(str[len-1-i]<0x30||str[len-1-i]>0x39){
				printf("[%s][%s][%d] str[len-1-i]=%d\n",FILE_LINE,str[len-1-i]);
		    return 0xfe;//字符非法
		}
		val=val+(str[len-1-i]-0x30)*pow;
		pow*=10;
	}
	*result=val;
	return 0;	
}



//使用设备UP_COMMU_DEV_AT，故不可重入，其他使用设备UP_COMMU_DEV_ATIPD的函数不能与其并发！
//关闭回显方式下使用
uint8 CMD_Csq(uint8* csq)//西门子/华为模块共用,里面Gprs_Sem对华为模块来说是不需要的,但是西门子需要
{
	uint8 err;
	uint16 val;
	char CsqStr[12];
	
	sem_wait(&Gprs_Sem);
    //printf("[%s][%s][%d] sem_wait(&Gprs_Sem) ok\n",FILE_LINE);
	//OSTimeDly(OS_TICKS_PER_SEC/10);
	
	UGprsWriteStr(Ats_CSQ);
	err=GprsGetCSQ(OS_TICKS_PER_SEC,CsqStr);
	if(err==0){
		if(CsqStr[1]==','){
			CsqStr[1]='\0';
		}
		else if(CsqStr[2]==','){
			CsqStr[2]='\0';
		}
		else{
			sem_post(&Gprs_Sem);
			return 0xf0;
		}
		err = AsciiDec(CsqStr,&val);
		*csq=(uint8)val;
	}
	sem_post(&Gprs_Sem);
	return err;
	
}





void ReadCsq(void)
{
	uint8 csq;
	if(CMD_Csq(&csq)==0){
        UpdGprsRunSta_Csq(csq);
	   printf("Gprs csq = %d .\n",csq);
    }
}


/*
  ******************************************************************************
  * 函数名称： uint8 GprsInit_xmz(void)
  * 说    明： 初始化西门子GPRS模块,建立网络连接。
  * 参    数： 无
  * 备    注:  使用设备UP_COMMU_DEV_AT，故不可重入，其他使用设备UP_COMMU_DEV_AT的函数不能与其并发！
			直接向UART1 发送，此函数成功前其他函数不可向UART1 发送，这通过全局结构体gGprsRunSta实现
  ******************************************************************************
*/
uint8 GprsInit_xmz(void)
{	
	uint8 err;
	uint32 i, n=0;
    	printf("[%s][%s][%d] \n",FILE_LINE);
    pSys_config pSyscon = malloc(sizeof(sys_config_str));
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	while(1){
		n++;
		if(n > 10){
			for(i=0;i<6;i++){
				
				OSTimeDly(10*OS_TICKS_PER_SEC); 
				ReadCsq();
			}
		}
		
		if(gGprsFirst == FALSE){
			GprsRestart_xmz();
		}
		//对下面的说明，集中器第一次测西门子GPRS模块时，发现很多时候下面发AT会失败1次，仔细测试发现，
		//模块开机后（发出^SYSSTART后5秒左右）如果发AT，会立即响应，但再发AT，会等1秒多才响应。
		//因此，这里第二次接收AT的响应成功实际上是上一次的滞后回应，而第二次的AT的响应则留到队列里，
		//不过没有关系，会被下面的IPR命令吃掉，所以这里就没有做针对的处理
		
		gGprsFirst = FALSE;
		
		UpQueueFlush(UP_COMMU_DEV_AT); //设备UP_COMMU_DEV_AT的队列，清空
		
		err = CMD_AT_RP(Ats_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,3,TRUE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){		
            	debug_err(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_AT CMD Err!\n");
			continue;
		}
		
        	#ifndef GPRS_ECHO
       	 /*关闭回显*/
        	err = CMD_AT_RP(Ats_ATE0,Ata_OK,NULL,OS_TICKS_PER_SEC,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){		
            	debug_err(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_AT CMD Err!\n");
			continue;
		}
        	#endif
        
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_AT CMD OK!\n");
     
        	err = CMD_AT_RP(Ats_IPR_0,Ata_OK,NULL,OS_TICKS_PER_SEC,2,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC/2);
		if(err){
            	debug_err(gDebugModule[GPRS_MODULE],"WARNING: <GprsInit_xmz> Ats_IPR_0 CMD Err!\n");
			OSTimeDly(OS_TICKS_PER_SEC/2);
		}
		
		err = CMD_AT_RP(Ats_SSYNC_c,Ata_SSYNC_c,NULL,OS_TICKS_PER_SEC,2,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC/2);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_SSYNC_c CMD Err!\n");
			err=CMD_AT_RP(Ats_SSYNC,Ata_OK,NULL,OS_TICKS_PER_SEC*2,2,FALSE);
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Try CMD Ats_SSYNC CMD!\n");
			OSTimeDly(OS_TICKS_PER_SEC/2);
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_SSYNC_c CMD OK!\n"); 
		
		
		err = CMD_AT_RP(Ats_CPIN,Ata_CPIN,NULL,OS_TICKS_PER_SEC*4,20,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_CPIN CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_CPIN CMD OK!\n");
		
		ReadCsq();
		OSTimeDly(2*OS_TICKS_PER_SEC);
		ReadCsq();
		OSTimeDly(2*OS_TICKS_PER_SEC);
		
		err = CMD_AT_RP(Ats_CREG,Ata_CREG,Ata_CREG_Or,OS_TICKS_PER_SEC*2,30,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_CREG CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_CREG CMD OK!\n");
		
		ReadCsq();
		OSTimeDly(OS_TICKS_PER_SEC);

		err = CMD_AT_RP(Ats_CGDCONT,Ata_OK,NULL,2*OS_TICKS_PER_SEC,15,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_CGDCONT CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_CGDCONT CMD OK!\n"); 
		
		err = CMD_AT_RP(Ats_CGATT,Ata_OK,NULL,10*OS_TICKS_PER_SEC,3,FALSE);
        	if(err){
			debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] \n",FILE_LINE);
		}
        	debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_CGATT CMD OK!\n"); 
        
		err = CMD_AT_RP(Ats_CGREG,Ata_CGREG,Ata_CGREG_Or,OS_TICKS_PER_SEC*2,5,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_CGREG CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_CGREG CMD OK!\n"); 
        	/*begin:yangfei added 2013-02-26 for*/
        	err = CMD_AT_RP(Ats_CGREG_1,Ata_OK,NULL,OS_TICKS_PER_SEC*2,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
         	/*end:yangfei added 2013-02-26*/
		ReadCsq();
		OSTimeDly(OS_TICKS_PER_SEC);
		
		GetSocketType();
		//ProSocketType();
		err = CMD_AT_RP(Ats_SICS_conType,Ata_OK123,NULL,OS_TICKS_PER_SEC,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_SICS_conType CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_SICS_conType CMD OK!\n");
		
		err = CMD_AT_RP(Ats_SICS_apn,Ata_OK123,NULL,OS_TICKS_PER_SEC,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_SICS_apn CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_SICS_apn CMD OK!\n");
		
		err = CMD_AT_RP(Ats_SISS_srvType,Ata_OK123,NULL,OS_TICKS_PER_SEC,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_SISS_srvType CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_SISS_srvType CMD OK!\n");
		
		err = CMD_AT_RP(Ats_SISS_conId,Ata_OK123,NULL,OS_TICKS_PER_SEC,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
			debug_info(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_SISS_conId CMD Err!\n");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_SISS_conId CMD OK!\n");

		get_sys_config(CONFIG_PRIMARY_SERVER, pSyscon);
	 	strcpy(Ats_SISS_address,Ats_SISS_address_h);
		printf("PRIMARY_SERVER: %s\n", pSyscon->f_config_value);
     	strcat(Ats_SISS_address,pSyscon->f_config_value);
     	strcat(Ats_SISS_address,colon);
		get_sys_config(CONFIG_PRIMARY_PORT, pSyscon);
		printf("PRIMARY_PORT: %s\n", pSyscon->f_config_value);
     	strcat(Ats_SISS_address,pSyscon->f_config_value);
     	strcat(Ats_SISS_address,over124);
		printf("Ats_SISS_address: %s\n", Ats_SISS_address);
     	err = CMD_AT_RP(Ats_SISS_address,Ata_OK123,NULL,OS_TICKS_PER_SEC*3,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
      		debug_err(gDebugModule[GPRS_MODULE],"<GprsInit_xmz> Ats_SISS_address CMD Err!");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_SISS_address CMD OK!\n");
        
		UpQueueFlush(UP_COMMU_DEV_ATIPD);
		//打开连接
		err = CMD_AT_RP(Ats_SISO,Ata_OK,NULL,OS_TICKS_PER_SEC*20,0,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
      		debug_err(gDebugModule[GPRS_MODULE],"<GprsInit_xmz> Ats_SISO CMD Err=%d!\n",err);
			continue; /*yangfei:needed use */
		}
    		debug_info(gDebugModule[GPRS_MODULE],"INFO: <GprsInit_xmz> Ats_SISO CMD OK!\n");
		OSTimeDly(OS_TICKS_PER_SEC);
		//err=CMD_AT_RP(Ats_SISI,Ata_SISI,NULL,OS_TICKS_PER_SEC*5,4,TRUE);
		////OSTimeDly(OS_TICKS_PER_SEC);

		OSTimeDly(OS_TICKS_PER_SEC);
		break;
	}	

	printf("GPRS Module Init Success!\n");
	
	return 0;
}





/*
  ******************************************************************************
  * 函数名称： void  GPRS_Mana_Proc(void *pdata)
  * 说    明： GPRS管理相关，包括网络连接，定时心跳，模块状态检测，模块重启机制。
  * 参    数： 无
  * 备    注:  在pthread_GPRS_Mana线程中，设备一律按照UP_COMMU_DEV_AT。
  ******************************************************************************
*/
uint8 gGprsFirst;  //标记GPRS模块是不是第一次初始化。
void  GPRS_Mana_Proc(void *pdata)
{
	uint8 err = 0;
	uint8 lu8xmlIndex = 0;
	uint8 lu8Dev = UP_COMMU_DEV_AT;  //本任务中设备都使用UP_COMMU_DEV_AT。
	uint8 tmpmid = 0;
	int ReStartCounter = 0;
	GPRS_RUN_STA GprsRunSta;
	uint32 HeartFrmSndCycles = 0;
	pXml_info pXml_info = malloc(sizeof(xml_info_str));
	memset(pXml_info, 0x00, sizeof(xml_info_str));



	pdata = pdata;

	do{
		GprsRestart_xmz();
		gGprsFirst = TRUE;
		InitGprsRunSta();
		UpdGprsRunSta_ModuId(MODU_ID_UNKNOWN);
		tmpmid = ModelIdentify();
		UpdGprsRunSta_ModuId(tmpmid);

		if(tmpmid == MODU_ID_XMZ)
			printf("GPRS Module MC52I .\n");
		else
			printf("GPRS Module Unkown.\n");
		
	}while(tmpmid == MODU_ID_UNKNOWN);  //检测到已知模块才会退出。

	for(;;){ 
		ReStartCounter++;
		if(ReStartCounter > 1){
			debug_info(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] GPRS Modul Restart, Retry Times is %2d\n", ReStartCounter);
		}
		else{
			debug_info(gDebugModule[GPRS_MODULE],"INFO: [TaskGprsMana] GPRS Modul Start!\n");
		}

		//sem_wait(&GprsIPDTask_sem);  //GPRS模块初始化建立网络前，先关闭GprsIPD线程。GprsIPD可以一直开着。
	   
		InitGprsRunSta();//GPRS没有就绪时 		
		tmpmid = GetGprsRunSta_ModuId();
		
		if(tmpmid == MODU_ID_XMZ){
    			GprsInit_xmz(); //使用设备 UP_COMMU_DEV_AT初始化西门子模块。
    			debug_info(gDebugModule[GPRS_MODULE],"GPRS  XMZ  init OK!! \n");
    		}  
        	else{
               debug_err(gDebugModule[GPRS_MODULE],"GPRS MODU_ID_UNKNOWN!! \n");
          }
		
	  	UpdGprsRunSta_Ready(TRUE);

	  	if(ConnectConfirm() == FALSE){
	  		continue;
	  	}

	  	UpdGprsRunSta_Connect(TRUE);
		
	  	while(1){
			DlyGprsCheck(GPRS_CHECK_CYCLE);

			HeartFrmSndCycles = (HeartFrmSndCycles + 1) % (GPRS_HEART_FRM_TIME / GPRS_CHECK_CYCLE);
			if(HeartFrmSndCycles  == 0){
				//发送心跳帧，并AddSndDog。
				pXml_info->func_type = em_FUNC_HEATBT;
				pXml_info->func_type = em_OPER_RD;
				
				err = setXmlInfo(lu8Dev,pXml_info);
				if(err != NO_ERR){
					printf("HEART_BEAT setXmlInfo ERR.\n");
					break;
				}
				do{
					lu8xmlIndex = Get_XMLBuf();  //获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
				}while(lu8xmlIndex == ERR_FF);
				
				err = xml_exec(lu8Dev, lu8xmlIndex);
				Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存。
		
				
				UpdGprsRunSta_AddSndDog();

			}

			ReadGprsRunSta(&GprsRunSta);
			if(GprsRunSta.SndDog > GPRS_HEART_FRM_REC_OUTTIME){
				UpdGprsRunSta_Connect(FALSE);    //GPRS 断线
				break;
			}

         
	  	} 
		
	}

}



uint8 gpHexToAscii(uint8 x)
{
	if(x<0x0a){
		return(x+'0');
	}
	else{
		return(x-0x0a+'A');
	}
}


void gpHex16ToStr(uint16 x,char* str)
{
	uint8 i,n;
	uint8 buf[10];
	n=0;
	while(x){
		buf[n++]=gpHexToAscii(x%10);
		x=x/10;
	}
	
	//反序
	for(i=0;i<n;i++){
		str[i]=buf[n-1-i];
	}
	str[i]='\0';
}



//extern uint32 gprs_sfai1,gprs_sfai2;
uint8 GprsIPSEND_xmz(uint8* ipdata,uint16 len)
{
	char buf[8];
	uint8 err;
 

	if(len > MAX_IPSEND_MC52i_BYTENUM){
		return 0xdd;
	}

    	if(len == 0){
      	debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] GprsIPSEND_xmz len=0\n",FILE_LINE);
      	return 0xd1;
     }

	OSTimeDly(OS_TICKS_PER_SEC/10);
	strcpy(Ats_SISW,Ats_SISW_h);
    
	gpHex16ToStr(len,buf);
	strcat(Ats_SISW,buf);
	strcat(Ats_SISW,over123);
    	
    	UGprsWriteStr(Ats_SISW);
	
	#ifdef  GPRS_ECHO
	err = GprsGetHead(UP_COMMU_DEV_AT,Ats_SISW,NULL,2*OS_TICKS_PER_SEC,TRUE);
	if(err){
        	debug_err(gDebugModule[GPRS_MODULE],"[%s][%s][%d]Ats_SISW get head Failure, The err is %d!\n", FILE_LINE,err);
		return err;
	}
	#endif
	err = GprsGetHead(UP_COMMU_DEV_AT,Ata_SISW_m,NULL,2*OS_TICKS_PER_SEC,FALSE);
	if(err){
        	debug_err(gDebugModule[GPRS_MODULE],"[%s][%s][%d]Ats_SISW_m get head Failure, The err is %d!\n", FILE_LINE,err);
		return err;
	}

	UpDevSend(UP_COMMU_DEV_GPRS,ipdata,len);

	err = GprsGetHead(UP_COMMU_DEV_AT,Ata_OK,NULL,2*OS_TICKS_PER_SEC,TRUE);
	if(err){
		debug_err(gDebugModule[GPRS_MODULE],"Ata_OK get head Failure, The err is %d!", err);
		return err;
	}

	printf("GPRS IPSend Success.\n");
	
	return NO_ERR;
}



uint8 GprsIPSEND(uint8* ipdata,uint16 len)
{
	if(GetGprsRunSta_ModuId() == MODU_ID_XMZ){
		return GprsIPSEND_xmz(ipdata,len);
	}
	
	return 1;
}




/*
******************************************************************************
* 函数名称：void GPRS_FileSend(uint8 Dev, FILE *fp)
* 说	明： 通过GPRS端口，发送一个文件内容。
* 参	数： 
******************************************************************************
*/
void GPRS_FileSend(uint8 Dev, FILE *fp)
{
	int	i = 0;
	uint8 err = 0;
	int32 l32FileSize = 0;
	uint32 lu32SendNum = 0;

	sem_wait(&Gprs_Sem);
	
	//计算文件大小。
	fseek(fp,0,2);			//位置指针到文件末尾
	l32FileSize = ftell(fp);	//计算文件大小
	fseek(fp,0,0);			//位置指针到文件开始
	lu32SendNum = SEND_TEMPBUF_SIZE;

	for(i=0;i<(l32FileSize/SEND_TEMPBUF_SIZE + 1);i++){
		if(i == l32FileSize/SEND_TEMPBUF_SIZE){ //最后一包
			lu32SendNum = l32FileSize % SEND_TEMPBUF_SIZE;
		}
		
		memset(pSendTempBuf[Dev],0,sizeof(pSendTempBuf));
		fread(pSendTempBuf[Dev],1,lu32SendNum,fp);
                 //debug_info(gDebugModule[GPRS_MODULE], "send xml:  %s\n",gGPRSSendBuf);
		       // debug_info(gDebugModule[GPRS_MODULE], "INFO: [TaskUpSend] SendNum=%d\n",SendNum);
  		err = GprsIPSEND(pSendTempBuf[Dev],lu32SendNum);
		if(err != NO_ERR){

		}

	}

	sem_post(&Gprs_Sem);


}



/*
  ******************************************************************************
  * 函数名称： int pthread_GPRS_Mana(void)
  * 说    明： GPRS管理线程。
  * 参    数： 无
  * 备    注:在pthread_GPRS_Mana线程中，设备一律按照UP_COMMU_DEV_AT。
  ******************************************************************************
*/

int pthread_GPRS_Mana(void)
{

	GPRS_Mana_Proc(NULL);

	return(CONTROL_RET_SUC);

}




//char AtaHead_IPDATA[]="%IPDATA:";
//%IPDATA:25,"68450045006841FFFFFFFFFF84F10000801E3117070006A416"\r\n
//独占设备UP_COMMU_DEV_ATIPD
uint8 GprsGetIPDATA_xmz(char* ipdata,uint16 OutTime,uint16* StrLen)
{
	uint8 err;

	ipdata=ipdata;
	StrLen=StrLen;
    
   	err=GprsGetHead(UP_COMMU_DEV_ATIPD,Ata_URC_SISR,NULL,OutTime,TRUE);
 
	return err;	

}



/*
get data from USART3RecQueue_At UP_COMMU_DEV_AT

jh:交互
*/
uint8 GprsGetIPDATA_jh(char* ipdata,uint16 OutTime,uint16* StrLen)
{
	uint8 err;
	char  lenstr[8];
	uint16 i,Len;
	OutTime = OutTime;
	sem_wait(&Gprs_Sem);

  	UpQueueFlush(UP_COMMU_DEV_AT);
	UGprsWriteStr(Ats_SISR);
	
	#ifdef  GPRS_ECHO
	err=GprsGetHead(UP_COMMU_DEV_AT,Ats_SISR,NULL,2*OS_TICKS_PER_SEC,TRUE);
	if(err){
		sem_post(&Gprs_Sem);
        	debug_err(gDebugModule[GPRS_MODULE], "[%s][%s][%d] Ats_SISR err=%d\n",FILE_LINE,err);
		return err;
	}
	#endif
  
	err=GprsGetHead(UP_COMMU_DEV_AT,Ata_SISR_m,NULL,2*OS_TICKS_PER_SEC,FALSE);
	if(err){
		sem_post(&Gprs_Sem);
        	debug_err(gDebugModule[GPRS_MODULE],"[%s][%d]Ata_SISR_m err=%d\n",__FUNCTION__,__LINE__,err);
		return err;
	}
	
	Len = GprsGetViaTail(UP_COMMU_DEV_AT,lenstr,'\n',3,8,&err);
	if(err){
		sem_post(&Gprs_Sem);
    		debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d\n",__FUNCTION__,__LINE__,err);
		return err;
	}
	if(lenstr[Len-2]!='\r'){
		sem_post(&Gprs_Sem);
    		debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d\n",__FUNCTION__,__LINE__,err);
		return 0xff;
	}
	lenstr[Len-2]='\0';
	

	err = AsciiDec(lenstr,&Len);
	if(err){
		sem_post(&Gprs_Sem);
    		debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d\n",__FUNCTION__,__LINE__,err);
		*StrLen = 0;
		return 0xfe;
	}
	
	if(Len > MAX_REC_MC52i_BYTENUM){
		return 0xdd;
	}
	
	for(i=0;i<Len;i++){
		err=UpGetch(UP_COMMU_DEV_AT,(uint8*)(&ipdata[i]),3*OS_TICKS_PER_SEC);
		if(err){
			sem_post(&Gprs_Sem);
      		debug_err(gDebugModule[GPRS_MODULE],"[%s][%d] len=%d err=%d\n",__FUNCTION__,__LINE__,Len,err);
			return 0xf0;
		}
	}

	*StrLen = Len;
	sem_post(&Gprs_Sem);
	return 0;
}



/********************************************************************************************************
**  函 数  名 称: Fun_GprsIpd_xmz       									                           **
**  函 数  功 能: 西门子模块接收网络传输数据处理函数。							                                           **			
**  输 入  参 数: pdata												                                   **
**  输 出  参 数: none											                                       **
**  返   回   值: none																			   	   **
**  备	    注: 						                                                               **
********************************************************************************************************/
static uint16 gUpdateBegin = 0;

void Fun_GprsIpd_xmz(void)
{
	//本任务没有任务狗,原因之一是它可能在一段时间处在挂起状态
	uint8 err;
	uint16 len;
	uint8 Count = 0;
	
	while(1){
		
		if(gUpdateBegin == 1){
			err = 0;
			UpQueueFlush(UP_COMMU_DEV_ATIPD);
		}
      	else{
     		err = GprsGetIPDATA_xmz(gRecBuf_xmz,0,&len);
      	}
		
		if(err==0){
			debug_debug(gDebugModule[GPRS_MODULE],"enter GPRS jh\n");
			do{
				err = GprsGetIPDATA_jh(gRecBuf_xmz,0,&len);
	  			if(err){
					//重试一次 
					//可能对调试过程中断点(或本地口来的数据)导致错误而彻底中断西门子模块发的^SISR: 1, 1(数据还没有完全读空时)
					debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d\n",__FUNCTION__,__LINE__,err);	
					OSTimeDly(OS_TICKS_PER_SEC/8);
	  				err = GprsGetIPDATA_jh(gRecBuf_xmz,0,&len);
					if(err){
						debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d\n",__FUNCTION__,__LINE__,err);
					}
	  			}
	  		
				debug_debug(gDebugModule[GPRS_MODULE],"[%s][%s][%d] GprsIPREC len=%d ok \n",FILE_LINE,len);	
				if(len > 400){gUpdateBegin = 1;}/*进入主动读取数据模式*/
				if(len == 0)
					{Count++;}
				else
					{Count = 0;}
				if(Count > 10){gUpdateBegin = 0;}/*连续10次无数据进入查询接受数据模式*/
			
            		OSTimeDly(OS_TICKS_PER_SEC/8);
	  			UpdGprsRunSta_FeedRecDog();	
	  			UpdGprsRunSta_AddFrmRecTimes(len);	
	  			UpRecQueueWrite(UP_COMMU_DEV_GPRS,(uint8*)gRecBuf_xmz,len);
        			if(len){
        				debug_debug(gDebugModule[GPRS_MODULE],"[%s][%s][%d] %s\n",FILE_LINE,gRecBuf_xmz);
          			memset(gRecBuf_xmz,0,sizeof(gRecBuf_xmz));
        			}
	  		}while(err==0 && len>0);	
			debug_debug(gDebugModule[GPRS_MODULE],"leave GPRS jh \n");
		}
		else{
			debug_err(gDebugModule[GPRS_MODULE],"%s %d err=%d\n",__FUNCTION__,__LINE__,err);
		}
	} 
	
}




/*
  ******************************************************************************
  * 函数名称： int pthread_GPRS_IPD(void)
  * 说    明： 获取GPRS网络传输数据的线程。
  * 参    数： 无
  ******************************************************************************
*/

int pthread_GPRS_IPD(void)
{
	uint8 lu8GprsType = 0;

	while(1){

		lu8GprsType = GetGprsRunSta_ModuId();
		if(lu8GprsType == MODU_ID_XMZ){  //目前只有西门子模块支持。
			//printf("Fun_GprsIpd_xmz start.\n");  //测试用
			Fun_GprsIpd_xmz();

		}
		else{
			OSTimeDly(1000);//等待1秒钟之后继续循环。
		}



	}

	return(CONTROL_RET_SUC);

}





/*
******************************************************************************
* 函数名称： void pthread_GprsDataDeal(void)
* 说	明： 通过GPRS传输到集中器的数据处理线程。
* 参	数： 
******************************************************************************
*/
void pthread_GprsDataDeal(void)
{
	uint8 err = 0;
	//uint8 lu8data = 0;
	uint16 lu16outtime = 1000;  //等待 毫秒数。
	uint8 lu8xmlIndex = 0;
	//FILE *fp;
	printf("pthread_GprsDataDeal start.\n");

	while(1){	
		do{
			lu8xmlIndex = Get_XMLBuf();  //获取一个xml暂存空间,最后一定要释放该空间，获取-使用-释放。
		}while(lu8xmlIndex == ERR_FF);

		//printf("Get_XMLBuf %d.\n",lu8xmlIndex);
		
		err = UpGetXMLStart(lu8xmlIndex,UP_COMMU_DEV_GPRS,lu16outtime);
		if(err == NO_ERR){
			printf("pthread_GprsDataDeal UpGetXMLStart OK.\n");
			err = UpGetXMLEnd(lu8xmlIndex,UP_COMMU_DEV_GPRS,lu16outtime);
			if(err == NO_ERR){//说明接收到一帧完整的xml数据。
				//printf("pthread_GprsDataDeal UpGetXMLEnd OK.lu8xmlIndex = %d.\n",lu8xmlIndex);
				//err = XmlInfo_Analyze(UP_COMMU_DEV_GPRS, lu8xmlIndex);
				err = parse_xml(UP_COMMU_DEV_GPRS, lu8xmlIndex);
				
				Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存。

			}
			else{
				Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存。
			}

		}
		else{
			Put_XMLBuf(lu8xmlIndex);  //释放被占用的xml暂存。
		}
	}
	
}






