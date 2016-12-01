#include <includes.h>


/*****************************************************************
 * set com parameters table
 ****************************************************************/
static uint8 (*WATER_METER_ComParaSetArray[])(void) = {METER_ComSet3};

/*****************************************************************
 *  Table ��ʽΪ���������ú���ָ���� + ���ݱ�ʶ�� +
 *  ǰ��������+ͨѶ����(0:MBUS  1:485)+ͨѶЭ������(0:MODBUS  1:645)
 ****************************************************************/
static uint16 gWATER_METER_Table[ELECMETER_PROTO_SUM][5] = {
	{WATER_COMSET_1, 0x1F90, 4, WATER_MBUS, WATER_PROTO_CJ188}//��Դˮ��		-----idx: -0-
};









uint8 ReadWaterMeter(MeterFileType *pmf, waterDataPtr pWaterData)
{
	uint8 err = NO_ERR;
	uint8 lu8retrytimes = 0;
	char log[1024];
	if(pmf->u8ProtocolType >= WATER_PROTO_SUM){  //��ֹЭ��汾�ų��ޡ�
		return ERR_1;
	}

	//����Э��汾�ţ����ö�Ӧ�˿ڲ���.
	lu8retrytimes = 3;
	do{
		err = (*WATER_METER_ComParaSetArray[gWATER_METER_Table[pmf->u8ProtocolType][0]])();  //���ճ���Э�飬���ó����ڲ�����
		lu8retrytimes--;
	}while((err != TRUE) && (lu8retrytimes > 0));
	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));
	//�齨��������֡
	err = waterMeterCommunicate(pmf, pWaterData);
	sprintf(log, "[%s][%s][%d]err: %d\n", FILE_LINE, err);
	write_log_file(log, strlen(log));
	return err;
}

