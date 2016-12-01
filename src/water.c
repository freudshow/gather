#include <includes.h>


/*****************************************************************
 * set com parameters table
 ****************************************************************/
static uint8 (*WATER_METER_ComParaSetArray[])(void) = {METER_ComSet3};

/*****************************************************************
 *  Table 格式为：串口设置函数指针标号 + 数据标识符 +
 *  前导符个数+通讯类型(0:MBUS  1:485)+通讯协议类型(0:MODBUS  1:645)
 ****************************************************************/
static uint16 gWATER_METER_Table[ELECMETER_PROTO_SUM][5] = {
	{WATER_COMSET_1, 0x1F90, 4, WATER_MBUS, WATER_PROTO_CJ188}//兴源水表		-----idx: -0-
};









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

