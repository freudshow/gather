/*
  *******************************************************************************
  * @file    read_heatmeters.c 
  * @author  zjjin
  * @version V0.0.0
  * @date    03-05-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
*/ 

#include "includes.h"
#include "readallmeters.h"
#include "read_heatmeter.h"
#include "meter_table.h"
#include "commap.h"






/*
  ******************************************************************************
  * �������ƣ�void CreateFrame_CJ188(MeterFileType *pmf,uint8 *pDataTemp,uint8 *plen)
  * ˵    ����
  * ��    ���� 
  			
  ******************************************************************************
*/
static uint8 gMETER_FrameSer = 0x00;

void CreateFrame_CJ188(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
	//uint16 lu8BufSize = *plen;   //pDataTempָ��ָ��ռ�Ĵ�С���Է�����,��ǰ��256�϶����ã���û�������Ժ����ơ�
	DELU_Protocol	ProtocoalInfo;
	uint8 *pTemp = pDataTemp;
	uint8 *pCSStart; //��¼У�������ʼλ�á�
	uint8 lu8FrameLen = 0;
	uint8 lu8CheckLen = 0;
	uint8 lu8CS = 0;
	uint8 lu8TempLen = 0;

	ProtocoalInfo.PreSmybolNum  = gMETER_Table[pmf->u8ProtocolType][2];
	ProtocoalInfo.MeterType = pmf->u8MeterType;
	memcpy(ProtocoalInfo.MeterAddr,  pmf->u8MeterAddr, 7);
	ProtocoalInfo.ControlCode 	= 0x01;
	ProtocoalInfo.Length		= 0x03;
	ProtocoalInfo.DataIdentifier= gMETER_Table[pmf->u8ProtocolType][1];

	pTemp = pDataTemp;
	
	memset(pTemp, 0xFE, ProtocoalInfo.PreSmybolNum);		//����ǰ���ַ�0xFE, �������������
	pTemp += ProtocoalInfo.PreSmybolNum;
	lu8FrameLen += ProtocoalInfo.PreSmybolNum;
	
	pCSStart = pTemp;							//Ԥ������У��λ
	*pTemp++ = DELU_FRAME_START;					//�齨 ֡ͷ 0x68
	*pTemp++ = ProtocoalInfo.MeterType;					//�Ǳ�����
	memcpy(pTemp, ProtocoalInfo.MeterAddr, 7);				//�齨 ��ַ��
	pTemp += 7;
	lu8FrameLen += 9;
	lu8CheckLen += 9;
	
	*pTemp++ = ProtocoalInfo.ControlCode;					//�齨 ������
	*pTemp++ = ProtocoalInfo.Length;						//�齨 �����򳤶�
	lu8FrameLen += 2;
	lu8CheckLen += 2;
	
	*pTemp++ = ProtocoalInfo.DataIdentifier;
	*pTemp++ = (ProtocoalInfo.DataIdentifier) >> 8;		//�齨 ���ݱ�ʶ��
	*pTemp++ = gMETER_FrameSer++;
	lu8FrameLen += 3;
	lu8CheckLen += 3;
	
	lu8TempLen =ProtocoalInfo.Length - 3;
	memcpy(pTemp, ProtocoalInfo.DataBuf, lu8TempLen);
	pTemp += lu8TempLen;
	lu8FrameLen += lu8TempLen;
	lu8CheckLen += lu8TempLen;
	
	lu8CS = PUBLIC_CountCS(pCSStart, lu8CheckLen);			//���� У���ֽ�
	*pTemp++ = lu8CS;									//�齨 У��λ
	*pTemp++ = 0x16;									//�齨 ����λ
	lu8FrameLen += 2;									//�齨 ����֡����

	*pTemp++ = 0x16;	//��ǿ485����1200bps��,���һ�ֽ����ǲ��ԣ���֡���һ�ֽڡ�
	lu8FrameLen += 1;	
	
	*plen = lu8FrameLen;							//��������֡����



}


void CreateFrame_lcMod(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
    uint16 crcsum;
    pDataTemp[0] = pmf->u8MeterAddr[0];
    pDataTemp[1] = MODBUS_READ;
    pDataTemp[2] = (LC_ELEC_WORK_START >> LEN_BYTE);
    pDataTemp[3] = LC_ELEC_WORK_START;
    pDataTemp[4] = (LC_ELEC_WORK_LEN >> LEN_BYTE);
    pDataTemp[5] = LC_ELEC_WORK_LEN;
    crcsum = crc16ModRtu(pDataTemp, 6);
    pDataTemp[6] = crcsum;//��λ
    pDataTemp[7] = (crcsum >> LEN_BYTE);//��λ 
    *plen = 8;
}




/****************************************************************************************************
**	�� ��  �� ��: METER_ReceiveFrame
**	�� ��  �� ��: �������еļ���ģ�������λ���·��Ķ������������
**	�� ��  �� ��: uint8 dev -- ����ͨ���豸; 
**                uint16 Out_Time -- �������ʱ�ȴ�ʱ��
**	�� ��  �� ��: uint8 *buf -- �������ָ��; 
**                uint8 *datalen -- ������յ�����֡����
**  ��   ��   ֵ: 0 -- ���յ���Ч֡;		1 -- ����֡ͷ����ȷ; 		2 -- ���յ�������֡���Ǳ����ܱ��ַ
**				  4 -- ����֡���Ȳ���		5 -- ����֡У�鲻��ȷ		6 -- ����֡β����ȷ
**	��		  ע: �˴��������������������궨�壬����ķ���ֵ
*****************************************************************************************************/

uint8 CJ188_ReceiveFrame(uint8 dev,uint16 Out_Time,uint8 *buf,uint16 *datalen)
{
	uint8 data 	= 0x00;
	uint8 len	= 0x00;
	uint8 Cs	= 0x00;
	uint32 i,j;
	uint8 lu8FEnum = 0;//��¼����֡ͷFE��������

	
	*datalen = 0;
	   	
	i = 500;
	lu8FEnum = 0;
	while(i--){	//��֡ͷ
		if(DownDevGetch(dev, &data, Out_Time)) 
			return 1;

		if(data == 0xfe)
			lu8FEnum += 1;
		else	if(data == 0x68){	
			if(lu8FEnum >= 2)
				break; 
			else
				lu8FEnum = 0;
		}
		else
			lu8FEnum = 0;
		
	}
	

	if(i == 0)   //˵��û�ҵ�ͷ��ֱ�ӷ���1.
		return 1;
	
	
	Cs 		= data;
	*buf++ 	= data;
	
	if(DownDevGetch(dev, &data, Out_Time))  		
		return 2;		//������
	Cs 		+= data;
	*buf++	 = data;
	
    for(i=0; i<7; i++)												//��ַ
    {
		if(DownDevGetch(dev, &data, Out_Time))  	
			return 3;
		Cs 		+= data;
		*buf++	 = data;
	}
	
	if(DownDevGetch(dev, &data, Out_Time))  		
		return 4;		//������
	Cs 		+= data;
	*buf++	 = data;
	
	if(DownDevGetch(dev, &len, Out_Time))  		
		return 5;		//���ݳ���
	if(len > METER_FRAME_LEN_MAX)             		
		return 10;
	Cs 		+= len;
	*buf++	 = len;

	for(j=0; j<len; j++)											//������
	{
	    if(DownDevGetch(dev, &data, Out_Time))  	
			return 6;
		*buf++  = data;
		Cs     += data;
    }  
     
	if(DownDevGetch(dev, &data, Out_Time))  		
		return 7;		//У���ֽ�
	if(data != Cs)         							
		return 11; 
	
	*buf++	= data; 
	
	if(DownDevGetch(dev, &data, Out_Time))  		
		return 8;		//������
	if(data != 0x16)         						
		return 12;
	   
  	*buf++		= data; 
  	*datalen 	= len + 16;
  	  
    return NO_ERR;
    
}

uint8 Elec_ReceiveFrame(MeterFileType *pmf, uint8 dev,uint16 Out_Time,uint8 *buf,uint16 *datalen)
{
    uint8 data 	= 0x00;
    uint8 len	= 0x00;
    uint8* pBuf   = buf;
    uint32 i;
    uint8 datafield_len = 0;//������ĳ���
    uint8 data_idx = 0;//�����������
    *datalen = 0;

    i = 500;
    lcElecAnsSt stat = em_init_state;
    data_idx = 0;
    while(i--){
        if(DownDevGetch(dev, &data, Out_Time)) 
            return ERR_1;
        //printf("[%s][%s][%d]data: %02x, pmf->u8MeterAddr[0]: %02x, state: %02x\n", FILE_LINE, data, pmf->u8MeterAddr[0], stat);
        switch(stat){
        case em_init_state:
            if((uint8)data == (uint8)pmf->u8MeterAddr[0]) {
                len++;
                *pBuf++ = data;
                stat = em_address_state;
            }
            else
                continue;
            break;
        case em_address_state:
            if(data == MODBUS_READ) {
                len++;
                *pBuf++ = data;
                stat = em_func_state;
            }
            else
                return ERR_1;
            break;
        case em_func_state:
            len++;
            *pBuf++ = data;
            datafield_len = data;
            stat = em_length_state;
            break;
        case em_length_state:
            len++;
            *pBuf++ = data;
            data_idx++;
            if(data_idx > datafield_len) {
                stat = em_crc_state;
            }
            else
                stat = em_data_state;
            break;
        case em_data_state:
            len++;
            *pBuf++ = data;
            data_idx++;
            if(data_idx > datafield_len) {
                stat = em_crc_state;
            }
            break;
        case em_crc_state:
            len++;
            *pBuf++ = data;
            goto end;
            break;
        default:
            break;
        }
    }
    end:
        *datalen = len;
    return NO_ERR;
}


/*
  ******************************************************************************
  * �������ƣ�HeatMeter_DataSendAndRec(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
  * ˵    �������ͳ�������֡���ȴ��������ݣ������ؽ����Ƿ���ȷ��
  * ��    ���� *pmf ����Ϣ;
  			*pDataTemp ���������ݴ��λ��ָ�룻
  			*plen ������������Ч���ȡ�
  			���� �ɹ� or ʧ�ܡ�  			
  ******************************************************************************
*/

uint8 HeatMeter_DataSendAndRec(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
	uint8 err = 0;
	uint32 lu32len = *plen;
	uint8 lu8protocoltype = 0;
	uint16 lu16OutTime = 2000;//���롣
	uint8 lu8DownComDev = DOWN_COMMU_DEV_485;

	if(pmf->u8Channel == RS485_DOWN_CHANNEL)
		lu8DownComDev = DOWN_COMMU_DEV_485;
	else
		lu8DownComDev = DOWN_COMMU_DEV_MBUS;

	DownDevSend(lu8DownComDev,pDataTemp,lu32len);  //�������ݡ�

	lu8protocoltype = pmf->u8ProtocolType;

	switch(lu8protocoltype){
		case HYDROMETER775_VER:
		case ZENNER_VER:

			break;
		

		default://Ĭ�ϳ�CJ188��ʽ��
			err = CJ188_ReceiveFrame(lu8DownComDev,lu16OutTime,pDataTemp,plen);
			//printf("CJ188_ReceiveFrame err = %d.\n",err);  //������
			break;



	}	


	return err;
	
}

uint8 RS4852Down_DataSend_byspeed(uint8 * Data,uint32 n,uint32 speed);

uint8 ElecMeter_DataSendAndRec(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
	uint8 err = 0;
	uint32 lu32len = *plen;
	uint16 lu16OutTime = 2000;//���롣
	uint8 lu8DownComDev = DOWN_COMMU_DEV_485;
	if(pmf->u8Channel == RS485_DOWN_CHANNEL)
		lu8DownComDev = DOWN_COMMU_DEV_485;
	else
		lu8DownComDev = DOWN_COMMU_DEV_MBUS;

	//DownDevSend(lu8DownComDev,pDataTemp,lu32len);  //�������ݡ�
    
	switch(pmf->u8ProtocolType){
		case ELEC_LC_MODBUS:
            RS4852Down_DataSend_byspeed(pDataTemp, lu32len, 9600);
            err = Elec_ReceiveFrame(pmf,lu8DownComDev,lu16OutTime,pDataTemp,plen);
            //for(i=0;i<*plen;i++)
            //    printf(" 0x%02x ", pDataTemp[i]);
            //printf("\n");
            break;
		default://Ĭ�ϳ�CJ188��ʽ��
			break;
	}	


	return err;
	
}




/****************************************************************************************************
**	�� ��  �� ��: uint8 Meter_AnalDataFrm_CJ188(uint8 *pRecFrame,uint8 *pLen)
**	�� ��  �� ��: ������CJ188���豸�������ݴ���ȡ����Ч���ݣ������ء�
**	�� ��  �� ��: 
**	�� ��  �� ��: 
**   ��   ��   ֵ: 
**	��		ע: 
*****************************************************************************************************/
uint8 Meter_AnalDataFrm_CJ188(uint8 *pRecFrame,uint8 *pLen)
{
	uint8 *pTemp		= pRecFrame;
	DELU_Protocol ProtoclData;
		
	
	if(0x68 != *pTemp++)	return 1; 
			
	ProtoclData.MeterType = *pTemp++;						//�Ǳ�����
	memcpy(ProtoclData.MeterAddr, pTemp, 7);					//�Ǳ��ַ
	pTemp += 7;
			
	ProtoclData.ControlCode	= *pTemp++;					//������
	ProtoclData.Length		= *pTemp++;					//����
	ProtoclData.DataIdentifier = *pTemp++;
	ProtoclData.DataIdentifier |= (*pTemp++ << 8);
	ProtoclData.SER			= *pTemp++;
	memcpy(ProtoclData.DataBuf, pTemp, (ProtoclData.Length-3));	//����������
	pTemp += ProtoclData.Length - 3;
	
	memcpy(pRecFrame,ProtoclData.DataBuf, (ProtoclData.Length-3));
	*pLen = ProtoclData.Length - 3;

	
	return NO_ERR;
	
}



/*
  ******************************************************************************
  * �������ƣ�uint8 HeatMeter_DataDeal(MeterFileType *pmf,uint8 *pDataBuf,uint16 *pLen,CJ188_Format *pCJ188Data)
  * ˵    �����������ص����ݣ��������ͳһCJ188��ʽ��
  * ��    ����
  			
  ******************************************************************************
*/

uint8 HeatMeter_DataDeal(MeterFileType *pmf,uint8 *pDataBuf,uint16 *pLen,CJ188_Format *pCJ188Data)
{
	uint8 err = 0;
	uint8 lu8len = 0;
	uint8 lu8ProtocolType = 0;
	//CJ188_Format  MeterData={0,0x05,0,0x05,0,0x17,0,0x35,0,0x2c};

	lu8ProtocolType = pmf->u8ProtocolType;

	switch(lu8ProtocolType){



		default:{  //���ϵ�³CJ188Э��ģ�Ĭ�ϡ�
			err = Meter_AnalDataFrm_CJ188(pDataBuf,&lu8len);
			memcpy(pCJ188Data,pDataBuf,sizeof(CJ188_Format));
		}
		break;

	}

	
	return err;

}

uint8 ElecMeter_DataDeal(MeterFileType *pmf,uint8 *pDataBuf,uint16 *pLen, lcModbusElec_str *pData)
{
    uint8 err = NO_ERR;
    uint8 len = 0;
    switch(pmf->u8ProtocolType){
    case ELEC_LC_MODBUS:
        pDataBuf += 2;//�Թ���ַ��
        len = *pDataBuf++;
        len = len;
        pData->pact_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
        pDataBuf += 4;
        strcpy((char*)pData->pact_tot_elec_unit, "+KWh");
        pData->nact_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
        pDataBuf += 4;
        strcpy((char*)pData->nact_tot_elec_unit, "-KWh");
        pData->preact_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
        pDataBuf += 4;
        strcpy((char*)pData->preact_tot_elec_unit, "+KVarh");
        pData->nreact_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
        pDataBuf += 4;
        strcpy((char*)pData->nreact_tot_elec_unit, "-KVarh");
        pData->act_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
        pDataBuf += 4;
        strcpy((char*)pData->act_tot_elec_unit, "KWh");
        pData->react_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
        strcpy((char*)pData->react_tot_elec_unit, "KVarh");
        printf("[%s][%s][%d]pact_tot_elec: %f, pact_tot_elec: %f, pact_tot_elec: %f, pact_tot_elec: %f, pact_tot_elec: %f, pact_tot_elec: %f, \n", \
            FILE_LINE, pData->pact_tot_elec, pData->nact_tot_elec, pData->preact_tot_elec, pData->nreact_tot_elec, \
            pData->act_tot_elec, pData->react_tot_elec);
        break;
    default:
        break;
    }

    return err;
}





/*
  ******************************************************************************
  * �������ƣ�Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo)
  * ˵    �����齨��������֡�����ȱ�,������ͬ���ҵ��ȱ������ݴ���ɹ̶�ͳһ��ʽ��
  * ��    ���� *pmf ����Ϣ��
  			
  ******************************************************************************
*/

uint8 HeatMeterCommunicate(MeterFileType *pmf,CJ188_Format *pCJ188Data)
{
	uint8 lu8DataTemp[256] = {0};  //ŷ��Ƚϳ��������256.
	uint16 lu16len = 0;
	uint8 err = 0;

	if(pmf->u8ProtocolType == HYDROMETER775_VER || pmf->u8ProtocolType == ZENNER_VER){  //һЩŷ��ı�

	}
	else{	//CJ188Э��ı��������ﴦ��	
		lu16len = sizeof(lu8DataTemp);  //CreateFrame_CJ188()�д���洢�ռ��С�����ⳬ�ޡ�
		CreateFrame_CJ188(pmf,lu8DataTemp,&lu16len);  //��������֡��
		err = HeatMeter_DataSendAndRec(pmf,lu8DataTemp,&lu16len);
		if(err == NO_ERR){
			err = HeatMeter_DataDeal(pmf,lu8DataTemp,&lu16len,pCJ188Data);//�������յ������ݣ��������ͳһCJ188��ʽ��
		}

		
	}
	

	return err;

}

uint8 ElecMeterCommunicate(MeterFileType *pmf,lcModbusElec_str *pData)
{
	uint8 lu8DataTemp[256] = {0};  //����֡
	uint16 lu16len = 0;//����֡�ĳ���
	uint8 err = 0;

	lu16len = sizeof(lu8DataTemp);  //CreateFrame_CJ188()�д���洢�ռ��С�����ⳬ�ޡ�
	CreateFrame_lcMod(pmf,lu8DataTemp,&lu16len);  //��������֡��
	err = ElecMeter_DataSendAndRec(pmf,lu8DataTemp,&lu16len);
    //int i;
    //printf("[%s][%s][%d] ", FILE_LINE);
    //for(i=0;i<lu16len;i++)
    //    printf(" 0x%02x ", lu8DataTemp[i]);
    //printf("\n");
	if(err == NO_ERR){
		err = ElecMeter_DataDeal(pmf,lu8DataTemp,&lu16len, pData);//�������յ�������
	}		
	return err;
}


/*
  ******************************************************************************
  * �������ƣ�HeatData_insertDB(MeterFileType *pmf,CJ188_Format *pCJ188Data,struct tm *pNowTime,struct tm *pTimeNode)
  * ˵    ���������ȱ����ݴ������ݿ����С�
  * ��    ���� 
  


  ******************************************************************************
*/




/*
  ******************************************************************************
  * �������ƣ�Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo)
  * ˵    �������ȱ�����
  * ��    ���� *pmf ����Ϣ��
  			*pPositionInfo ��װλ����Ϣ��
  * ��    ע:
   struct tm { 		   pNowTime = localtime(&timep);  
	  int tm_sec;     printf("%d %d %d \n",(1900+pNowTime->tm_year), (1+pNowTime->tm_mon),pNowTime->tm_mday); 
	  int tm_min;     printf("%d:%d:%d\n", pNowTime->tm_hour, pNowTime->tm_min, pNowTime->tm_sec);
	  int tm_hour; 
	  int tm_mday;    �������ϴ���󣬼��ǵ�ǰϵͳʱ�䡣
	  int tm_mon; 
	  int tm_year; 
	  int tm_wday; 
	  int tm_yday; 
	  int tm_isdst; 
  };
  ******************************************************************************
*/

uint8 Read_HeatMeter(MeterFileType *pmf)
{
	uint8 err = 0;
	char lcRet[100];
	uint8 lu8retrytimes = 0;
	//DELU_Protocol	ProtocoalInfo;
	CJ188_Format CJ188_Data;
	struct tm NowTime;
	time_t timep;

	

	if(pmf->u8ProtocolType >= HEATMETER_PROTO_SUM){  //��ֹЭ��汾�ų��ޡ�
		//Э��汾�ų��޴���
	}		

	//����Э��汾�ţ����ö�Ӧ�˿ڲ���.
	lu8retrytimes = 3;
	do{
		err = (*METER_ComParaSetArray[gMETER_Table[pmf->u8ProtocolType][0]])();  //���ճ���Э�飬���ó����ڲ�����
		lu8retrytimes --;

	}while((err != TRUE) && (lu8retrytimes > 0));

	//�齨��������֡�����ȱ�,������ͬ���ҵ��ȱ������ݴ���ɹ̶�ͳһ��ʽ��
	err = HeatMeterCommunicate(pmf,&CJ188_Data);

	//����t_request_data�������ã����������ݴ����Ӧ���ݿ���
	if(err == NO_ERR){
		//��ȡ��ǰʱ��				
		time(&timep); 
		localtime_r(&timep, &NowTime); 
		
		insert_his_data(pmf,&CJ188_Data,&NowTime,&gTimeNode,lcRet);
		//printf("insert_his_data over.\n");
	}


	return err;

}


uint8 Read_ElecMeter(MeterFileType *pmf)
{
    uint8 err = NO_ERR;
    char lcRet[100];
    uint8 lu8retrytimes = 0;
    lcModbusElec_str lcModStr;
    struct tm NowTime;
    time_t timep;
    if(pmf->u8ProtocolType >= ELECMETER_PROTO_SUM){  //��ֹЭ��汾�ų��ޡ�
        return ERR_1;
    }

    //����Э��汾�ţ����ö�Ӧ�˿ڲ���.
    lu8retrytimes = 3;
    do{
        err = (*ELEC_METER_ComParaSetArray[gELEC_METER_Table[pmf->u8ProtocolType][0]])();  //���ճ���Э�飬���ó����ڲ�����
        lu8retrytimes--;
    }while((err != TRUE) && (lu8retrytimes > 0));

    //�齨��������֡
	err = ElecMeterCommunicate(pmf,&lcModStr);
    
    //����t_request_data�������ã����������ݴ����Ӧ���ݿ���
    if(err == NO_ERR){
        time(&timep); 
        localtime_r(&timep, &NowTime); 
        insert_his_data(pmf,&lcModStr,&NowTime,&gTimeNode,lcRet);
    }
    return err;
}








