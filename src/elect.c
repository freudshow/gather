#include "globaldefine.h"
#include "readallmeters.h"
#include "commap.h"
#include "elect.h"


uint8 (*ELEC_METER_ComParaSetArray[])(void) = {METER_ComSet8};

/*****************************************************************
 *  Table ��ʽΪ���������ú���ָ���� + ��ȡ������ݱ�ʶ�� +
 *  ǰ��������+ͨѶ����(0:MBUS  1:485)+ͨѶЭ������(0:MODBUS  1:645)
 ****************************************************************/
uint16 gELEC_METER_Table[ELECMETER_PROTO_SUM][5] = {
	{ELEC_COMSET_1, 0x0000, 0, 1, 0},//�������	-----idx: -0-
	{ELEC_COMSET_1, 0x0000, 0, 1, 0},//�������	-----idx: -0-
};





void CreateFrame_lcMod(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
	uint16 crcsum;
	pDataTemp[0] = pmf->u8MeterAddr[0];//����modbos�ı��ַ����Ϊ1�ֽ�
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

uint8 ElecLcMod_ReceiveFrame(MeterFileType *pmf, uint8 dev,uint16 Out_Time,uint8 *buf,uint16 *datalen)
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
	if(i<0) {
		*datalen = 0;
		return ERR_1;
	} else {
		*datalen = len;
	}
	return NO_ERR;
}

uint8 ElecMeter_DataSendAndRec(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
	uint8 err = 0;
	uint32 lu32len = *plen;
	uint16 lu16OutTime = 2000;//���롣
	uint8 lu8DownComDev = DOWN_COMMU_DEV_485;
	char log[4096];
	char tmplog[5];
	int i;

	if(pmf->u8Channel == RS485_DOWN_CHANNEL) {
		lu8DownComDev = DOWN_COMMU_DEV_485;
	} else {
		lu8DownComDev = DOWN_COMMU_DEV_MBUS;
	}

	sprintf(log, "[%s][%s][%d]", FILE_LINE);
	for(i=0;i<lu32len;i++) {
		sprintf(tmplog, "%02X", pDataTemp[i]);
		strcat(log, " ");
		strcat(log, tmplog);
	}
	strcat(log, "\n");
	write_log_file(log, strlen(log));
	err = RS485Down_DataSend(pDataTemp, lu32len);
	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));

	switch(pmf->u8ProtocolType){
	case ELEC_LCMODBUS:
		err = ElecLcMod_ReceiveFrame(pmf,lu8DownComDev,lu16OutTime,pDataTemp,plen);
		sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
		write_log_file(log, strlen(log));
	break;
		default:
		break;
	}

	return err;
}

//��ȡ�����й����ܵ���ֵ
float get_lcMod_actE(float data)
{
	return (data*ELEC_LCMOD_DEF_V*ELEC_LCMOD_DEF_I)/18000000;
}

//��ȡ�����޹����ܵ���ֵ
float get_lcMod_reactE(float data)
{
	return (data*ELEC_LCMOD_DEF_V*ELEC_LCMOD_DEF_I)/10800000;
}

uint8 ElecMeter_DataDeal(MeterFileType *pmf,uint8 *pDataBuf,uint16 *pLen, lcModbusElec_str *pData)
{
	uint8 err = NO_ERR;
	uint8 len = 0;
	char log[1024];
	if(pData == NULL)
		return ERR_1;

	switch(pmf->u8ProtocolType){
	case ELEC_LCMODBUS:
		if(*pLen<(LC_ELEC_WORK_LEN*2+4)) {
			sprintf(log, "[%s][%s][%d]pLen error: %d\n", FILE_LINE, *pLen);
			return ERR_1;
		}
		pDataBuf += 2;//�Թ���ַ��͹�������
		len = *pDataBuf++;
		len = len;

		pData->pact_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
		pData->pact_tot_elec = get_lcMod_actE(pData->pact_tot_elec);
		pDataBuf += 4;
		strcpy((char*)pData->pact_tot_elec_unit, "+KWh");

		pData->nact_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
		pDataBuf += 4;
		pData->nact_tot_elec = get_lcMod_actE(pData->nact_tot_elec);
		strcpy((char*)pData->nact_tot_elec_unit, "-KWh");

		pData->preact_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
		pDataBuf += 4;
		pData->preact_tot_elec = get_lcMod_reactE(pData->preact_tot_elec);
		strcpy((char*)pData->preact_tot_elec_unit, "+KVarh");

		pData->nreact_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
		pDataBuf += 4;
		pData->nreact_tot_elec = get_lcMod_reactE(pData->nreact_tot_elec);
		strcpy((char*)pData->nreact_tot_elec_unit, "-KVarh");

		pData->act_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
		pDataBuf += 4;
		pData->act_tot_elec = get_lcMod_actE(pData->act_tot_elec);
		strcpy((char*)pData->act_tot_elec_unit, "KWh");

		pData->react_tot_elec = (pDataBuf[0]<<3*8 | pDataBuf[1]<<2*8 | pDataBuf[2]<<8 | pDataBuf[3]);
		pData->react_tot_elec = get_lcMod_reactE(pData->react_tot_elec);
		strcpy((char*)pData->react_tot_elec_unit, "KVarh");
		break;
	default:
		break;
	}

	return err;
}

uint8 ElecMeterCommunicate(MeterFileType *pmf,lcModbusElec_str *pData)
{
	uint8 lu8DataTemp[256] = {0};  //����֡
	uint16 lu16len = 0;//����֡�ĳ���
	uint8 err = 0;

	lu16len = sizeof(lu8DataTemp);
	switch (pmf->u8ProtocolType) {
	case ELEC_LCMODBUS:
		CreateFrame_lcMod(pmf,lu8DataTemp,&lu16len);  //��������֡��
		break;
	default:
		break;
	}
	err = ElecMeter_DataSendAndRec(pmf,lu8DataTemp,&lu16len);

	if(err == NO_ERR) {
		err = ElecMeter_DataDeal(pmf,lu8DataTemp,&lu16len, pData);//�������յ�������
	}
	return err;
}

uint8 Read_ElecMeter(MeterFileType *pmf, lcModbusElec_str* pElecData)
{
	uint8 err = NO_ERR;
	uint8 lu8retrytimes = 0;
	char log[1024];
	if(pmf->u8ProtocolType >= ELECMETER_PROTO_SUM){  //��ֹЭ��汾�ų��ޡ�
		return ERR_1;
	}

	//����Э��汾�ţ����ö�Ӧ�˿ڲ���.
	lu8retrytimes = 3;
	do{
		err = (*ELEC_METER_ComParaSetArray[gELEC_METER_Table[pmf->u8ProtocolType][0]])();  //���ճ���Э�飬���ó����ڲ�����
		lu8retrytimes--;
	}while((err != TRUE) && (lu8retrytimes > 0));
	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));
	//�齨��������֡
	err = ElecMeterCommunicate(pmf, pElecData);
	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));
	return err;
}
