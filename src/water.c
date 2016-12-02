#include <includes.h>


/*****************************************************************
 * set com parameters table
 ****************************************************************/
static uint8 (*WATER_METER_ComParaSetArray[])(void) = {METER_ComSet3};

/*****************************************************************
 *  Table 格式为：串口设置函数指针标号 + 数据标识符 +
 *  前导符个数+通讯类型(0:MBUS  1:485)+通讯协议类型(0:MODBUS  1:645)
 ****************************************************************/
static uint16 gWATER_METER_Table[][5] = {
	{WATER_COMSET_1, 0x1F90, 4, WATER_MBUS, WATER_PROTO_CJ188}//兴源水表		-----idx: -0-
};



void waterCJ188Frame(MeterFileType *pmf, uint8 *pDataTemp, uint16 *plen)
{
	waterCJ188Str	ProtocoalInfo;
	uint8 *pTemp = pDataTemp;

	ProtocoalInfo.PreSymbolNum  = gWATER_METER_Table[pmf->u8ProtocolType][em_water_table_prefixCnt];
	ProtocoalInfo.startCode     = CJ188_FRAME_START;
	ProtocoalInfo.MeterType = pmf->u8MeterType;
	memcpy(ProtocoalInfo.MeterAddr,  pmf->u8MeterAddr, 7);
	ProtocoalInfo.ControlCode 	= 0x01;
	ProtocoalInfo.Length		= 0x03;
	ProtocoalInfo.DataIdentifier= gWATER_METER_Table[pmf->u8ProtocolType][em_water_table_dataID];
	ProtocoalInfo.SER = 0;
	ProtocoalInfo.chkSum = PUBLIC_CountCS((uint8*)&(ProtocoalInfo.startCode), \
		(&ProtocoalInfo.SER - &ProtocoalInfo.PreSymbolNum));//计算 校验字节
	ProtocoalInfo.endCode = CJ188_FRAME_END;

	memset(pTemp, 0xFE, ProtocoalInfo.PreSymbolNum);		//填入前导字符0xFE, 个数视情况而定
	pTemp += ProtocoalInfo.PreSymbolNum;
	memcpy(pTemp, (uint8*)&(ProtocoalInfo.startCode), \
		(&ProtocoalInfo.endCode - &ProtocoalInfo.PreSymbolNum));
	pTemp += sizeof(waterCJ188Str);
	pTemp++;
	*pTemp++ = CJ188_FRAME_END;	//联强485总线1200bps调,最后一字节总是不对，组帧多放一字节。

	*plen = (pTemp - pDataTemp);//返回数据帧长度
}


uint8 createSendCmdXYuan(MeterFileType *pmf, uint8* buf, uint16* len)
{
	waterCJ188Frame(pmf, buf, len);
	return NO_ERR;
}


uint8 waterReceiveFrame(MeterFileType *pmf, uint8 dev, uint16 Out_Time,\
								uint8 *buf,uint16 *datalen)
{
	uint8 data 	= 0x00;
	uint8* pBuf   = buf;
	uint32 i = 500;
	uint8 datafield_len = 0;//数据域的长度
	uint8 data_idx = 0;//数据域的索引
	uint8 addrLen = 0;
	
	*datalen = 0;
	waterCJ188St stat = em_cj188_init_state;
	while(i--){
		if(DownDevGetch(dev, &data, Out_Time))
			return ERR_1;

		switch(stat){
		case em_cj188_init_state://find head of buf
			if(CJ188_FRAME_START == data) {
				*pBuf++ = data;
				stat = em_cj188_startCode_state;
			}
			break;
		case em_cj188_startCode_state:
			if(WATERMETER == data) {
				*pBuf++ = data;
				stat = em_cj188_meterType_state;
			} else {
				pBuf = buf;
				stat = em_cj188_init_state;
			}
			break;
		case em_cj188_meterType_state:
			*pBuf++ = data;
			addrLen++;
			stat = em_cj188_address_state;
			break;
		case em_cj188_address_state:
			*pBuf++ = data;
			addrLen++;
			if(addrLen > LENGTH_B_METER_ADDRESS)
				stat = em_cj188_control_state;
			break;
		case em_cj188_control_state:
			*pBuf++ = data;
			datafield_len = data;
			stat = em_cj188_length_state;
			break;
		case em_cj188_length_state:
			*pBuf++ = data;
			data_idx++;
			stat = em_cj188_data_state;
			break;
		case em_cj188_data_state:
			*pBuf++ = data;
			data_idx++;
			if (data_idx > datafield_len) {
				if(PUBLIC_CountCS(buf, pBuf-buf) == data) {
					stat = em_cj188_chksum_state;
				}
				else {
					pBuf = buf;
					stat = em_cj188_init_state;
				}
			}
			break;
		case em_cj188_chksum_state:
			*pBuf++ = data;
			if (CJ188_FRAME_END == data)
				stat = em_cj188_endCode_state;

			goto end;
			break;
		default:
			break;
		}
	}
end:
	if(em_cj188_endCode_state == stat) {//em_cj188_endCode_state是唯一的接受状态
		*datalen = (pBuf - buf + 1);
		return NO_ERR;
	}
	else {//停在其他状态, 则出错
		memset(pBuf, 0, (pBuf - buf + 1));
		return ERR_1;
	}
}


uint8 waterMeter_DataSendAndRec(MeterFileType *pmf, uint8* buf, uint16* plen)
{
	uint8 err = 0;
	uint32 lu32len = *plen;
	uint16 lu16OutTime = 2000;//毫秒。
	uint8 lu8DownComDev;
	char log[4096];

	if(pmf->u8Channel == RS485_DOWN_CHANNEL) {
		lu8DownComDev = DOWN_COMMU_DEV_485;
	} else {
		lu8DownComDev = DOWN_COMMU_DEV_MBUS;
	}

	switch(gWATER_METER_Table[pmf->u8ProtocolType][em_water_table_busType]) {
	case WATER_MBUS:
		err = MBUS_DataSend(buf, lu32len);
		break;
	case WATER_RS485:
		err = RS485Down_DataSend(buf, lu32len);
		break;
	default:
		break;
	}

	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));

	switch(gWATER_METER_Table[pmf->u8ProtocolType][em_water_table_protoType]) {
	case WATER_PROTO_MODBUS:
		break;
	case WATER_PROTO_CJ188:
		err = waterReceiveFrame(pmf, lu8DownComDev, lu16OutTime, buf, plen);
		break;
	default:
		break;
	}

	return NO_ERR;
}

uint8 waterDataDealXYuan(uint8* pData, uint16* pLen, waterDataPtr pWaterData)
{
	xingYuanStr dataStr;

	pData += 14;
	memcpy((uint8*)&(dataStr.accumFlow), pData, sizeof(xingYuanStr));

	memset((uint8*)&(pWaterData->accumFlow), 0, sizeof(waterDataStr));
	pWaterData->accumFlow = dataStr.accumFlow;
	pWaterData->accumFlowUnit = dataStr.accumFlowUnit;
	return NO_ERR;
}

uint8 waterMeter_DataDeal(MeterFileType *pmf, uint8* pData, uint16* pLen, waterDataPtr pWaterData)
{
	uint8 err = NO_ERR;

	if(NULL == pWaterData || \
		NULL == pmf|| \
		NULL == pData|| \
		NULL == pLen)
		return ERR_1;

	switch(pmf->u8ProtocolType){
	case em_water_xingYuan:
		err = waterDataDealXYuan(pData, pLen, pWaterData);
		break;
	default:
		err = ERR_1;
		break;
	}
	*pLen = sizeof(waterDataStr);
	return err;
}

uint8 waterMeterCommunicate(MeterFileType *pmf, waterDataStr *pData)
{
	uint8 lu8DataTemp[256] = {0};  //抄表帧
	uint16 lu16len = 0;//抄表帧的长度
	uint8 err = 0;

	lu16len = sizeof(lu8DataTemp);
	switch (pmf->u8ProtocolType) {
	case em_water_xingYuan:
		err = createSendCmdXYuan(pmf,lu8DataTemp,&lu16len);
		break;
	default:
		err = ERR_1;
		break;
	}
	if(ERR_1 == err)
		return ERR_1;
	err = waterMeter_DataSendAndRec(pmf, lu8DataTemp, &lu16len);

	if(NO_ERR ==err)
		err = waterMeter_DataDeal(pmf,lu8DataTemp,&lu16len, pData);

	return err;
}


uint8 ReadWaterMeter(MeterFileType *pmf, waterDataPtr pWaterData)
{
	uint8 err = NO_ERR;
	uint8 lu8retrytimes = 0;
	char log[1024];
	if(pmf->u8ProtocolType >= WATER_PROTO_SUM){  //防止协议版本号超限。
		return ERR_1;
	}

	//根据协议版本号，设置对应端口参数.
	lu8retrytimes = 3;
	do{
		err = (*WATER_METER_ComParaSetArray[gWATER_METER_Table[pmf->u8ProtocolType][0]])();  //按照抄表协议，设置抄表串口参数。
		lu8retrytimes--;
	}while((err != TRUE) && (lu8retrytimes > 0));
	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));
	//组建抄表数据帧
	err = waterMeterCommunicate(pmf, pWaterData);
	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));
	return err;
}

