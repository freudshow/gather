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
	
	pRecFrame = ProtoclData.DataBuf;
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





/*
  ******************************************************************************
  * �������ƣ�Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo)
  * ˵    �������ȱ�����
  * ��    ���� *pmf ����Ϣ��
  			*pPositionInfo ��װλ����Ϣ��
  ******************************************************************************
*/

uint8 Read_HeatMeter(MeterFileType *pmf,char *pPositionInfo)
{
	uint8 err = 0;
	uint8 lu8retrytimes = 0;
	//DELU_Protocol	ProtocoalInfo;
	CJ188_Format CJ188_Data;
	

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


	}

	



	return err;

}










