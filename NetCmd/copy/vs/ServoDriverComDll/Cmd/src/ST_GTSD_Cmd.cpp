// CmdDll.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "BaseReturn_def.h"
#include "ST_GTSD_Cmd.h"
//////////////////////////////////////////////////////////////////////////
#ifdef CMD_TEST
	typedef struct testcmd_read_number_testA
	{
		int16 value;
		int16 flag;
	}TEST_cmd_read_number_testA;
	//ÿ�ζ��������ݸ���
	TEST_cmd_read_number_testA cmd_read_number_testA[1000];
	int16 cmd_read_number_test_indexA = 0;
	int16 cmd_read_number_testB[1000];
	int16 cmd_read_number_test_indexB = 0;
	//��¼���ݳ����ĸ���
	int64 cmd_exceed_numberA = 0;
	int64 cmd_exceed_numberB = 0;
	int16 plotdatabuffer_test[48000];
	int32 plotdatabuffer_test_index = 0;
#endif
//////////////////////////////////////////////////////////////////////////

const int32	 OUTPUT_LIMIT_SCALE		= 4096;
const int32	 MAX_CUR_SCALE			= 15000;
const int32  MAX_SPD_SCALE			= 16777216;   //2^24
const int32	 MAX_VOL_SCALE			= 16384;

int16 Cmd_PlotDataBuffer[10000] = { 0 };

CAbsCom*     g_AbsCom				= NULL;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//������ID����źϲ���һ��short������bit[0-11]Ϊ����ID, bit[12 - 15]Ϊ���
static int16 GetCmdIDAndAxisNum(short cmdID, short motorNum)
{
	short ret;
	ret = cmdID;
	ret += (motorNum << 12);
	return ret;
}
//////////////////////////////////////////////////////////////////////////
//������ͨ��
int16 GTSD_CMD_ST_OPEN(int16 comType)
{
	//�������
	if (g_AbsCom == NULL)
	{
		g_AbsCom = new CAbsCom;
	}
	else
	{ 
		 return Rt_Success;//����豸�Ѿ��򿪣�ֱ�ӷ���
	}
	if (g_AbsCom == NULL)
	{
		return Net_Rt_CreateObj_Err;
	}
	int16 rtn;
	//����open����
	rtn = g_AbsCom->GTSD_Com_Open(comType);
	return rtn;
}
//////////////////////////////////////////////////////////////////////////
//�ر�����ͨ��
int16 GTSD_CMD_ST_CLOSE(int16 comType)
{
	//�����Ѿ��ر��˾�ֱ���˳�
	if (g_AbsCom == NULL)
	{
		return Rt_Success;
	}

	int16 rtn;
	//����close����
	rtn = g_AbsCom->GTSD_Com_Close(comType);
	if (g_AbsCom != NULL)
	{
		delete g_AbsCom;
	}
	g_AbsCom = NULL;

	return rtn;
}
//////////////////////////////////////////////////////////////////////////
//���ŷ�
//���������
int16 GTSD_CMD_ST_SetServoOn(int16 axis, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = SERVO_EN_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;									//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = 1;												//�ŷ�ʹ��							

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//���ŷ�
//���������
int16 GTSD_CMD_ST_SetServoOff(int16 axis, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = SERVO_EN_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;									//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = 0;												//�ŷ�off							

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡ�ŷ�״̬
//���������
//�ŷ�״̬�ṹ��ָ��
int16 GTSD_CMD_ST_GetServoState(int16 axis, SERVO_STATE* serv, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = SERVO_EN_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;									//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);

	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		serv->act_on_tmp = dspdata[0];
		serv->act_on = dspdata[1];
		serv->sec_on = dspdata[2];
		serv->sof_st_on = dspdata[3];
		serv->serv_ready = dspdata[4];
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//�����ŷ�����ģʽ
//���������
//������ģʽ
int16 GTSD_CMD_ST_SetServoTaskMode(int16 axis, int16 mode, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = SERVO_TASK_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = mode;												//����ģʽ							

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡ�ŷ�����ģʽ
//���������
//������ģʽ
int16 GTSD_CMD_ST_GetServoTaskMode(int16 axis, SERVO_MODE* mode, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = SERVO_TASK_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		mode->usr_mode = dspdata[0];
		mode->cmd_id = dspdata[1];
		mode->curr_id = dspdata[2];
		mode->curr_state = dspdata[3];

		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//���û��ģʽ
//���������
int16 GTSD_CMD_ST_SetMixMode(int16 axis, int16 com_type)
{
	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
//��ȡ���ģʽ
//���������
int16 GTSD_CMD_ST_GetMixMode(int16 axis, int16 com_type)
{
	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
//����ƫ������һ��16bit��ֵ��������
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Write16BitByAdr(int16 axis, int16 ofst, int16 value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_16BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = ofst;												//����ofst
	dspdata[4] = value;											//����value


	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ�ôӴ�������ȡһ��16bit��ֵ
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Read16BitByAdr(int16 axis, int16 ofst, int16* value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_16BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		
	dspdata[3] = ofst;												//����ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		*value = dspdata[1];
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ������һ��32bit��ֵ��������
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Write32BitByAdr(int16 axis, int16 ofst, int32 value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_32BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = ofst;												//����ofst
	dspdata[4] = (value & 0xffff);									//����value,�����õ�16bit
	dspdata[5] = ((value >> 16) & 0xffff);							//����value,�����ø�16bit

	int16 dsp_comNum = 6;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ�ôӴ�������ȡһ��32bit��ֵ
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Read32BitByAdr(int16 axis, int16 ofst, int32* value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_32BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		
	dspdata[3] = ofst;												//����ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		*value = (((dspdata[1]) & 0x0000ffff) | ((dspdata[2] << 16) & 0xffff0000));								//��λ�ں�
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ������һ��64bit��ֵ��������
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Write64BitByAdr(int16 axis, int16 ofst, int64 value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_64BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = ofst;												//����ofst
	dspdata[4] = (value & 0xffff);									//����value,�����õ�16bit
	dspdata[5] = ((value >> 16) & 0xffff);							//����value,������  16bit
	dspdata[6] = ((value >> 32) & 0xffff);							//����value,������  16bit
	dspdata[7] = (((value >> 32) >> 16) & 0xffff);					//����value,�����ø�16bit

	int16 dsp_comNum = 8;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ�ôӴ�������ȡһ��64bit��ֵ
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Read64BitByAdr(int16 axis, int16 ofst, int64* value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_64BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		
	dspdata[3] = ofst;												//����ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		int64 tmp, tmp1;
		tmp = (((int64)dspdata[4]) << 32);
		tmp1 = ((tmp << 16) & 0xffff000000000000);
		*value = (((int64)(dspdata[1]) & 0x000000000000ffff) | ((((int64)dspdata[2]) << 16) & 0x00000000ffff0000) | ((((int64)dspdata[3]) << 32) & 0x0000ffff00000000) | tmp1);								//��λ�ں�
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����idָ��
//���������
//������idָ��ο�ֵ
int16 GTSD_CMD_ST_SetIdRef(int16 axis, double id_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = ID_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ	
	int32 tmp;
	tmp = (int32)((id_ref) / 100.0*((double)(MAX_CUR_SCALE)) + 0.5);
	if ((tmp>32767) || (tmp<-32768))
	{
		tmp = (int32)(0.05*((double)(MAX_CUR_SCALE)) + 0.5);
	}
	dspdata[3] = (int16)tmp;											//����id_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡidָ��
//���������
//������idָ��ο�ֵ
int16 GTSD_CMD_ST_GetIdRef(int16 axis, ID_STATE* id_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = ID_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		id_ref->chd_idr_tmp = (((double)dspdata[0]) / ((double)MAX_CUR_SCALE) * 100.0);
		id_ref->tsk_id_ref = (((double)dspdata[1]) / ((double)MAX_CUR_SCALE) * 100.0);
		id_ref->ctl_id_ref = (((double)dspdata[2]) / ((double)MAX_CUR_SCALE) * 100.0);
		id_ref->ctl_id = (((double)dspdata[3]) / ((double)MAX_CUR_SCALE) * 100.0);
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����iqָ��
//���������
//������idָ��ο�ֵ
int16 GTSD_CMD_ST_SetIqRef(int16 axis, double iq_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = IQ_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int32 tmp;
	tmp = (int32)((iq_ref) / 100.0*((double)(MAX_CUR_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < -32768))
	{
		tmp = (int32)(0.05*((double)(MAX_CUR_SCALE)) + 0.5);
	}
	dspdata[3] = (int16)tmp;										//����iq_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡiqָ��
//���������
//������iqָ��ο�ֵ
int16 GTSD_CMD_ST_GetIqRef(int16 axis, IQ_STATE* iq_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = IQ_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		iq_ref->chd_iqr_tmp = (((double)dspdata[0]) / ((double)MAX_CUR_SCALE) * 100.0);
		iq_ref->tsk_iq_ref = (((double)dspdata[1]) / ((double)MAX_CUR_SCALE) * 100.0);
		iq_ref->ctl_iq_ref = (((double)dspdata[2]) / ((double)MAX_CUR_SCALE) * 100.0);
		iq_ref->ctl_iq = (((double)dspdata[3]) / ((double)MAX_CUR_SCALE) * 100.0);
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//�����ٶ�ָ��
//���������
//�������ٶ�ָ��ο�ֵ����
int16 GTSD_CMD_ST_SetSpdRef(int16 axis, double spd_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = SPD_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int64 tmp;
	tmp = (int64)(spd_ref / 100.0*((double)MAX_SPD_SCALE) + 0.5);
	if ((tmp>0x7fffffff) || (tmp<-0x7fffffff))
	{
		tmp = (int64)(0.05*((double)MAX_SPD_SCALE) + 0.5);
	}
	int32 tmp1;
	tmp1 = (int32)tmp;
	dspdata[3] = (tmp1 & 0xffff);									//����spd_ref��16λ
	dspdata[4] = ((tmp1 >> 16) & 0xffff);								//����spd_ref��16λ

	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡ�ٶ�ָ��
//���������
//�������ٶ�ָ��ο�ֵ����
int16 GTSD_CMD_ST_GetSpdRef(int16 axis, SPD_STATE* spd_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = SPD_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		spd_ref->chd_spd_tmp = ((double)(((dspdata[0] & 0x0000ffff) | ((dspdata[1] << 16) & 0xffff0000)))) / ((double)MAX_SPD_SCALE)*100.0;
		spd_ref->tsk_spd_ref = ((double)(((dspdata[2] & 0x0000ffff) | ((dspdata[3] << 16) & 0xffff0000)))) / ((double)MAX_SPD_SCALE)*100.0;
		spd_ref->ctl_spd_ref = ((double)(((dspdata[4] & 0x0000ffff) | ((dspdata[5] << 16) & 0xffff0000)))) / ((double)MAX_SPD_SCALE)*100.0;
		spd_ref->ctl_spd_fd = ((double)(((dspdata[6] & 0x0000ffff) | ((dspdata[7] << 16) & 0xffff0000)))) / ((double)MAX_SPD_SCALE)*100.0;
		spd_ref->rsv_mot_spd = ((double)(((dspdata[8] & 0x0000ffff) | ((dspdata[9] << 16) & 0xffff0000)))) / ((double)MAX_SPD_SCALE)*100.0;
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ѹ��ָ��
//���������
//������ѹ��ָ��ο�ֵ
int16 GTSD_CMD_ST_SetPreRef(int16 axis, int16 pre_ref, int16 com_type)
{
	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
//��ȡѹ��ָ��
//���������
//������ѹ��ָ��ο�ֵ
int16 GTSD_CMD_ST_GetPreRef(int16 axis, int16* pre_ref, int16 com_type)
{
	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
//����udrefָ��
//���������
//������udָ��ο�ֵ
int16 GTSD_CMD_ST_SetUdRef(int16 axis, double ud_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UD_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int32 tmp;
	tmp = (int32)(ud_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp>16384) || (tmp<-16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;											//����ud_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡudrefָ��
//���������
//������udָ��ο�ֵ
int16 GTSD_CMD_ST_GetUdRef(int16 axis, UD_STATE* ud_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UD_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		ud_ref->chd_ud_tmp = ((double)(dspdata[0])) / ((double)MAX_VOL_SCALE)*100.0;
		ud_ref->tsk_ud_ref = ((double)(dspdata[1])) / ((double)MAX_VOL_SCALE)*100.0;
		ud_ref->cur_ud = ((double)(dspdata[2])) / ((double)MAX_VOL_SCALE)*100.0;

		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����uqrefָ��
//���������
//������uqָ��ο�ֵ
int16 GTSD_CMD_ST_SetUqRef(int16 axis, double uq_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UQ_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int32 tmp;
	tmp = (int32)(uq_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;											//����uq_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡuqrefָ��
//���������
//������uqָ��ο�ֵ
int16 GTSD_CMD_ST_GetUqRef(int16 axis, UQ_STATE* uq_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UQ_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		uq_ref->chd_uq_tmp = ((double)(dspdata[0])) / ((double)MAX_VOL_SCALE)*100.0;
		uq_ref->tsk_uq_ref = ((double)(dspdata[1])) / ((double)MAX_VOL_SCALE)*100.0;
		uq_ref->cur_uq = ((double)(dspdata[2])) / ((double)MAX_VOL_SCALE)*100.0;

		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����uarefָ��
//���������
//������uaָ��ο�ֵ
int16 GTSD_CMD_ST_SetUaRef(int16 axis, double ua_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UA_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int32 tmp;
	tmp = (int32)(ua_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;											//����ua_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡuarefָ��
//���������
//������uaָ��ο�ֵ
int16 GTSD_CMD_ST_GetUaRef(int16 axis, UA_STATE* ua_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UA_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		ua_ref->chd_ua_tmp = ((double)(dspdata[0])) / ((double)MAX_VOL_SCALE)*100.0;
		ua_ref->tsk_ua_ref = ((double)(dspdata[1])) / ((double)MAX_VOL_SCALE)*100.0;
		ua_ref->cur_ua = ((double)(dspdata[2])) / ((double)MAX_VOL_SCALE)*100.0;

		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ubrefָ��
//���������
//������ubָ��ο�ֵ
int16 GTSD_CMD_ST_SetUbRef(int16 axis, double ub_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UB_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ	
	int32 tmp;
	tmp = (int32)(ub_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;										//����ub_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡubrefָ��
//���������
//������ubָ��ο�ֵ
int16 GTSD_CMD_ST_GetUbRef(int16 axis, UB_STATE* ub_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UB_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		ub_ref->chd_ub_tmp = ((double)(dspdata[0])) / ((double)MAX_VOL_SCALE)*100.0;
		ub_ref->tsk_ub_ref = ((double)(dspdata[1])) / ((double)MAX_VOL_SCALE)*100.0;
		ub_ref->cur_ub = ((double)(dspdata[2])) / ((double)MAX_VOL_SCALE)*100.0;

		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ucrefָ��
//���������
//������ucָ��ο�ֵ
int16 GTSD_CMD_ST_SetUcRef(int16 axis, double uc_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UC_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;									//д����	
	dspdata[2] = 0;												//����ֵ
	int32 tmp;
	tmp = (int32)(uc_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;											//����uc_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡucrefָ��
//���������
//������ucָ��ο�ֵ
int16 GTSD_CMD_ST_GetUcRef(int16 axis, UC_STATE* uc_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = UC_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		uc_ref->chd_uc_tmp = ((double)(dspdata[0])) / ((double)MAX_VOL_SCALE)*100.0;
		uc_ref->tsk_uc_ref = ((double)(dspdata[1])) / ((double)MAX_VOL_SCALE)*100.0;
		uc_ref->cur_uc = ((double)(dspdata[2])) / ((double)MAX_VOL_SCALE)*100.0;

		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����PosAdjrefָ��
//���������
//������PosAdjָ��ο�ֵ
int16 GTSD_CMD_ST_SetPosAdjRef(int16 axis, double PosAdj_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = POS_ADJ_IREF_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int32 tmp;
	tmp = (int32)(PosAdj_ref / 100.0*((double)MAX_CUR_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_CUR_SCALE));
	}
	dspdata[3] = (int16)tmp;										//����PosAdj_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡPosAdjrefָ��
//���������
//������PosAdjָ��ο�ֵ
int16 GTSD_CMD_ST_GetPosAdjRef(int16 axis, POS_ADJ_STATE* pos_adj_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = POS_ADJ_IREF_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		pos_adj_ref->chd_pos_adj_tmp = ((double)(dspdata[0])) / ((double)MAX_CUR_SCALE)*100.0;
		pos_adj_ref->tsk_pos_adj_ref = ((double)(dspdata[1])) / ((double)MAX_CUR_SCALE)*100.0;
		pos_adj_ref->ctl_id_ref = ((double)(dspdata[2])) / ((double)MAX_CUR_SCALE)*100.0;
		pos_adj_ref->ctl_id = ((double)(dspdata[3])) / ((double)MAX_CUR_SCALE)*100.0;
		pos_adj_ref->ctl_iq_ref = ((double)(dspdata[4])) / ((double)MAX_CUR_SCALE)*100.0;
		pos_adj_ref->ctl_iq = ((double)(dspdata[5])) / ((double)MAX_CUR_SCALE)*100.0;
		pos_adj_ref->rsv_pos_elec = (dspdata[6]);
		pos_adj_ref->rsv_pos = ((dspdata[7] & 0x0000ffff) | ((dspdata[8] << 16) & 0xffff0000));
		pos_adj_ref->rsv_pos_in = ((dspdata[9] & 0x0000ffff) | ((dspdata[10] << 16) & 0xffff0000));
		pos_adj_ref->mfj_pos_adj_flag = (dspdata[11]);
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����Posrefָ��
//���������
//������Posָ��ο�ֵ
int16 GTSD_CMD_ST_SetPosRef(int16 axis, int32 Pos_ref, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = POS_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (Pos_ref & 0xffff);									//����Pos_ref ��16bit
	dspdata[4] = ((Pos_ref >> 16) & 0xffff);							//����Pos_ref ��16bit

	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
/*
//////////////////////////////////////////////////////////////////////////
//����FndCurr
//���������
//������FndCurrֵ
int16 GTSD_CMD_ST_SetFndCurr(int16 axis, int16 fnd, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = FND_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (fnd);											//����fnd

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡFndCurr
//���������
//������FndCurr
int16 GTSD_CMD_ST_GetFndCurr(int16 axis, int16* fnd, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = FND_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*fnd) = (dspdata[0]);

		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����FnqCurr
//���������
//������FnqCurrֵ
int16 GTSD_CMD_ST_SetFnqCurr(int16 axis, int16 fnq, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = FNQ_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (fnq);											//����fnq

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡFnqCurr
//���������
//������FnqCurr
int16 GTSD_CMD_ST_GetFnqCurr(int16 axis, int16* fnq, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = FNQ_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*fnq) = (dspdata[0]);
		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����TidCurr
//���������
//������TidCurrֵ
int16 GTSD_CMD_ST_SetTidCurr(int16 axis, int32 tid, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = TID_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (tid & 0xffff);										//����tid��16bit
	dspdata[4] = ((tid >> 16) & 0xffff);								//����tid��16bit

	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡTidCurr
//���������
//������TidCurr
int16 GTSD_CMD_ST_GetTidCurr(int16 axis, int32* tid, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = TID_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*tid) = ((dspdata[0] & 0x0000ffff) | ((dspdata[1] << 16) & 0xffff0000));

		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����TiqCurr
//���������
//������TiqCurrֵ
int16 GTSD_CMD_ST_SetTiqCurr(int16 axis, int32 tiq, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = TIQ_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (tiq & 0xffff);										//����tiq��16bit
	dspdata[4] = ((tiq >> 16) & 0xffff);								//����tiq��16bit

	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡTiqCurr
//���������
//������TiqCurr
int16 GTSD_CMD_ST_GetTiqCurr(int16 axis, int32* tiq, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = TIQ_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*tiq) = ((dspdata[0] & 0x0000ffff) | ((dspdata[1] << 16) & 0xffff0000));
		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����Fnvel
//���������
//������Fnvelֵ
int16 GTSD_CMD_ST_SetFnVel(int16 axis, int16 fnvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = FN_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (fnvel);											//����fnvel

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡFnvel
//���������
//������Fnvel
int16 GTSD_CMD_ST_GetFnVel(int16 axis, int16* fnvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = FN_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*fnvel) = (dspdata[0]);
		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����Tivel
//���������
//������Tivelֵ
int16 GTSD_CMD_ST_SetTiVel(int16 axis, double tivel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = TI_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (int16)(((tivel)*100.0) + 0.5);							//����tivel,����ֵΪ0.01ms---1

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡTivel
//���������
//������Tivel
int16 GTSD_CMD_ST_GetTiVel(int16 axis, double* tivel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = TI_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*tivel) = ((double)(dspdata[0])) / 100.0;
		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����abslimvel
//���������
//������abslimvelֵ
int16 GTSD_CMD_ST_SetAbsLimVel(int16 axis, int16 abslimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = ABS_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int16 tmp;
	tmp = (int16)(abslimvel / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp>32767) || (tmp<0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;										//����abslimvel

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡabslimvel
//���������
//������abslimvel
int16 GTSD_CMD_ST_GetAbsLimVel(int16 axis, int16* abslimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = ABS_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*abslimvel) = (int16)(((double)dspdata[0]) / ((double)OUTPUT_LIMIT_SCALE)*100.0);
		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����poslimvel
//���������
//������poslimvelֵ
int16 GTSD_CMD_ST_SetPosLimVel(int16 axis, int16 poslimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = POS_LIM_VEL_WR_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int16 tmp;
	tmp = (int16)(poslimvel / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//����poslimvel

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡposlimvel
//���������
//������poslimvel
int16 GTSD_CMD_ST_GetPosLimVel(int16 axis, int16* poslimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = POS_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*poslimvel) = (int16)(((double)dspdata[0]) / ((double)OUTPUT_LIMIT_SCALE)*100.0);
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����neglimvel
//���������
//������neglimvelֵ
int16 GTSD_CMD_ST_SetNegLimVel(int16 axis, int16 neglimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = NEG_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ	
	int16 tmp;
	tmp = (int16)(neglimvel / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//����neglimvel


	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡneglimvel
//���������
//������neglimvel
int16 GTSD_CMD_ST_GetNegLimVel(int16 axis, int16* neglimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = NEG_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*neglimvel) = (int16)(((double)dspdata[0]) / ((double)OUTPUT_LIMIT_SCALE)*100.0);
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����fnpos
//���������
//������fnposֵ
int16 GTSD_CMD_ST_SetFnPos(int16 axis, double fnpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = FN_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (int16)((fnpos)*10.0 + 0.5);											//����fnpos

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡfnpos
//���������
//������fnpos
int16 GTSD_CMD_ST_GetFnPos(int16 axis, double* fnpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = FN_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*fnpos) = ((double)dspdata[0]) / 10.0;
		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����abslimpos
//���������
//������abslimposֵ
int16 GTSD_CMD_ST_SetAbsLimPos(int16 axis, int16 abslimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = ABS_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int16 tmp;
	tmp = (int16)(abslimpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//����abslimpos


	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡabslimpos
//���������
//������abslimpos
int16 GTSD_CMD_ST_GetAbsLimPos(int16 axis, int16* abslimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = ABS_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*abslimpos) = (int16)(((double)dspdata[0]) / ((double)OUTPUT_LIMIT_SCALE)*100.0);
		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����poslimpos
//���������
//������poslimposֵ
int16 GTSD_CMD_ST_SetPosLimPos(int16 axis, int16 poslimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = POS_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ	
	int16 tmp;
	tmp = (int16)(poslimpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//����poslimpos


	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡposlimpos
//���������
//������poslimpos
int16 GTSD_CMD_ST_GetPosLimPos(int16 axis, int16* poslimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = POS_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*poslimpos) = (int16)(((double)dspdata[0]) / ((double)OUTPUT_LIMIT_SCALE)*100.0);
		return Rt_Success;
	}
}

//////////////////////////////////////////////////////////////////////////
//����neglimpos
//���������
//������neglimposֵ
int16 GTSD_CMD_ST_SetNegLimPos(int16 axis, int16 neglimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = NEG_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ	
	int16 tmp;
	tmp = (int16)(neglimpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//����neglimpos

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡneglimpos
//���������
//������neglimpos	
int16 GTSD_CMD_ST_GetNegLimPos(int16 axis, int16* neglimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = NEG_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*neglimpos) = (int16)(((double)dspdata[0]) / ((double)OUTPUT_LIMIT_SCALE)*100.0);
		return Rt_Success;
	}
}


//////////////////////////////////////////////////////////////////////////
//����kgaffdpos
//���������
//������kgaffdposֵ
int16 GTSD_CMD_ST_SetKgaFfdPos(int16 axis, int16 kgaffdpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = KGA_FF_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int16 tmp;
	tmp = (int16)(kgaffdpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//����kgaffdpos

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡkgaffdpos
//���������
//������kgaffdpos	
int16 GTSD_CMD_ST_GetKgaFfdPos(int16 axis, int16* kgaffdpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = KGA_FF_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*kgaffdpos) = (int16)(((double)dspdata[0]) / ((double)OUTPUT_LIMIT_SCALE)*100.0);
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����kgvffdpos
//���������
//������kgvffdposֵ
int16 GTSD_CMD_ST_SetKgvFfdPos(int16 axis, int16 kgvffdpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = KGV_FF_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ
	int16 tmp;
	tmp = (int16)(kgvffdpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//����kgvffdpos

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡkgvffdpos
//���������
//������kgvffdpos	
int16 GTSD_CMD_ST_GetKgvFfdPos(int16 axis, int16* kgvffdpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = KGV_FF_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		(*kgvffdpos) = (int16)(((double)dspdata[0]) / ((double)OUTPUT_LIMIT_SCALE)*100.0);
		return Rt_Success;
	}
}
*/
//////////////////////////////////////////////////////////////////////////
//ͨ����ַ����16bit��fpga
int16 GTSD_CMD_ST_Set16bitFPGAByAddr(int16 dsp_number, int16 com_addr, int16 value, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	int16 base_addr;
	if (dsp_number == GTSD_DSP_A)
	{
		base_addr = FPGA_DSPA_BASEADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		base_addr = (int16)FPGA_DSPB_BASEADDR;
	}
	else
	{
		return Net_Rt_param_Err;
	}
	int16 comAddr = base_addr + (com_addr >> 1);
	int16 Data = value;
	int16 comNum = 1;
	int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &Data, comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//ͨ����ַ��ȡ16bit��fpga
int16 GTSD_CMD_ST_Get16bitFPGAByAddr(int16 dsp_number, int16 com_addr, int16* pvalue, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	int16 base_addr;
	if (dsp_number == GTSD_DSP_A)
	{
		base_addr = FPGA_DSPA_BASEADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		base_addr = (int16)FPGA_DSPB_BASEADDR;
	}
	else
	{
		return Net_Rt_param_Err;
	}
	int16 comAddr = base_addr + (com_addr >> 1);
	int16 comNum = 1;
	int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, pvalue, comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//ͨ����ַ����32bit��fpga
int16 GTSD_CMD_ST_Set32bitFPGAByAddr(int16 dsp_number, int16 com_addr, int32 value, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	int16 base_addr;
	if (dsp_number == GTSD_DSP_A)
	{
		base_addr = FPGA_DSPA_BASEADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		base_addr = (int16)FPGA_DSPB_BASEADDR;
	}
	else
	{
		return Net_Rt_param_Err;
	}
	int16 comAddr = base_addr + (com_addr >> 1);
	int32 Data = value;
	int16 high = ((Data >> 16) & 0xffff);
	int16 low = ((Data)& 0xffff);
	int16 comNum = 1;
	int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &low, comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	comAddr++;
	rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &high, comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//ͨ����ַ��ȡ32bit��fpga
int16 GTSD_CMD_ST_Get32bitFPGAByAddr(int16 dsp_number, int16 com_addr, int32* pvalue, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	int16 base_addr;
	if (dsp_number == GTSD_DSP_A)
	{
		base_addr = FPGA_DSPA_BASEADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		base_addr = (int16)FPGA_DSPB_BASEADDR;
	}
	else
	{
		return Net_Rt_param_Err;
	}
	int16 comAddr = base_addr + (com_addr >> 1);
	int16 high;
	int16 low;
	int16 comNum = 1;
	int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &low, comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	comAddr++;
	rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &high, comNum);
	*pvalue = ((((int32)(high << 16)) & 0xffff0000) | (low & 0x0000ffff));
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//������������
int16 GTSD_CMD_ST_SetWaveBuf(int16 dsp_number, WAVE_BUF_PRM wave, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ

	if (dsp_number == GTSD_DSP_A)
	{
		dsp_comAddr = DSPA_COMADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
	}
	else
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[64] = { 0 };											//ͨ������
	int16 Axis = 0;												//��Ŷ�����Ϊ0���ɣ���Ϊÿ��dsp�������ᣬ�����ĸ���һ��

	int16 cmd_id = WAVE_BUF_SET_WR_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;									//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = (wave.cmd.all);									//����controlword
	int16 j = 4;

	if (wave.cmd.bit.NUM >MAX_WAVE_PLOT_NUM)
	{
		return Net_Rt_param_Err;
	}

	for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)
	{
		dspdata[j] = wave.inf[i].bytes;
		dspdata[j + 1] = wave.inf[i].ofst;
		j += 2;
	}

	int16 dsp_comNum = j;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//��ȡ��������
int16 GTSD_CMD_ST_GetWaveBuf(int16 dsp_number, tWaveBufCmd* ctrlword, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ

	if (dsp_number == GTSD_DSP_A)
	{
		dsp_comAddr = DSPA_COMADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
	}
	else
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = 0;												//���

	int16 cmd_id = WAVE_BUF_SET_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		ctrlword->all = (dspdata[0]);
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//�������û�ȡ��������
int16 GTSD_CMD_ST_GetWaveData(int16 dsp_number, int16* read_num, int16** data, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	//�ȶ�ȡ����
	int16 dsp_comAddr = PLOTWAVE_DSPA_NUM_ADDR;										//��ַ

	if (dsp_number == GTSD_DSP_A)
	{
		dsp_comAddr = PLOTWAVE_DSPA_NUM_ADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		dsp_comAddr = (int16)(PLOTWAVE_DSPB_NUM_ADDR);
	}
	else
	{
		return Net_Rt_param_Err;
	}

	//�����ݴ��ݳ�ȥ
	(*data) = Cmd_PlotDataBuffer;

	int16 read_data_num = 0;
	int16 dsp_comNum = 1;
	int rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &read_data_num, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		*read_num = 0;
		return rtn;
	}

#ifdef CMD_TEST
	if (dsp_number == GTSD_DSP_A)
	{
		cmd_read_number_testA[cmd_read_number_test_indexA].value = read_data_num;
		if (read_data_num > (int16)PLOTWAVE_GET_DATA_MAX_ONCE)
		{
			cmd_read_number_testA[cmd_read_number_test_indexA].flag = 1;
		}
		else
		{
			cmd_read_number_testA[cmd_read_number_test_indexA].flag = 0;
		}
		cmd_read_number_test_indexA++;
		if (cmd_read_number_test_indexA >= 1000)
		{
			cmd_read_number_test_indexA = 0;
		}
		if (read_data_num == 8192)
		{
			cmd_exceed_numberA++;
		}
		if (read_data_num >= 4096)
		{
			cmd_exceed_numberA++;
		}
	}
	else
	{
		cmd_read_number_testB[cmd_read_number_test_indexB++] = read_data_num;
		if (cmd_read_number_test_indexB >= 1000)
		{
			cmd_read_number_test_indexB = 0;
		}
		if (read_data_num >= 3000)
		{
			cmd_exceed_numberB++;
		}
	}
#endif


	//�ٸ��ݶ�ȡ�ĸ��������ж�ȡ��������
	if (dsp_number == GTSD_DSP_A)
	{
		dsp_comAddr = PLOTWAVE_DSPA_DATA_ADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		dsp_comAddr = (int16)(PLOTWAVE_DSPB_DATA_ADDR);
	}
	else
	{
		return Net_Rt_param_Err;
	}

	int32 Cmd_PlotDataBuffer_index = 0; //index
	int16 Cmd_PlotDataBuffer_times = 0; //����
	//����fpga buffer�е����ݳ����趨ֵ����ô�Ͷ�ȡһ��
	if (read_data_num > (int16)PLOTWAVE_GET_DATA_MAX_ONCE)
	{
		while (read_data_num > (int16)PLOTWAVE_GET_DATA_MAX_ONCE)
		{
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &Cmd_PlotDataBuffer[Cmd_PlotDataBuffer_index], (int16)PLOTWAVE_GET_DATA_MAX_ONCE);
			if (rtn != GTSD_COM_SUCCESS)
			{
				*read_num = 0;
				return rtn;
			}
			Cmd_PlotDataBuffer_index += (int16)PLOTWAVE_GET_DATA_MAX_ONCE;
			read_data_num -= (int16)PLOTWAVE_GET_DATA_MAX_ONCE;
			Cmd_PlotDataBuffer_times++;
		}
		if (read_data_num != 0)
		{
			//�ٶ�ȡһ�ν�ʣ�µĶ���
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &Cmd_PlotDataBuffer[Cmd_PlotDataBuffer_index], (int16)read_data_num);
			if (rtn != GTSD_COM_SUCCESS)
			{
				*read_num = 0;
				return rtn;
			}
		}
		/*
		#ifdef CMD_TEST
		if (Cmd_PlotDataBuffer_times>=4)
		{
		Cmd_PlotDataBuffer_times = Cmd_PlotDataBuffer_times;
		}
		#endif
		*/
		//rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, Cmd_PlotDataBuffer, (int16)0);

#ifdef CMD_TEST
		/*
		int16 cnt_zero = 0;
		for (int32 k = 0; k < 480;k++)
		{
		if (Cmd_PlotDataBuffer[k] == 0)
		{
		cnt_zero++;
		}
		}
		if (cnt_zero>300)
		{
		cnt_zero = 0;
		}
		*/

		memcpy_s(&(plotdatabuffer_test[plotdatabuffer_test_index]), sizeof(int16)*((int16)PLOTWAVE_GET_DATA_MAX_ONCE)*Cmd_PlotDataBuffer_times, Cmd_PlotDataBuffer, sizeof(int16)*((int16)PLOTWAVE_GET_DATA_MAX_ONCE)*Cmd_PlotDataBuffer_times);
		plotdatabuffer_test_index += ((int16)PLOTWAVE_GET_DATA_MAX_ONCE)*Cmd_PlotDataBuffer_times;
		if (plotdatabuffer_test_index >= 48000)
		{
			plotdatabuffer_test_index = 0;
		}
#endif
		/**/

		*read_num = ((int16)PLOTWAVE_GET_DATA_MAX_ONCE)*Cmd_PlotDataBuffer_times + read_data_num;

		return Rt_Success;

	}
	else
	{
		*read_num = 0;
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//���fpga��FIFO
int16 GTSD_CMD_ST_ClearFpgaFifo(int16 dsp_number, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	//�ȶ�ȡ����
	int16 dsp_comAddr = PLOTWAVE_DSPA_NUM_ADDR;										//��ַ

	if (dsp_number == GTSD_DSP_A)
	{
		dsp_comAddr = PLOTWAVE_DSPA_NUM_ADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		dsp_comAddr = (int16)(PLOTWAVE_DSPB_NUM_ADDR);
	}
	else
	{
		return Net_Rt_param_Err;
	}

	int16 read_data_num = 0;
	int16 dsp_comNum = 1;
	int rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &read_data_num, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	//��ȡ���ݵĵ�ַ�͸����ĵ�ַ�ǲ�ͬ�ġ�
	if (dsp_number == GTSD_DSP_A)
	{
		dsp_comAddr = PLOTWAVE_DSPA_DATA_ADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		dsp_comAddr = (int16)(PLOTWAVE_DSPB_DATA_ADDR);
	}
	else
	{
		return Net_Rt_param_Err;
	}
	while (read_data_num >= (int16)PLOTWAVE_GET_DATA_MAX_ONCE)
	{
		//��ȡfifo�е�����
		rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, Cmd_PlotDataBuffer, (int16)PLOTWAVE_GET_DATA_MAX_ONCE);
		if (rtn != GTSD_COM_SUCCESS)
		{
			return rtn;
		}
		read_data_num -= (int16)PLOTWAVE_GET_DATA_MAX_ONCE;
	}
	//���ʣ���ֵһ�ζ�ȡ���
	rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, Cmd_PlotDataBuffer, read_data_num);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	//�ٶ�ȡһ��fifo�е����ݸ��������Ϊ0����ô�ͷ��سɹ�
	if (dsp_number == GTSD_DSP_A)
	{
		dsp_comAddr = PLOTWAVE_DSPA_NUM_ADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		dsp_comAddr = (int16)(PLOTWAVE_DSPB_NUM_ADDR);
	}
	else
	{
		return Net_Rt_param_Err;
	}
	rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &read_data_num, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	if (read_data_num == 0)
	{
		return Rt_Success;
	}
	else
	{
		return Net_Rt_param_Err;
	}

}
//////////////////////////////////////////////////////////////////////////
//����ƫ������һ��16bit��ֵ��FRAM
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Fram_Write16BitByAdr(int16 axis, int16 ofst, int16 value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };												//ͨ������
	int16 Axis = axis;													//���
	int16 dsp_comAddr = DSPA_COMADDR;											//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;												//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_FRAM_16BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);						//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;										//д����	
	dspdata[2] = 0;													//����ֵ											
	dspdata[3] = ofst;													//����ofst
	dspdata[4] = value;												//����value

	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ�ô�FRAM��ȡһ��16bit��ֵ
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Fram_Read16BitByAdr(int16 axis, int16 ofst, int16* value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_FRAM_16BIT_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;									//������	
	dspdata[2] = 0;												//����ֵ																		
	dspdata[3] = ofst;												//����ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		*value = dspdata[1];
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ������һ��32bit��ֵ��FRAM
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Fram_Write32BitByAdr(int16 axis, int16 ofst, int32 value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_FRAM_32BIT_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = ofst;												//����ofst
	dspdata[4] = (value & 0xffff);									//����value,�����õ�16bit
	dspdata[5] = ((value >> 16) & 0xffff);							//����value,�����ø�16bit

	int16 dsp_comNum = 6;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ�ô�FRAM��ȡһ��32bit��ֵ
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Fram_Read32BitByAdr(int16 axis, int16 ofst, int32* value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_FRAM_32BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		
	dspdata[3] = ofst;												//����ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		*value = (((dspdata[1]) & 0x0000ffff) | ((dspdata[2] << 16) & 0xffff0000));								//��λ�ں�
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ������һ��64bit��ֵ��FRAM
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Fram_Write64BitByAdr(int16 axis, int16 ofst, int64 value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_FRAM_64BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = ofst;												//����ofst
	dspdata[4] = (value & 0xffff);									//����value,�����õ�16bit
	dspdata[5] = ((value >> 16) & 0xffff);							//����value,������  16bit
	dspdata[6] = ((value >> 32) & 0xffff);							//����value,������  16bit
	dspdata[7] = (((value >> 32) >> 16) & 0xffff);					//����value,�����ø�16bit

	int16 dsp_comNum = 8;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//����ƫ�ô�FRAM��ȡһ��64bit��ֵ
//���������
//������ƫ��
//������ֵ
int16 GTSD_CMD_ST_Fram_Read64BitByAdr(int16 axis, int16 ofst, int64* value, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_FRAM_64BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		
	dspdata[3] = ofst;												//����ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		int64 tmp, tmp1;
		tmp = (((int64)dspdata[4]) << 32);
		tmp1 = ((tmp << 16) & 0xffff000000000000);
		*value = (((int64)(dspdata[1]) & 0x000000000000ffff) | ((((int64)dspdata[2]) << 16) & 0x00000000ffff0000) | ((((int64)dspdata[3]) << 32) & 0x0000ffff00000000) | tmp1);								//��λ�ں�
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_FlashWrite(int16 axis, INTEL_HEX_FRAME* packet, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[100] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_SPI_FLASH_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;												//д����	
	dspdata[2] = 0;												//����ֵ											
	dspdata[3] = packet->lenth;											
	dspdata[4] = ((packet->addr >> 16) & 0xffff);
	dspdata[5] = (packet->addr & 0xffff);						
	dspdata[6] = (packet->type);							
	for (int16 i = 0; i < packet->lenth; ++i)
	{
		dspdata[7 + i] = packet->data[i];
	}
	dspdata[7 + packet->lenth] = packet->checksum;

	int16 dsp_comNum = 8 + packet->lenth;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_FlashRead(int16 axis, INTEL_HEX_FRAME* packet_w, INTEL_HEX_FRAME* packet_r, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[200] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = WR_SPI_FLASH_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		
	dspdata[3] = packet_w->lenth;												
	dspdata[4] = ((packet_w->addr >> 16) & 0xffff);
	dspdata[5] = (packet_w->addr & 0xffff);
	int16 dsp_comNum = 100;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		packet_r->lenth = (Uint8)dspdata[0];
		packet_r->addr = ((Uint32)((Uint16)dspdata[1] << 16) | (Uint16)(dspdata[2]));
		for (int16 i = 0; i < packet_r->lenth;++i)
		{
			packet_r->data[i] = (Uint8)dspdata[3 + i];
		}
		
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_FlashErase(int16 axis, int16 blockNum, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;													//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = SPI_ERASE_FLASH_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;									//д����	
	dspdata[2] = 0;													//����ֵ	
	dspdata[3] = blockNum;											//��Ҫ������block number

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_InterruptSwitch(int16 axis, int16 int_switch, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;													//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = INTERRUPT_SWITCH_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;									//д����	
	dspdata[2] = 0;													//����ֵ											
	dspdata[3] = int_switch;											//�жϿ���							

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_ProcessorGeneralFunc(int16 axis, GENERALFUNCTION* gefunc, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	if ((gefunc->data == NULL))
	{
		return Net_Rt_param_Err;
	}
	if (gefunc->dataLenth == 0)
	{
		return Net_Rt_param_Err;
	}
	if (gefunc->mode >1)
	{
		return Net_Rt_param_Err;
	}
	if (gefunc->cmd > TUNNING_MESSAGE_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16 Axis			 = axis;														//���
	int16 dsp_comAddr	 = DSPA_COMADDR;												//��ַ
	if (Axis > 1)
	{
		dsp_comAddr		 = (int16)(DSPB_COMADDR);
		Axis			 = Axis % 2;													//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}
	gefunc->data[0]		 = GetCmdIDAndAxisNum(gefunc->cmd, Axis);						//�ϲ���ź�����id
	gefunc->data[1]		 = gefunc->mode;												//����ģʽ	д��0 ��1
	gefunc->data[2]		 = 0;															//����ֵ	

	int16 dsp_comNum     = gefunc->dataLenth;											//���ݳ����ǰ����������ܳ���

	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, (1 - gefunc->mode), dsp_comAddr, gefunc->data, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	return Rt_Success;
	
}

int16 GTSD_CMD_ST_ResetSystem(int16 axis, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;													//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = RESET_SYSTEM_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;									//д����	
	dspdata[2] = 0;													//����ֵ											

	int16 dsp_comNum = 3;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_CheckResetFinish(int16 axis, bool& flag_finish, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	flag_finish = 0;

	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = CHECK_RESET_FINISH_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		
	int32 value1, value2;
	int16 dsp_comNum = 15;
	//��ȡ��һ��
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		value1 = (((dspdata[0]) & 0x0000ffff) | ((dspdata[1] << 16) & 0xffff0000));								//��λ�ں�
	}
	Sleep(1);
	//��ȡ�ڶ���
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;
	rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		value2 = (((dspdata[0]) & 0x0000ffff) | ((dspdata[1] << 16) & 0xffff0000));								//��λ�ں�
	}

	if ((value2>value1) &&(value1!=0)&&(value2!=0))
	{
		flag_finish = true;
	}
	else
	{
		flag_finish = false;
	}
	return Rt_Success;
}

int16 GTSD_CMD_ST_ReadProcessorVersion(int16 axis, Uint16& ver, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;												//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = RD_PROCESSOR_VER_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_READ;												//������	
	dspdata[2] = 0;												//����ֵ																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		ver = dspdata[0];			
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_ReadFirmwareVersion(int16 axis, Uint16& ver, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16 Axis = axis;												//���
	int16 dsp_number = GTSD_DSP_A;	
	int16 com_addr = (int16)FPGA_VERSION;
	if (Axis > 1)
	{
		dsp_number = (int16)(GTSD_DSP_B);
	}
	int16 rtn =  GTSD_CMD_ST_Get16bitFPGAByAddr(dsp_number,com_addr, (int16*)(&ver));
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_ClrAlarm(int16 axis, int16 com_type /*= GTSD_COM_TYPE_NET*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//ͨ������
	int16 Axis = axis;													//���
	int16 dsp_comAddr = DSPA_COMADDR;										//��ַ
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//�����2����ô����һ��dsp�������轫��Ÿ���Ϊ0
	}

	int16 cmd_id = ALARM_CLEAR_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//�ϲ���ź�����id
	dspdata[1] = GTSD_DSP_WRITE;									//д����	
	dspdata[2] = 0;													//����ֵ											

	int16 dsp_comNum = 3;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}
