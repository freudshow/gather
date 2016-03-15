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


//¶¨ÒåËµÃ÷£ºAts_: AT·¢ËÍÃüÁî,Ata_: ATÓ¦´ðÃüÁî
char Ats_CGMM[]="AT+CGMM\r";
/*begin:yangfei modified 2013-01-15*/
char Ata_CGMM_xmz_R3[]="\r\nMC52iR3\r\n\r\nOK\r\n";
char Ata_CGMM_xmz[]="\r\nMC52i\r\n\r\nOK\r\n";
/*end:yangfei modified 2013-01-15*/
char Ats_SMSO[]="AT^SMSO\r";
char Ata_SMSO[]="\r\n^SMSO: MS OFF\r\n";

//´´½¨connecting profile:
//at^sics=0,conType,GPRS0
//at^sics=0,apn,cmnet
char Ats_SICS_conType[]="at^sics=0,conType,GPRS0\r";
char Ats_SICS_apn[256]="at^sics=0,apn,cmnet\r";
 // char Ats_SICS_apn[256]="at^sics=0,apn,uninet\r";

///´´½¨tcp service profile
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

//´ò¿ªTCPÁ¬½Ó
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

char Ats_CPIN[]="AT+CPIN\?\r";////¼ì²é SIM ¹¤×÷ÊÇ·ñÕý³£
char Ata_CPIN[]="\r\n+CPIN: READY\r\n\r\nOK\r\n";

char Ats_CREG[]="AT+CREG\?\r";////¼ì²é GSM ÍøÂç×¢²áÇé¿ö
char Ata_CREG[]="\r\n+CREG: 0,1\r\n\r\nOK\r\n"; //±¾µØ
char Ata_CREG_Or[]="\r\n+CREG: 0,5\r\n\r\nOK\r\n";//ÂþÓÎ

char Ats_CGREG_1[]="AT+CGREG=1\r"; //


char Ats_CGATT_1[]="AT+CGATT\?\r";
char Ata_CGATT_1[]="+CGATT: 1\r\n\r\nOK\r\n";
char Ats_CGATT[]="AT+CGATT=1\r";
//char Ats_CGAUTO[]="AT+CGAUTO=1\r";

char Ats_CGREG[]="AT+CGREG\?\r"; //¼ì²é GPRS ÍøÂç×¢²áÇé¿ö
char Ata_CGREG[]="\r\n+CGREG: 0,1\r\n\r\nOK\r\n";//±¾µØ
char Ata_CGREG_Or[]="\r\n+CGREG: 0,5\r\n\r\nOK\r\n";//ÂþÓÎ

char Ata_CGREG_yf[]="\r\n+CGREG: 1,1\r\n\r\nOK\r\n";//±¾µØ
char Ata_CGREG_Or_yf[]="\r\n+CGREG: 1,5\r\n\r\nOK\r\n";//ÂþÓÎ

char Ats_CGDCONT[256]="AT+CGDCONT=1,\"ip\",\"cmnet\"\r";  //ÉèÖÃ APNÎÞÏß½ÓÈëµã //HGGDGS.HB

//char Ats_CGDCONT[256]="AT+CGDCONT=1,\"ip\",\"uninet\"\r";  

//Ata_OK

//char Ats_ETCPIP[]="AT%ETCPIP=\"csdytq\",\"csdytq\"\r";   //½øÐÐ ppp ²¦ºÅ
char Ats_ETCPIP[]="AT%ETCPIP=\"\",\"\"\r";   //½øÐÐ ppp ²¦ºÅ
//Ata_OK
//============½¨Á¢Á¬½Ó
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
char Ats_CSQ[]="AT+CSQ\r"; //¼ì²éµ±µØµÄÍøÂçÐÅºÅÇ¿¶È
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
///r/nERROR: 9 //µ±Ç°Ã»ÓÐ TCP»ò UDP Á¬½Ó´æÔÚ(Õë¶Ô IPSEND)
///r/nERROR: 20//TCP·¢ËÍ»º³åÇøÂú






GPRS_RUN_STA gGprsRunSta;
uint8 gGPRSManaBusy = 0;  //GPRS¹ÜÀíÕýÃ¦ÖÐ£¬0-²»Ã¦£¬1-Ã¦¡£
UP_COMM_MSG gUpCommMsg;








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
  * º¯ÊýÃû³Æ£ºvoid GprsOff_xmz(void)
  * Ëµ    Ã÷£º¹Ø±ÕÎ÷ÃÅ×ÓGPRSÄ£¿é¡£
  * ²Î    Êý£º
  * ±¸    ×¢: 
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
  * º¯ÊýÃû³Æ£ºvoid GprsOn_xmz(void)
  * Ëµ    Ã÷£º¿ªÆôÎ÷ÃÅ×ÓGPRSÄ£¿é¡£
  * ²Î    Êý£º
  * ±¸    ×¢: 
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
  * º¯ÊýÃû³Æ£ºvoid UGprsWriteStr(char *Str)
  * Ëµ    Ã÷£º 
  * ²Î    Êý£º
  * ±¸    ×¢: 
  ******************************************************************************
*/

void UGprsWriteStr(char *Str)
{             		
	UpDevSend(UP_COMMU_DEV_GPRS,(uint8*)Str,strlen(Str));
	
}



/*
  ******************************************************************************
  * º¯ÊýÃû³Æ£ºuint8 IfByte(uint8 data,char* HeadStr,char* OrHeadStr,uint8 p)
  * Ëµ    Ã÷£º±È½Ï×Ö·û,ºÍHeadStr£¬»òOrHeadStr ¶¼±È½Ï¡£
  * ²Î    Êý£º·µ»Ø0£¬ËµÃ÷ºÍHeadStrÆ¥Åä³É¹¦£¬1ËµÃ÷ºÍOrHeadStrÆ¥Åä³É¹¦£¬2ËµÃ÷Æ¥ÅäÊ§°Ü¡£
  			
  * ±¸    ×¢: 
  ******************************************************************************
*/
uint8 IfByte(uint8 data,char* HeadStr,char* OrHeadStr,uint8 p)
{
	if(data == (uint8)(HeadStr[p])){
		return 0;//Æ¥ÅäÊ×Ñ¡»ØÓ¦³É¹¦
	}
	else if(OrHeadStr != NULL){//Æ¥Åä±¸Ñ¡»ØÓ¦
		if(data == (uint8)(OrHeadStr[p])){
			return 1;//Æ¥Åä±¸Ñ¡»ØÓ¦³É¹¦
		}
		else{
			return 2;//Æ¥ÅäÊ§°Ü
		}
	}
	else{
		return 2;//Æ¥ÅäÊ§°Ü
	}
	
}


/*
  ******************************************************************************
  * º¯ÊýÃû³Æ£ºuint8 GprsGetHead(uint8 device,char* HeadStr,char* OrHeadStr,uint16 OutTime,uint8 Find)
  * Ëµ    Ã÷£º 
  * ²Î    Êý£ºFind: ½ÓÊÕ·½Ê½, TRUE: Ò»Ö±²éÕÒÕýÈ·µÄHeadStr,Ö±µ½³¬Ê±»ò½ÓÊÕ×Ö·ûÊý³¬¹ýÄ³¸öºÜ´óµÄÖµ;
  					FALSE: °´OutTime ÊÕµ½µÚÒ»¸ö×Ö·ûºó£¬Á¬Ðø½ÓÊÕHeadStrµÄ³¤¶Èºó¼´·µ»Ø³É¹¦»òÊ§°Ü.
  		    
  			
  * ±¸    ×¢: ´Ëº¯ÊýÔÚHeadStrºÍOrHeadStr²»µÈ³¤Ê±²»¶Ô,»¹ÐèÐÞ¸Ä
  ******************************************************************************
*/

//¹ØÓÚÔÚÊÕÖÐ¼ä×Ö½ÚÊ±µÄ³¬Ê±£¬×¢ÒâÐèÒª¿¼ÂÇÕâÑùÒ»ÖÖÇé¿ö£ºÊÕµ½µÄµÚÒ»¸ö×Ö½Ú²»Æ¥ÅäÊ±£¨C-D£©£¬£¨ÈçÔÚÊÕIPCLOSEÊ±£¬ÊÕµ½IPDATA£©£¬
//ÄÇÃ´ÔÚºóÃæµÄÆ¥Åä¹ý³ÌÖÐ¶ÓIPCLOSEµÄµÚÒ»¸ö×Ö·ûµÄÆ¥Åä£¬¾Í±ä³ÉÁË¶ÔÖÐ¼ä×Ö·ûµÄÆ¥Åä£¬¶øÕâÓëÔÚÊÕµ½ÕæÕýµÄIPCLOSE ºóÔÙÆ¥Åä
//ÖÐ¼ä×Ö·û£¨ÈçÆ¥Åä'P'£©ÊÇÓÐÇø±ðµÄ¡£Õâ¸öµÈ´ýÊ±¼äÓ¦¸Ã¿¼ÂÇGPRSÄ£¿éÔÚ·¢ÍêIPDATAÔÙÏìÓ¦IPCLOSEÊ±¿ÉÄÜ´æÔÚµÄÇø±ðÓÚÖÐ¼ä×Ö·û
//Ö®¼äµÄÊ±¼ä¼ä¸ôµÄÊ±¼ä¼ä¸ô¡£Òò´Ë´ËÖµÒªÊÊµ±ÉèÖÃ´óÒ»µã
uint8 GprsGetHead(uint8 device,char* HeadStr,char* OrHeadStr,uint16 OutTime,uint8 Find)//¿ÉÖØÈë
{
	uint8 err,flag;
	uint8 data,headlen;
	uint16 p;
	uint32 n;
  		
	headlen = strlen(HeadStr);
	if(headlen==0 || headlen>HEAD_MAX_LEN){//headlen ±ØÐë´óÓÚ0 £¬¼´HeadStr±ØÐë·Ç¿Õ;HeadStr×î¶àÔÊÐíHEAD_MAX_LEN¸ö×Ö·û
		return 0xff;
	}

	err = UpGetch(device,&data,OutTime);
	if(err){	//³¬Ê±
        debug_err(gDebugModule[MSIC_MODULE],"[%s][%s][%d] \n",FILE_LINE);
		return err;	
	}

	n = 5000;///²éÕÒÇé¿öÏÂ×î¶à½ÓÊÕµÄ×Ö·ûÊý
	flag = 0;
	p = 0;
	while(n>1){
		n--;
		if(IfByte(data,HeadStr,OrHeadStr,p) >= 2){
	 		if(Find == TRUE){
	 			if(p > 0){//ÕýÔÚÆ¥ÅäHeadStrµÄµÚ1¸ö×Ö½ÚÒÔºóµÄ×Ö½Ú£¨ÒÑÆ¥Åäp¸ö×Ö½Ú £¬Æ¥ÅäµÚp+1¸ö×Ö½Ú³ö´í£©
	 				p = 0;     //
	 				continue;//ÖØÐÂ¿ªÊ¼Æ¥ÅäHeadStrµÄµÚ1¸ö×Ö½Ú
	 			}
	 		}
	 		else{
	 			flag = 0xfe;//±íÊ¾Ê§°Ü£¬µ«»¹ÊÇÒª½ÓÊÕÍêheadlen¸ö×Ö·û£¬ËùÒÔ²¢²»·µ»Ø
	 			p++;
	 		}
	 	}
	 	else{
	 		p++;	
	 	}	
		
		if(p == headlen){//Æ¥ÅäÍê³É
			break;
		}
		
		err=UpGetch(device,&data,OutTime);
		if(err){//
			return 0xfe;	//½ÓÊÕÖÐ¼ä×Ö·ûÊ±³¬Ê±  //ncq080819 ÕâÀïÒ²¿ÉÄÜÊÇÕýÔÚÆ¥ÅäµÚ1¸ö×Ö·û£¡ËùÒÔÉÏÃæ³¬Ê±Ê±¼äÓ¦ÔõÃ´¸ø»¹²»ºÃ°ì£¬ÏÖÔÚ»¹²»ºÏÀí
		}
		
	}
	if(n == 1){//²éÕÒ´ÎÊý³¬ÏÞÖÆ£¬Ó¦·µ»ØÊ§°Ü
		flag=0xfd;
	}
	
	return flag;
}




/*
  ******************************************************************************
  * º¯ÊýÃû³Æ£ºuint8 CMD_AT(char* AtComd,char* AtRightAnsw,char* OrAtRightAnsw,uint16 OutTime,uint8 ATEMode)
  * Ëµ    Ã÷£º 
  * ²Î    Êý£ºATEMode:ÊÇ·ñµÄ´ø»ØÏÔ£¬TRUE£º´ø»ØÏÔ£» FLASE£º²»´ø»ØÏÔ
  * ±¸    ×¢: Ê¹ÓÃÉè±¸UP_COMMU_DEV_AT£¬¹Ê²»¿ÉÖØÈë£¬ÆäËûÊ¹ÓÃÉè±¸UP_COMMU_DEV_ATµÄº¯Êý²»ÄÜÓëÆä²¢·¢£¡
  ******************************************************************************
*/
uint8 CMD_AT(char* AtComd,char* AtRightAnsw,char* OrAtRightAnsw,uint16 OutTime,uint8 ATEMode)
{
	uint8 err;	
	
	UGprsWriteStr(AtComd);//×¢Òâº¯Êý·µ»ØºóÓ²¼þ·¢ËÍ²¢Ã»ÓÐÍê³É

	OSTimeDly(OS_TICKS_PER_SEC/8);
    	err=GprsGetHead(UP_COMMU_DEV_AT,AtRightAnsw,OrAtRightAnsw,OutTime,TRUE);/*yangfei modified */
    	if(err){
      	debug_err(gDebugModule[MSIC_MODULE], "[%s][%s][%d] AtRightAnsw err=%d\n",FILE_LINE,err);
     }
	
  	return err;
	
}


/*
  ******************************************************************************
  * º¯ÊýÃû³Æ£ºuint8 CMD_AT_RP(char* AtComd,char* AtRightAnsw,char* OrAtRightAnsw,uint16 OutTime,uint32 rp,uint8 ATEMode)
  * Ëµ    Ã÷£º 
  * ²Î    Êý£ºATEMode:ÊÇ·ñµÄ´ø»ØÏÔ£¬TRUE£º´ø»ØÏÔ£» FLASE£º²»´ø»ØÏÔ
  * ±¸    ×¢: Ê¹ÓÃÉè±¸UP_COMMU_DEV_AT£¬¹Ê²»¿ÉÖØÈë£¬ÆäËûÊ¹ÓÃÉè±¸UP_COMMU_DEV_ATµÄº¯Êý²»ÄÜÓëÆä²¢·¢£¡
  ******************************************************************************
*/
uint8 CMD_AT_RP(char* AtComd,char* AtRightAnsw,char* OrAtRightAnsw,uint16 OutTime,uint32 rp,uint8 ATEMode)
{
	uint32 i;
	uint8 err;
	
	for(i=0;i<1+rp;i++){//rp:ÖØÊÔ´ÎÊý£¬0 Îª²»ÖØÊÔ£¬Ö»·¢Ò»´Î
		err=CMD_AT(AtComd,AtRightAnsw,OrAtRightAnsw,OutTime,ATEMode);
		if(err==0){//³É¹¦
			break;
		}
		else{
			debug_err(gDebugModule[MSIC_MODULE], "[%s][%s][%d] CMD_AT err\n",FILE_LINE);
      		OSTimeDly(2*OS_TICKS_PER_SEC);//Ê§°ÜÖØÊÔÇ°¼ÓÑÓÊ±
    		}
	}
	
	return err;	
}



/*
  ******************************************************************************
  * º¯ÊýÃû³Æ£º void GprsRestart_xmz(void)
  * Ëµ    Ã÷£º GPRSÄ£¿éÖØÆô£¬Î÷ÃÅ×ÓÄ£¿é¡£
  * ²Î    Êý£º ÎÞ
  ******************************************************************************
*/
void GprsRestart_xmz(void)
{
	uint8 err;
	
	err=CMD_AT_RP(Ats_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,3,TRUE);
	printf("[%s][%s][%d] \n",FILE_LINE);
    	debug_debug(gDebugModule[GPRS_MODULE],"WARNING: <GprsRestart_xmz> GPRS Modul Shutdown!\n");
	OSTimeDly(OS_TICKS_PER_SEC);
	if(err == 0){//ÈÏÎªÏÖÔÚGPRSÄ£¿é´¦ÓÚ¿ª»ú×´Ì¬
		GprsOff_xmz();//½ô¼±¹Ø»ú
		printf("WARNING: <GprsRestart_xmz> GPRS Modul Power Off!\n");
		OSTimeDly(15*OS_TICKS_PER_SEC);
	}
    	debug_debug(gDebugModule[GPRS_MODULE],"INFO: <GprsRestart_xmz> GPRS Modul Power On, And ReStart!\n");
   	GprsOn_xmz();
	OSTimeDly(2*OS_TICKS_PER_SEC);
	
}


/*
  ******************************************************************************
  * º¯ÊýÃû³Æ£º void InitGprsRunSta(void)
  * Ëµ    Ã÷£º ³õÊ¼»¯GPRSÔËÐÐ×´Ì¬¡£
  * ²Î    Êý£º ÎÞ
  ******************************************************************************
*/
void InitGprsRunSta(void)
{	
	OS_ENTER_CRITICAL();
	//gGprsRunSta.ModuId = MODU_ID_UNKNOWN;
	gGprsRunSta.Ready = FALSE;
	gGprsRunSta.Connect = FALSE;
	gGprsRunSta.SocketType = 0;//0:TCP;1:UDP  //ÔÚReadyÎªTRUEºóÆäº¬Òå²ÅÓÐÐ§
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


//¶ÁÈ¡GPRSÔËÐÐ×´Ì¬
void ReadGprsRunSta(GPRS_RUN_STA* Sta)
{	
	OS_ENTER_CRITICAL();
	memcpy((uint8*)Sta,(uint8*)(&gGprsRunSta),sizeof(GPRS_RUN_STA));
	OS_EXIT_CRITICAL();
}

//¸üÐÂGPRSÔËÐÐ×´Ì¬
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
	UpdGprsRunSta_Stype(SOCKET_TCP);  //µ±Ç°¹Ì¶¨ÓÃTCP¡£
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
  * º¯ÊýÃû³Æ£º uint8 GprsTalk(uint32 rp)
  * Ëµ    Ã÷£º 
  * ²Î    Êý£º ÎÞ
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
  * º¯ÊýÃû³Æ£º uint8 ModelTalkIn(void)
  * Ëµ    Ã÷£º ºÍGPRSÄ£¿é»á»°Í¨Ñ¶
  * ²Î    Êý£º ÎÞ
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
	
	//»á»°Ê§°Ü,°´Î÷ÃÅ×ÓÄ£¿é½øÐÐ¿ª»ú´¦Àí²¢ÖØÐÂ»á»°
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
  * º¯ÊýÃû³Æ£º uint8 ModelIdentify(void)
  * Ëµ    Ã÷£º GPRSÄ£¿éÊ¶±ð,Ä¿Ç°Ö»Ö§³ÖÎ÷ÃÅ×ÓMC52IÄ£¿é¡£
  * ²Î    Êý£º ÎÞ
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
	
	//»á»°ÒÑ¾­³É¹¦,¶ÁÈ¡Ä£¿éID====
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
	
	
	//====ÒâÍâµÄÊ§°Ü
	OSTimeDly(5*OS_TICKS_PER_SEC);
	n--;
 	if(n == 0){
		return MODU_ID_UNKNOWN;
 	}
	
 	goto gplp;
	
}



/********************************************************************************************************
**  º¯ Êý  Ãû ³Æ: ConnectConfirm       									                               **
**	º¯ Êý  ¹¦ ÄÜ: µÇÂ¼È·ÈÏ£¬Ê§°Ü²¢ÖØÊÔ						                                           **
**	Êä Èë  ²Î Êý: none												                                   **
**	Êä ³ö  ²Î Êý: none											                                       **
**  ·µ   »Ø   Öµ: none																			   	   **
**	±¸		  ×¢:    					                                                               **
********************************************************************************************************/
uint8 ConnectConfirm(void)
{
	uint8 err = 0;
    	uint8 TryT = 0;
   	//int ret = 0;
    
	while(1){ 
		if(TryT++ >= 5){
			debug_err(gDebugModule[GPRS_MODULE], "ERROR: <ConnectConfirm> Retry five Times! GPRS UpLand Failure!\n");
			return FALSE; //5´ÎÃ»ÓÐÊÕµ½µÇÂ½Ö¡»ØÓ¦
		}

		OSTimeDly(OS_TICKS_PER_SEC);
		debug_err(gDebugModule[GPRS_MODULE],"GPRS Start Send Confirm frame !!!!\n");

		//×é½¨µÇÂ¼Ö¡¡£
		
		//setGprsXmlType(REQUEST);/*²É¼¯Æ÷ÇëÇóÉí·ÝÑéÖ¤£¨¸ÃÊý¾Ý°üÎª²É¼¯Æ÷·¢ËÍ¸ø·þÎñÆ÷£©*/       
        	//OSSemPend(SEQUEUE_XML, OS_TICKS_PER_SEC*30, &err);/*µÈ´ýsequence:·þÎñÆ÷·¢ËÍÒ»´®Ëæ»úÐòÁÐ*/
        	if(err != OS_ERR_NONE){	
    			debug_err(gDebugModule[GPRS_MODULE],"[%s][%s][%d]OSSemPend recive sequence err=%d\n",FILE_LINE,err);
    			continue;
		}

		//×é½¨MD5.
		//setGprsXmlType(MD5_XML);

		debug_info(gDebugModule[GPRS_MODULE], "INFO: sem_post MD5_XML!\n");/*²É¼¯Æ÷·¢ËÍ¼ÆËãµÄMD5£¬md5×ÓÔªËØÓÐÐ§*/
        	//OSSemPend(RESULT_XML, OS_TICKS_PER_SEC*30, &err);
        	if(err!=OS_ERR_NONE){
  		    	debug_err(gDebugModule[GPRS_MODULE],"[%s][%s][%d]OSSemPend recive result  err=%d\n",FILE_LINE,err);
			continue;
  		}
        	else{//ÊÕµ½»ØÓ¦
			debug_info(gDebugModule[GPRS_MODULE], "INFO: <ConnectConfirm> Recvie GPRS UpLand Confirm Frame OK!\n");
			break;
		}
		
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <ConnectConfirm> Retry Times is %d\n", TryT);
	}
	
	return TRUE;
}




/********************************************************************************************************
**  º¯ Êý  Ãû ³Æ: TaskGprsIpd       									                               **
**	º¯ Êý  ¹¦ ÄÜ: GPRSÑÓÊ±¼ì²â							                                           **
**	Êä Èë  ²Î Êý: time_s ÑÓÊ±µÄÃëÊý											                                   **
**	Êä ³ö  ²Î Êý: none											                                       **
**  ·µ   »Ø   Öµ: none																			   	   **
**	±¸		  ×¢: 						                                                               **
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
//ÓÃÓÚ½ÓÊÕÖÐ¼ä²¿·Ö³¤¶È²»¹Ì¶¨µÄATÃüÁîÓ¦´ðµÄ²»¹Ì¶¨³¤¶È²¿·Ö£¬Æ¥ÅäÎ²×Ö·ûºóÍË³ö£¬²¢¸ù¾Ý×îÐ¡µÄ¿ÉÄÜ³¤¶ÈºÍ×î´óµÄ¿ÉÄÜ³¤¶È·µ»Øerr
//ÓÉÓÚ×ÜÊÇÓÃÓÚ½ÓÊÕÖÐ¼ä²¿·Ö£¬½ÓÊÕ×Ö·ûµÄ³¬Ê±Ê±¼ä¹Ì¶¨ÎªOS_TICKS_PER_SEC/5
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
		*err = UpGetch(device,&tmp,4*OS_TICKS_PER_SEC);//½ÓÊÕ×Ö·ûµÄ³¬Ê±Ê±¼ä¹Ì¶¨Îª
		if(*err != OS_ERR_NONE){//³¬Ê±
			return n;
		}
		data[n++] = tmp;
		data[n] = '\0';//±ãÓÚµ÷ÊÔ¹Û²ì
		
	}while(tmp != Tail);
  
	if(n < MinLen){
		*err=0xfe;
	}
	
	return n;
	
}




//char AtaHead_CSQ[]="\r\n+CSQ: ";
//char Ata_CSQ[]="\r\n+CSQ: 27,99 \r\n\r\nOK\r\n";
//Ê¹ÓÃÉè±¸UP_COMMU_DEV_AT£¬¹Ê²»¿ÉÖØÈë£¬ÆäËûÊ¹ÓÃÉè±¸UP_COMMU_DEV_ATIPDµÄº¯Êý²»ÄÜÓëÆä²¢·¢£¡
//¹Ø±Õ»ØÏÔ·½Ê½ÏÂÊ¹ÓÃ
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
		//'\r'ºóÃæµÄ¿Õ¸ñ¾Í²»ÅÐ¶ÏÁË
		//CsqStr[len-3]='\0';
		//strcpy(pCsq,&CsqStr[1]);
	}
	else{
		return err;
	}
	return 0;
}



//½«×î¶à4¸öAscii±íÊ¾µÄ10½øÖÆÊý×ª»»³É uint16ÐÍHEXÊý£¬Èç"256"×ª»»Îª0x100
//ÊäÈë²ÎÊýÎª×Ö·û´®
uint8 AsciiDec(char* str,uint16* result)
{
	uint8 i,len;
	uint16 val,pow;
	len=strlen(str);
	
   if(len>4){
		return 0xff;//³¤¶È³¬ÏÞ
	}
	val=0;
	pow=1;
	for(i=0;i<len;i++){
		if(str[len-1-i]<0x30||str[len-1-i]>0x39){
				printf("[%s][%s][%d] str[len-1-i]=%d\n",FILE_LINE,str[len-1-i]);
		    return 0xfe;//×Ö·û·Ç·¨
		}
		val=val+(str[len-1-i]-0x30)*pow;
		pow*=10;
	}
	*result=val;
	return 0;	
}



//Ê¹ÓÃÉè±¸UP_COMMU_DEV_AT£¬¹Ê²»¿ÉÖØÈë£¬ÆäËûÊ¹ÓÃÉè±¸UP_COMMU_DEV_ATIPDµÄº¯Êý²»ÄÜÓëÆä²¢·¢£¡
//¹Ø±Õ»ØÏÔ·½Ê½ÏÂÊ¹ÓÃ
uint8 CMD_Csq(uint8* csq)//Î÷ÃÅ×Ó/»ªÎªÄ£¿é¹²ÓÃ,ÀïÃæGprs_Sem¶Ô»ªÎªÄ£¿éÀ´ËµÊÇ²»ÐèÒªµÄ,µ«ÊÇÎ÷ÃÅ×ÓÐèÒª
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
  * º¯ÊýÃû³Æ£º uint8 GprsInit_xmz(void)
  * Ëµ    Ã÷£º ³õÊ¼»¯Î÷ÃÅ×ÓGPRSÄ£¿é,½¨Á¢ÍøÂçÁ¬½Ó¡£
  * ²Î    Êý£º ÎÞ
  * ±¸    ×¢:  Ê¹ÓÃÉè±¸UP_COMMU_DEV_AT£¬¹Ê²»¿ÉÖØÈë£¬ÆäËûÊ¹ÓÃÉè±¸UP_COMMU_DEV_ATµÄº¯Êý²»ÄÜÓëÆä²¢·¢£¡
			Ö±½ÓÏòUART1 ·¢ËÍ£¬´Ëº¯Êý³É¹¦Ç°ÆäËûº¯Êý²»¿ÉÏòUART1 ·¢ËÍ£¬ÕâÍ¨¹ýÈ«¾Ö½á¹¹ÌågGprsRunStaÊµÏÖ
  ******************************************************************************
*/
uint8 GprsInit_xmz(void)
{	
	uint8 err;
	uint32 i, n=0;
    	printf("[%s][%s][%d] \n",FILE_LINE);
    
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
		//¶ÔÏÂÃæµÄËµÃ÷£¬¼¯ÖÐÆ÷µÚÒ»´Î²âÎ÷ÃÅ×ÓGPRSÄ£¿éÊ±£¬·¢ÏÖºÜ¶àÊ±ºòÏÂÃæ·¢AT»áÊ§°Ü1´Î£¬×ÐÏ¸²âÊÔ·¢ÏÖ£¬
		//Ä£¿é¿ª»úºó£¨·¢³ö^SYSSTARTºó5Ãë×óÓÒ£©Èç¹û·¢AT£¬»áÁ¢¼´ÏìÓ¦£¬µ«ÔÙ·¢AT£¬»áµÈ1Ãë¶à²ÅÏìÓ¦¡£
		//Òò´Ë£¬ÕâÀïµÚ¶þ´Î½ÓÊÕATµÄÏìÓ¦³É¹¦Êµ¼ÊÉÏÊÇÉÏÒ»´ÎµÄÖÍºó»ØÓ¦£¬¶øµÚ¶þ´ÎµÄATµÄÏìÓ¦ÔòÁôµ½¶ÓÁÐÀï£¬
		//²»¹ýÃ»ÓÐ¹ØÏµ£¬»á±»ÏÂÃæµÄIPRÃüÁî³Ôµô£¬ËùÒÔÕâÀï¾ÍÃ»ÓÐ×öÕë¶ÔµÄ´¦Àí
		
		gGprsFirst = FALSE;
		
		UpQueueFlush(UP_COMMU_DEV_AT); //Éè±¸UP_COMMU_DEV_ATµÄ¶ÓÁÐ£¬Çå¿Õ
		
		err = CMD_AT_RP(Ats_AT,Ata_OK,NULL,OS_TICKS_PER_SEC,3,TRUE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){		
            	debug_err(gDebugModule[GPRS_MODULE], "WARNING: <GprsInit_xmz> Ats_AT CMD Err!\n");
			continue;
		}
		
        	#ifndef GPRS_ECHO
       	 /*¹Ø±Õ»ØÏÔ*/
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

	 	strcpy(Ats_SISS_address,Ats_SISS_address_h);
        	strcat(Ats_SISS_address,"122.5.18.174");
        	strcat(Ats_SISS_address,colon);
        	strcat(Ats_SISS_address,"9014");
        	strcat(Ats_SISS_address,over124);
        	err = CMD_AT_RP(Ats_SISS_address,Ata_OK123,NULL,OS_TICKS_PER_SEC*3,3,FALSE);
		OSTimeDly(OS_TICKS_PER_SEC);
		if(err){
      		debug_err(gDebugModule[GPRS_MODULE],"<GprsInit_xmz> Ats_SISS_address CMD Err!");
			continue;
		}
		debug_info(gDebugModule[GPRS_MODULE], "INFO: <GprsInit_xmz> Ats_SISS_address CMD OK!\n");
        
		UpQueueFlush(UP_COMMU_DEV_ATIPD);
		//´ò¿ªÁ¬½Ó
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
  * º¯ÊýÃû³Æ£º void  GPRS_Mana_Proc(void *pdata)
  * Ëµ    Ã÷£º GPRS¹ÜÀíÏà¹Ø£¬°üÀ¨ÍøÂçÁ¬½Ó£¬¶¨Ê±ÐÄÌø£¬Ä£¿é×´Ì¬¼ì²â£¬Ä£¿éÖØÆô»úÖÆ¡£
  * ²Î    Êý£º ÎÞ
  ******************************************************************************
*/
uint8 gGprsFirst;  //±ê¼ÇGPRSÄ£¿éÊÇ²»ÊÇµÚÒ»´Î³õÊ¼»¯¡£
void  GPRS_Mana_Proc(void *pdata)
{
	uint8 tmpmid = 0;
	GPRS_RUN_STA GprsRunSta;
	//uint32 heartcy_sec;//ÐÄÌøÖÜÆÚ£¬µ¥Î»Ãë
	uint32 ReStartCounter = 0x00;
	uint32 selfchecknum = 0;
	uint32 Cycles;
	uint32 HeartFrmSndCycles;



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
		
	}while(tmpmid == MODU_ID_UNKNOWN);  //¼ì²âµ½ÒÑÖªÄ£¿é²Å»áÍË³ö¡£

	for(;;){ 
		ReStartCounter++;
		if(ReStartCounter > 1){
			debug_info(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] GPRS Modul Restart, Retry Times is %2d\n", ReStartCounter);
		}
		else{
			debug_info(gDebugModule[GPRS_MODULE],"INFO: [TaskGprsMana] GPRS Modul Start!\n");
		}

		//sem_wait(&GprsIPDTask_sem);  //GPRSÄ£¿é³õÊ¼»¯½¨Á¢ÍøÂçÇ°£¬ÏÈ¹Ø±ÕGprsIPDÏß³Ì¡£GprsIPD¿ÉÒÔÒ»Ö±¿ª×Å¡£
	   
		InitGprsRunSta();//GPRSÃ»ÓÐ¾ÍÐ÷Ê± 		
		tmpmid = GetGprsRunSta_ModuId();
		
		if(tmpmid == MODU_ID_XMZ){
    			GprsInit_xmz(); //Ê¹ÓÃÉè±¸ UP_COMMU_DEV_AT³õÊ¼»¯Î÷ÃÅ×ÓÄ£¿é¡£
    			debug_info(gDebugModule[GPRS_MODULE],"GPRS  XMZ  init OK!! \n");
    		}  
        	else{
               debug_err(gDebugModule[GPRS_MODULE],"GPRS MODU_ID_UNKNOWN!! \n");
          }
		
	  	UpdGprsRunSta_Ready(TRUE);
		//gprs readyºó£¬Éè±¸ UP_COMMU_DEV_ATµÄÊ¹ÓÃÈ¨½»¸øTaskUpSend£¬ÏÂÃæ±¾ÈÎÎñ½«
		//¶¨Ê±¼ì²éGPRS×´Ì¬£¬È·¶¨ÊÇ·ñÐèÒªÖØÐÂÆô¶¯£¬²éÑ¯GPRS×´Ì¬Ê¹ÓÃµÄÊÇÉè±¸UP_COMMU_DEV_AT£¬¹Ê±¾ÈÎÎñ²»Ö±½ÓÊ¹ÓÃ£¬
	  	//¶øÊÇÏòTaskUpSend·¢³öÇëÇó£¬TaskUpSend½«½á¹ûÐ´ÈëÈ«¾Ö×´Ì¬ÐÅÏ¢
	  	//ÊÇ·ñ±¾µØ¿ÚÒ²ÏÞÖÆ£¿Ò²¾ÍÊÇÏÔÊ¾Ð´´®¿Ú1
	  
	  	//sem_post(&GprsIPDTask_sem);  //GPRSÄ£¿é³õÊ¼»¯½¨Á¢ÍøÂçÍê³Éºó£¬Ê¹ÄÜGprsIPDÏß³Ì¡£

	  	if(ConnectConfirm() == FALSE){
	  		continue;
	  	}

	  	//OSMboxPost(HeartFrmMbox,(void*)1);//Í¨ÖªÖ÷¶¯ÉÏ±¨ÈÎÎñ·¢ÐÄÌøÖ¡,µÚ1Ö¡£¨µÇÂ½Ö¡£©
	  	//==========Èç¹û·¢ÍêµÇÂ½Ö¡ºóÂíÉÏ»¹Òª·¢1¸öÐÄÌøÖ¡,ÔòÓ¦¼ÓÏÂÃæÁ½¾ä,·ñÔò,ÔÚ1¸öÐÄÌøÖÜÆÚºó²Å¿ªÊ¼·¢ÐÄÌøÖ¡
	  	//OSTimeDly(OS_TICKS_PER_SEC);
	  	//OSMboxPost(HeartFrmMbox,(void*)3);////Í¨ÖªÖ÷¶¯ÉÏ±¨ÈÎÎñ·¢ÐÄÌøÖ¡
	  	//==========
	  	////
	  	UpdGprsRunSta_Connect(TRUE);
		
	  	selfchecknum=0;
		
	  	while(1){
	  		DlyGprsCheck(10);
	  		UpdGprsRunSta_AddSndDog();
	  		UpdGprsRunSta_AddRecDog();	
			
	  		selfchecknum = (selfchecknum+1)%(GPRS_SELF_CHECK_CYCLE/GPRS_CHECK_CYCLE);
	  		if(selfchecknum==0 && gGPRSManaBusy==0){
	  			//========·¢ËÍÏûÏ¢£¬Í¨Öª·¢ËÍÈÎÎñ¼ì²éGPRS×´Ì¬£¬½á¹ûÌîÈëgGprsRunSta
	  			gGPRSManaBusy = 1;
	  			gUpCommMsg.GprsGvc = TRUE;

                	printf("[%s][%s][%d] GprsGvc=TRUE \n",FILE_LINE);
                	//sem_post(&UpSend_Sem);  //ÔÙ´´½¨Ò»¸ö×¨ÃÅÉÏ´«ÐÅÏ¢µÄÈÎÎñ¡£
                	//sem_wait(&GprsMana_Sem);  //Èç¹û³¤Ê±¼äÃ»ÓÐÕâ¸öÐÅºÅÁ¿·¢³ö£¬»á²»»áÒ»Ö±ÔÚÕâÀïµÈ´ý¡?
	  		}
	  		
	  		ReadGprsRunSta(&GprsRunSta);
	  		
	  		if(GprsRunSta.IpCloseNum >= 2){
				debug_err(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] GprsRunSta.IpCloseNum>=2! Modul Restart!\n");
	  			goto restart;
	  		}

	  		Cycles = GPRS_HEART_FRM_REC_OUTTIME / GPRS_CHECK_CYCLE;
	  		//Cycles += HeartFrmSndCycles;
	  		if(GprsRunSta.RecDog > Cycles){
				debug_err(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] Heart Frame Recive Out Time! Module will Restart!\n");
	  			goto restart;
	  		}
    
            	if(gGprsRunSta.IpSendFailNum > 2){
                	debug_err(gDebugModule[GPRS_MODULE],"WARNING: [TaskGprsMana] IpSendFailNum >2! Module will Restart!\n");
	  			goto restart;
              	}

			#if GPRS_HEART_FRM_TIME%GPRS_CHECK_CYCLE == 0
				HeartFrmSndCycles = GPRS_HEART_FRM_TIME/GPRS_CHECK_CYCLE;
			#else
				HeartFrmSndCycles = GPRS_HEART_FRM_TIME/GPRS_CHECK_CYCLE+1;
			#endif

	  		if(GprsRunSta.SndDog>=HeartFrmSndCycles && GprsRunSta.RecDog>=HeartFrmSndCycles){
	                debug_info(gDebugModule[GPRS_MODULE],"[%s][%s][%d] sem_post NOTIFY ÐÄÌø\n",FILE_LINE);
	                gGPRSManaBusy = 0;
				 // gjt add 0607 for xml send
				 //sem_wait(&Xml_Send_Sem);
				 // gjt add end
	  			 //gGPRS_XMLType = NOTIFY;
	  			 //sem_post(&ReportUp_Sem);
          
	  		}
			
           	continue;

           
 restart:
          	UpdGprsRunSta_Connect(FALSE);/*GPRS ¶ÏÏß*/
	  		break;/*ÖØÐÂÆô¶¯GPRS*/
         
	  	} 
		
	}

}



/*
  ******************************************************************************
  * º¯ÊýÃû³Æ£º int pthread_GPRS_Mana(void)
  * Ëµ    Ã÷£º GPRS¹ÜÀíÏß³Ì¡£
  * ²Î    Êý£º ÎÞ
  ******************************************************************************
*/

int pthread_GPRS_Mana(void)
{

	GPRS_Mana_Proc(NULL);

	return(CONTROL_RET_SUC);

}





