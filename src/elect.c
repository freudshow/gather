#include "globaldefine.h"
#include "bsp.h"
#include "readallmeters.h"
#include "commap.h"
#include "elect.h"

/*****************************************************************
 * set com parameters table
 ****************************************************************/
uint8 (*ELEC_METER_ComParaSetArray[])(void) = {METER_ComSet8};

/*****************************************************************
 *  Table ��ʽΪ���������ú���ָ���� + ��ȡ������ݱ�ʶ�� +
 *  ǰ��������+ͨѶ����(0:MBUS  1:485)+ͨѶЭ������(0:MODBUS  1:645)
 ****************************************************************/
uint16 gELEC_METER_Table[ELECMETER_PROTO_SUM][5] = {
	{ELEC_COMSET_1, 0x0000, 0, ELEC_RS485, ELEC_PROTO_MODBUS},//�������				-----idx: -0-
	{ELEC_COMSET_1, 0x0000, 0, ELEC_RS485, ELEC_PROTO_MODBUS},//acrel DDSD1352������	-----idx: -1-
	{ELEC_COMSET_1, 0x0000, 0, ELEC_RS485, ELEC_PROTO_MODBUS},//acrel DTSF1352������	-----idx: -2-
	{ELEC_COMSET_1, 0x0000, 0, ELEC_RS485, ELEC_PROTO_MODBUS},//acrel PZ80-E4C������	-----idx: -3-
};

uint8 createModCmd(MeterFileType *pmf, uint8 *pDataTemp, uint16 *plen, uint16 paraStart, uint16 regCount)
{
	modSendStr cmd;

	if(NULL == pmf || NULL == pDataTemp || NULL == plen)
		return ERR_1;

	cmd.address = pmf->u8MeterAddr[0];//����modbos�ı��ַ����Ϊ1�ֽ�
	cmd.func	=  MODBUS_READ;
	if(ERR_1 == inverseInt16(paraStart, &(cmd.start)))
		return ERR_1;
	if(ERR_1 == inverseInt16(regCount, &(cmd.regCount)))
		return ERR_1;

	cmd.crcSum = crc16ModRtu(((uint8*)&(cmd.address)), \
							 ((uint8*)(&cmd.crcSum) - (uint8*)(&cmd.address)));
	*plen = sizeof(modSendStr);
	memcpy(pDataTemp, ((uint8*)&(cmd.address)), *plen);

	return NO_ERR;
}

uint8 createSendCmdLc(MeterFileType *pmf, uint8 *pDataTemp, uint16 *plen)
{
	return createModCmd(pmf, pDataTemp, plen, LC_ELEC_WORK_START, LC_ELEC_WORK_LEN);
}

uint8 createSendCmdAcrelDDSD1352(MeterFileType *pmf, uint8 *pDataTemp, uint16 *plen)
{
	return createModCmd(pmf, pDataTemp, plen, ACREL_DDSD1352_PARA_START, ACREL_DDSD1352_PARA_LENGTH);
}

uint8 createSendCmdAcrelDTSF1352(MeterFileType *pmf, uint8 *pDataTemp, uint16 *plen)
{
	return createModCmd(pmf, pDataTemp, plen, ACREL_DTSF1352_PARA_START, ACREL_DTSF1352_PARA_LENGTH);
}

uint8 createSendCmdAcrelPZ80_E4C(MeterFileType *pmf, uint8 *pDataTemp, uint16 *plen)
{
	return createModCmd(pmf, pDataTemp, plen, ACREL_PZ80_E4C_PARA_START, ACREL_PZ80_E4C_PARA_LENGTH);
}

uint8 elecModReceiveFrame(MeterFileType *pmf, uint8 dev,uint16 Out_Time,uint8 *buf,uint16 *datalen)
{
	uint8 data 	= 0x00;
	uint8 len	= 0x00;
	uint8* pBuf   = buf;
	uint32 i = 500;
	uint8 datafield_len = 0;//������ĳ���
	uint8 data_idx = 0;//�����������
	*datalen = 0;

	lcElecAnsSt stat = em_init_state;
	data_idx = 0;
	while(i--){
		if(DownDevGetch(dev, &data, Out_Time))
			return ERR_1;

		switch(stat){
		case em_init_state://find head of buf
			if((uint8)data == (uint8)pmf->u8MeterAddr[0]) {
				len++;
				*pBuf++ = data;
				stat = em_address_state;
			}
			break;
		case em_address_state:
			if(data == MODBUS_READ) {
				len++;
				*pBuf++ = data;
				stat = em_func_state;
			}
			else//if func code not follow address, then not find head of buf
				stat = em_init_state;
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
			stat = em_end_state;
			goto end;
		default:
			break;
		}
	}
end:
	if(em_end_state == stat) {
		*datalen = len;
		return NO_ERR;
	}
	else {
		memset(pBuf, 0, len);
		*datalen = 0;
		return ERR_1;
	}
}

uint8 ElecMeter_DataSendAndRec(MeterFileType *pmf,uint8 *pDataTemp,uint16 *plen)
{
	uint8 err = 0;
	uint32 lu32len = *plen;
	uint16 lu16OutTime = 2000;//���롣
	uint8 lu8DownComDev;
	char log[4096];

	if(pmf->u8Channel == RS485_DOWN_CHANNEL) {
		lu8DownComDev = DOWN_COMMU_DEV_485;
	} else {
		lu8DownComDev = DOWN_COMMU_DEV_MBUS;
	}

	switch(gELEC_METER_Table[pmf->u8ProtocolType][em_elec_table_busType]) {
	case ELEC_MBUS:
		err = MBUS_DataSend(pDataTemp, lu32len);
		break;
	case ELEC_RS485:
		err = RS485Down_DataSend(pDataTemp, lu32len);
		break;
	default:
		break;
	}

	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));

	switch(gELEC_METER_Table[pmf->u8ProtocolType][em_elec_table_protoType]) {
	case ELEC_PROTO_MODBUS:
		err = elecModReceiveFrame(pmf, lu8DownComDev, \
								  lu16OutTime, pDataTemp, plen);
		break;
	case ELEC_PROTO_645:
		break;
	default:
		break;
	}

	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));

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

uint8 elecLcModDataDeal(uint8 *pDataBuf,uint16 *pLen, elecMeterDataPtr pData)
{
	if(NULL == pData || NULL == pDataBuf|| NULL == pLen)
		return ERR_1;
	if(*pLen<(LC_ELEC_WORK_LEN*2+4)) {
		return ERR_1;
	}
	pDataBuf += 3;//ignore address, function and length code

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

	return NO_ERR;
}

uint8 elecAcrelDDSD1352DataDeal(uint8 *pDataBuf,uint16 *pLen, elecMeterDataPtr pData)
{
	acrelDDSD1352DataStr ddsd1352Str;

	if(NULL == pData || NULL == pDataBuf|| NULL == pLen)
		return ERR_1;
	if(*pLen != sizeof(acrelDDSD1352DataStr)) {
		return ERR_1;
	}
	memcpy(((uint8*)&(ddsd1352Str.address)), pDataBuf, *pLen);
	/* modbus transform big endian format, 
	 * so we need to convert them to little endian
	 */
	/* start inverse */
	if(ERR_1 == inverseInt32(ddsd1352Str.activeWork, \
		&(ddsd1352Str.activeWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.activePeakWork, \
							&(ddsd1352Str.activePeakWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.activeFlatWork, \
							&(ddsd1352Str.activeFlatWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.activeValleyWork, \
							&(ddsd1352Str.activeValleyWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.reverseActiveWork, \
							&(ddsd1352Str.reverseActiveWork)))
		return ERR_1;
	if(ERR_1 == inverseInt16(ddsd1352Str.passwd, \
							&(ddsd1352Str.passwd)))
		return ERR_1;
	if(ERR_1 == inverseInt16(ddsd1352Str.voltage, \
							&(ddsd1352Str.voltage)))
		return ERR_1;
	if(ERR_1 == inverseInt16(ddsd1352Str.current, \
							&(ddsd1352Str.current)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)ddsd1352Str.activePower, \
							(uint16*)&(ddsd1352Str.activePower)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)ddsd1352Str.reactivePower, \
							(uint16*)&(ddsd1352Str.reactivePower)))
		return ERR_1;
	if(ERR_1 == inverseInt16(ddsd1352Str.apparentPower, \
							&(ddsd1352Str.apparentPower)))
		return ERR_1;

	if(ERR_1 == inverseInt16((uint16)ddsd1352Str.powerFactor, \
							(uint16*)&(ddsd1352Str.powerFactor)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)ddsd1352Str.frequency, \
							(uint16*)&(ddsd1352Str.frequency)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last1ActiveWork, \
		&(ddsd1352Str.last1ActiveWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last1ActivePeakWork, \
		&(ddsd1352Str.last1ActivePeakWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last1ActiveFlatWork, \
		&(ddsd1352Str.last1ActiveFlatWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last1ActiveValleyWork, \
		&(ddsd1352Str.last1ActiveValleyWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last2ActiveWork, \
		&(ddsd1352Str.last2ActiveWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last2ActivePeakWork, \
		&(ddsd1352Str.last2ActivePeakWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last2ActiveFlatWork, \
		&(ddsd1352Str.last2ActiveFlatWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last2ActiveValleyWork, \
		&(ddsd1352Str.last2ActiveValleyWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last3ActiveWork, \
		&(ddsd1352Str.last3ActiveWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last3ActivePeakWork, \
		&(ddsd1352Str.last3ActivePeakWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last3ActiveFlatWork, \
		&(ddsd1352Str.last3ActiveFlatWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(ddsd1352Str.last3ActiveValleyWork, \
		&(ddsd1352Str.last3ActiveValleyWork)))
		return ERR_1;
	/* end inverse */

	/* start format */
	pData->pact_tot_elec = ((float)ddsd1352Str.activeWork)*0.01;
	strcpy((char*)pData->pact_tot_elec_unit, "+KWh");
	pData->nact_tot_elec = ((float)ddsd1352Str.reverseActiveWork)*0.01;
	strcpy((char*)pData->nact_tot_elec_unit, "-KWh");
	pData->preact_tot_elec = 0.0;
	strcpy((char*)pData->preact_tot_elec_unit, "+KVarh");
	pData->nreact_tot_elec = 0.0;
	strcpy((char*)pData->nreact_tot_elec_unit, "-KVarh");
	pData->act_tot_elec = ((float)ddsd1352Str.activeWork)*0.01;
	strcpy((char*)pData->act_tot_elec_unit, "KWh");
	pData->react_tot_elec = ((float)ddsd1352Str.reverseActiveWork)*0.01;
	strcpy((char*)pData->react_tot_elec_unit, "KVarh");
	/* end format */
	return NO_ERR;
}


uint8 elecAcrelDTSF1352DataDeal(uint8 *pDataBuf,uint16 *pLen, elecMeterDataPtr pData)
{
	acrelDTSF1352DataStr dtsf1352Str;

	if(NULL == pData || NULL == pDataBuf|| NULL == pLen)
		return ERR_1;
	if(*pLen != sizeof(acrelDTSF1352DataStr)) {
		return ERR_1;
	}
	memcpy(((uint8*)&(dtsf1352Str.address)), pDataBuf, *pLen);
	/* modbus transform big endian format, 
	 * so we need to convert them to little endian
	 */
	/* start inverse */
	if(ERR_1 == inverseInt32(dtsf1352Str.activeWork, \
		&(dtsf1352Str.activeWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(dtsf1352Str.activeSharpWork, \
							&(dtsf1352Str.activeSharpWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(dtsf1352Str.activePeakWork, \
							&(dtsf1352Str.activePeakWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(dtsf1352Str.activeFlatWork, \
							&(dtsf1352Str.activeFlatWork)))
		return ERR_1;
	if(ERR_1 == inverseInt32(dtsf1352Str.activeValleyWork, \
							&(dtsf1352Str.activeValleyWork)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.voltageA, \
							(uint16*)&(dtsf1352Str.voltageA)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.voltageB, \
							(uint16*)&(dtsf1352Str.voltageB)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.voltageC, \
							(uint16*)&(dtsf1352Str.voltageC)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.currentA, \
							(uint16*)&(dtsf1352Str.voltageA)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.currentB, \
							(uint16*)&(dtsf1352Str.currentB)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.currentC, \
							(uint16*)&(dtsf1352Str.currentC)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.voltageAB, \
							(uint16*)&(dtsf1352Str.voltageAB)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.voltageCB, \
							(uint16*)&(dtsf1352Str.voltageCB)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.voltageAC, \
							(uint16*)&(dtsf1352Str.voltageAC)))
		return ERR_1;	
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.voltageRatio, \
							(uint16*)&(dtsf1352Str.voltageRatio)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.currentRatio, \
							(uint16*)&(dtsf1352Str.currentRatio)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.pulseConst, \
							(uint16*)&(dtsf1352Str.pulseConst)))
		return ERR_1;
	if(ERR_1 == inverseInt16((uint16)dtsf1352Str.runState, \
							(uint16*)&(dtsf1352Str.runState)))
		return ERR_1;
	/* end inverse */

	/* start format */
	pData->pact_tot_elec = ((float)dtsf1352Str.activeWork)*0.01;
	strcpy((char*)pData->pact_tot_elec_unit, "+KWh");
	pData->nact_tot_elec = 0.0;
	strcpy((char*)pData->nact_tot_elec_unit, "-KWh");
	pData->preact_tot_elec = 0.0;
	strcpy((char*)pData->preact_tot_elec_unit, "+KVarh");
	pData->nreact_tot_elec = 0.0;
	strcpy((char*)pData->nreact_tot_elec_unit, "-KVarh");
	pData->act_tot_elec = ((float)dtsf1352Str.activeWork)*0.01;
	strcpy((char*)pData->act_tot_elec_unit, "KWh");
	pData->react_tot_elec = 0.0;
	strcpy((char*)pData->react_tot_elec_unit, "KVarh");
	/* end format */
	return NO_ERR;
}

uint8 elecAcrelPZ80_E4CDataDeal(uint8 *pDataBuf,uint16 *pLen, elecMeterDataPtr pData)
{
	acrelPZ80_E4CDataStr PZ80_E4CStr;

	if(NULL == pData || NULL == pDataBuf|| NULL == pLen)
		return ERR_1;


	
}

uint8 ElecMeter_DataDeal(MeterFileType *pmf,uint8 *pDataBuf,uint16 *pLen, elecMeterDataPtr pData)
{
	uint8 err = NO_ERR;

	if(NULL == pData || \
		NULL == pmf|| \
		NULL == pDataBuf|| \
		NULL == pLen)
		return ERR_1;

	switch(pmf->u8ProtocolType){
	case em_lcModbus:
		err = elecLcModDataDeal(pDataBuf, pLen, pData);
		break;
	case em_acrel_DDSD1352:
		err = elecAcrelDDSD1352DataDeal(pDataBuf, pLen, pData);
		break;
	case em_acrel_DTSF1352:
		err = elecAcrelDTSF1352DataDeal(pDataBuf, pLen, pData);
		break;
	case em_acrel_PZ80_E4C:
		err = elecAcrelPZ80_E4CDataDeal(pDataBuf, pLen, pData);
		break;
	default:
		break;
	}
	*pLen = sizeof(elecMeterDataStr);
	return err;
}

uint8 ElecMeterCommunicate(MeterFileType *pmf,elecMeterDataStr *pData)
{
	uint8 lu8DataTemp[256] = {0};  //����֡
	uint16 lu16len = 0;//����֡�ĳ���
	uint8 err = 0;

	lu16len = sizeof(lu8DataTemp);
	switch (pmf->u8ProtocolType) {
	case em_lcModbus:
		err = createSendCmdLc(pmf,lu8DataTemp,&lu16len);
		break;
	case em_acrel_DDSD1352:
		err = createSendCmdAcrelDDSD1352(pmf,lu8DataTemp,&lu16len);
		break;
	case em_acrel_DTSF1352:
		err = createSendCmdAcrelDTSF1352(pmf,lu8DataTemp,&lu16len);
		break;
	case em_acrel_PZ80_E4C:
		err = createSendCmdAcrelPZ80_E4C(pmf,lu8DataTemp,&lu16len);
		break;
	default:
		break;
	}
	if(ERR_1 == err)
		return ERR_1;
	err = ElecMeter_DataSendAndRec(pmf, lu8DataTemp, &lu16len);

	if(NO_ERR ==err)
		err = ElecMeter_DataDeal(pmf,lu8DataTemp,&lu16len, pData);

	return err;
}

uint8 Read_ElecMeter(MeterFileType *pmf, elecMeterDataStr* pElecData)
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
