// CmdDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "BaseReturn_def.h"
#include "serialport.h"
#include "ST_GTSD_Cmd.h"
//////////////////////////////////////////////////////////////////////////
#ifdef CMD_TEST
	typedef struct testcmd_read_number_testA
	{
		int16 value;
		int16 flag;
	}TEST_cmd_read_number_testA;
	//每次读到的数据个数
	TEST_cmd_read_number_testA cmd_read_number_testA[1000];
	int16 cmd_read_number_test_indexA = 0;
	int16 cmd_read_number_testB[1000];
	int16 cmd_read_number_test_indexB = 0;
	//记录数据超出的个数
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
//将命令ID和轴号合并成一个short，其中bit[0-11]为命令ID, bit[12 - 15]为轴号
static int16 GetCmdIDAndAxisNum(short cmdID, short motorNum)
{
	short ret;
	ret = cmdID;
	ret += (motorNum << 12);
	return ret;
}
//////////////////////////////////////////////////////////////////////////
//打开命令通道
int16 GTSD_CMD_ST_OPEN(void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 comType)
{
	void* ptr = ptrv;
	//定义对象
	if (g_AbsCom == NULL)
	{
		g_AbsCom = new CAbsCom;
	}
	else
	{ 
		 return Rt_Success;//如果设备已经打开，直接返回
	}
	if (g_AbsCom == NULL)
	{
		return Net_Rt_CreateObj_Err;
	}
	//百分比进度
	progress = 10;
	(*tpfUpdataProgressPt)(ptr, &progress);

	int16 rtn;
	//调用open函数
	rtn = g_AbsCom->GTSD_Com_Open(tpfUpdataProgressPt, ptr, progress, comType);
	return rtn;
}
//////////////////////////////////////////////////////////////////////////
//关闭命令通道
int16 GTSD_CMD_ST_CLOSE(int16 comType)
{
	//假如已经关闭了就直接退出
	if (g_AbsCom == NULL)
	{
		return Rt_Success;
	}

	int16 rtn;
	//调用close函数
	rtn = g_AbsCom->GTSD_Com_Close(comType);
	if (g_AbsCom != NULL)
	{
		delete g_AbsCom;
	}
	g_AbsCom = NULL;

	return rtn;
}
//////////////////////////////////////////////////////////////////////////
//上伺服
//参数：轴号
int16 GTSD_CMD_ST_SetServoOn(int16 axis, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = SERVO_EN_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;									//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = 1;												//伺服使能							

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//下伺服
//参数：轴号
int16 GTSD_CMD_ST_SetServoOff(int16 axis, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = SERVO_EN_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;									//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = 0;												//伺服off							

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取伺服状态
//参数：轴号
//伺服状态结构体指针
int16 GTSD_CMD_ST_GetServoState(int16 axis, SERVO_STATE* serv, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = SERVO_EN_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;									//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);

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
//设置伺服任务模式
//参数：轴号
//参数：模式
int16 GTSD_CMD_ST_SetServoTaskMode(int16 axis, int16 mode, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = SERVO_TASK_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = mode;												//设置模式							

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取伺服任务模式
//参数：轴号
//参数：模式
int16 GTSD_CMD_ST_GetServoTaskMode(int16 axis, SERVO_MODE* mode, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = SERVO_TASK_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置混合模式
//参数：轴号
int16 GTSD_CMD_ST_SetMixMode(int16 axis, int16 com_type)
{
	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
//读取混合模式
//参数：轴号
int16 GTSD_CMD_ST_GetMixMode(int16 axis, int16 com_type)
{
	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
//根据偏置设置一个16bit的值到处理器
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Write16BitByAdr(int16 axis, int16 ofst, int16 value, void* ptr, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16* val = NULL;
	if (ptr != NULL)
	{
		val = (int16*)ptr;
	}
	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}
	int16 cmd_id	= WR_16BIT_COMM;							//cmd id		
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	if (val == NULL)
	{
		dspdata[1] = (GTSD_DSP_WRITE);
	}
	else
	{
		dspdata[1] = (GTSD_DSP_WRITE | (val[0] <<1));												//写命令	
	}
	dspdata[2] = 0;												//返回值											
	dspdata[3] = ofst;												//设置ofst
	dspdata[4] = value;											//设置value


	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据偏置从处理器读取一个16bit的值
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Read16BitByAdr(int16 axis, int16 ofst, int16* value, void* ptr, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16* val = NULL;
	if (ptr != NULL)
	{
		val = (int16*)ptr;
	}
	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_16BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	if (val == NULL)
	{
		dspdata[1] = (GTSD_DSP_READ);
	}
	else
	{
		dspdata[1] = (GTSD_DSP_READ | (val[0] << 1));												//读命令	
	}
	dspdata[2] = 0;												//返回值																		
	dspdata[3] = ofst;												//设置ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据偏置设置一个32bit的值到处理器
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Write32BitByAdr(int16 axis, int16 ofst, int32 value, void* ptr, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16* val = NULL;
	if (ptr != NULL)
	{
		val = (int16*)ptr;
	}
	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_32BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	if (val == NULL)
	{
		dspdata[1] = (GTSD_DSP_WRITE);
	}
	else
	{
		dspdata[1] = (GTSD_DSP_WRITE | (val[0] << 1));												//写命令	
	}
	dspdata[2] = 0;												//返回值											
	dspdata[3] = ofst;												//设置ofst
	dspdata[4] = (value & 0xffff);									//设置value,先设置低16bit
	dspdata[5] = ((value >> 16) & 0xffff);							//设置value,再设置高16bit

	int16 dsp_comNum = 6;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据偏置从处理器读取一个32bit的值
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Read32BitByAdr(int16 axis, int16 ofst, int32* value, void* ptr, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16* val = NULL;
	if (ptr != NULL)
	{
		val = (int16*)ptr;
	}
	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_32BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	if (val == NULL)
	{
		dspdata[1] = (GTSD_DSP_READ);
	}
	else
	{
		dspdata[1] = (GTSD_DSP_READ | (val[0] << 1));												//读命令	
	}
	dspdata[2] = 0;												//返回值																		
	dspdata[3] = ofst;												//设置ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		*value = (((dspdata[1]) & 0x0000ffff) | ((dspdata[2] << 16) & 0xffff0000));								//高位在后
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//根据偏置设置一个64bit的值到处理器
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Write64BitByAdr(int16 axis, int16 ofst, int64 value, void* ptr, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16* val = NULL;
	if (ptr != NULL)
	{
		val = (int16*)ptr;
	}
	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_64BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	if (val == NULL)
	{
		dspdata[1] = (GTSD_DSP_WRITE);
	}
	else
	{
		dspdata[1] = (GTSD_DSP_WRITE | (val[0] << 1));												//写命令	
	}
	dspdata[2] = 0;												//返回值											
	dspdata[3] = ofst;												//设置ofst
	dspdata[4] = (value & 0xffff);									//设置value,先设置低16bit
	dspdata[5] = ((value >> 16) & 0xffff);							//设置value,再设置  16bit
	dspdata[6] = ((value >> 32) & 0xffff);							//设置value,再设置  16bit
	dspdata[7] = (((value >> 32) >> 16) & 0xffff);					//设置value,再设置高16bit

	int16 dsp_comNum = 8;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据偏置从处理器读取一个64bit的值
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Read64BitByAdr(int16 axis, int16 ofst, int64* value, void* ptr, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16* val = NULL;
	if (ptr!= NULL)
	{
		val = (int16*)ptr;
	}
	
	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_64BIT_COMM;									//cmd id

	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	if (val == NULL)
	{
		dspdata[1] = (GTSD_DSP_READ);
	}
	else
	{
		dspdata[1] = (GTSD_DSP_READ | (val[0] << 1));												//读命令	
	}
	dspdata[2] = 0;												//返回值																		
	dspdata[3] = ofst;												//设置ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		int64 tmp, tmp1;
		tmp = (((int64)dspdata[4]) << 32);
		tmp1 = ((tmp << 16) & 0xffff000000000000);
		*value = (((int64)(dspdata[1]) & 0x000000000000ffff) | ((((int64)dspdata[2]) << 16) & 0x00000000ffff0000) | ((((int64)dspdata[3]) << 32) & 0x0000ffff00000000) | tmp1);								//高位在后
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//设置id指令
//参数：轴号
//参数：id指令参考值
int16 GTSD_CMD_ST_SetIdRef(int16 axis, double id_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = ID_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值	
	int32 tmp;
	tmp = (int32)((id_ref) / 100.0*((double)(MAX_CUR_SCALE)) + 0.5);
	if ((tmp>32767) || (tmp<-32768))
	{
		tmp = (int32)(0.05*((double)(MAX_CUR_SCALE)) + 0.5);
	}
	dspdata[3] = (int16)tmp;											//设置id_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取id指令
//参数：轴号
//参数：id指令参考值
int16 GTSD_CMD_ST_GetIdRef(int16 axis, ID_STATE* id_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = ID_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置iq指令
//参数：轴号
//参数：id指令参考值
int16 GTSD_CMD_ST_SetIqRef(int16 axis, double iq_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = IQ_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int32 tmp;
	tmp = (int32)((iq_ref) / 100.0*((double)(MAX_CUR_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < -32768))
	{
		tmp = (int32)(0.05*((double)(MAX_CUR_SCALE)) + 0.5);
	}
	dspdata[3] = (int16)tmp;										//设置iq_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取iq指令
//参数：轴号
//参数：iq指令参考值
int16 GTSD_CMD_ST_GetIqRef(int16 axis, IQ_STATE* iq_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = IQ_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置速度指令
//参数：轴号
//参数：速度指令参考值比率
int16 GTSD_CMD_ST_SetSpdRef(int16 axis, double spd_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = SPD_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int64 tmp;
	tmp = (int64)(spd_ref / 100.0*((double)MAX_SPD_SCALE) + 0.5);
	if ((tmp>0x7fffffff) || (tmp<-0x7fffffff))
	{
		tmp = (int64)(0.05*((double)MAX_SPD_SCALE) + 0.5);
	}
	int32 tmp1;
	tmp1 = (int32)tmp;
	dspdata[3] = (tmp1 & 0xffff);									//设置spd_ref低16位
	dspdata[4] = ((tmp1 >> 16) & 0xffff);								//设置spd_ref高16位

	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取速度指令
//参数：轴号
//参数：速度指令参考值比率
int16 GTSD_CMD_ST_GetSpdRef(int16 axis, SPD_STATE* spd_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = SPD_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设压力指令
//参数：轴号
//参数：压力指令参考值
int16 GTSD_CMD_ST_SetPreRef(int16 axis, int16 pre_ref, int16 com_type)
{
	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
//读取压力指令
//参数：轴号
//参数：压力指令参考值
int16 GTSD_CMD_ST_GetPreRef(int16 axis, int16* pre_ref, int16 com_type)
{
	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
//设置udref指令
//参数：轴号
//参数：ud指令参考值
int16 GTSD_CMD_ST_SetUdRef(int16 axis, double ud_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UD_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int32 tmp;
	tmp = (int32)(ud_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp>16384) || (tmp<-16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;											//设置ud_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取udref指令
//参数：轴号
//参数：ud指令参考值
int16 GTSD_CMD_ST_GetUdRef(int16 axis, UD_STATE* ud_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UD_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置uqref指令
//参数：轴号
//参数：uq指令参考值
int16 GTSD_CMD_ST_SetUqRef(int16 axis, double uq_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UQ_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int32 tmp;
	tmp = (int32)(uq_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;											//设置uq_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取uqref指令
//参数：轴号
//参数：uq指令参考值
int16 GTSD_CMD_ST_GetUqRef(int16 axis, UQ_STATE* uq_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UQ_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置uaref指令
//参数：轴号
//参数：ua指令参考值
int16 GTSD_CMD_ST_SetUaRef(int16 axis, double ua_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UA_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int32 tmp;
	tmp = (int32)(ua_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;											//设置ua_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取uaref指令
//参数：轴号
//参数：ua指令参考值
int16 GTSD_CMD_ST_GetUaRef(int16 axis, UA_STATE* ua_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UA_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置ubref指令
//参数：轴号
//参数：ub指令参考值
int16 GTSD_CMD_ST_SetUbRef(int16 axis, double ub_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UB_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值	
	int32 tmp;
	tmp = (int32)(ub_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;										//设置ub_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取ubref指令
//参数：轴号
//参数：ub指令参考值
int16 GTSD_CMD_ST_GetUbRef(int16 axis, UB_STATE* ub_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UB_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置ucref指令
//参数：轴号
//参数：uc指令参考值
int16 GTSD_CMD_ST_SetUcRef(int16 axis, double uc_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UC_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;									//写命令	
	dspdata[2] = 0;												//返回值
	int32 tmp;
	tmp = (int32)(uc_ref / 100.0*((double)MAX_VOL_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_VOL_SCALE));
	}
	dspdata[3] = (int16)tmp;											//设置uc_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取ucref指令
//参数：轴号
//参数：uc指令参考值
int16 GTSD_CMD_ST_GetUcRef(int16 axis, UC_STATE* uc_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = UC_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置PosAdjref指令
//参数：轴号
//参数：PosAdj指令参考值
int16 GTSD_CMD_ST_SetPosAdjRef(int16 axis, double PosAdj_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = POS_ADJ_IREF_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int32 tmp;
	tmp = (int32)(PosAdj_ref / 100.0*((double)MAX_CUR_SCALE) + 0.5);
	if ((tmp > 16384) || (tmp < -16384))
	{
		tmp = (int32)(0.05*((double)MAX_CUR_SCALE));
	}
	dspdata[3] = (int16)tmp;										//设置PosAdj_ref

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取PosAdjref指令
//参数：轴号
//参数：PosAdj指令参考值
int16 GTSD_CMD_ST_GetPosAdjRef(int16 axis, POS_ADJ_STATE* pos_adj_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = POS_ADJ_IREF_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置Posref指令
//参数：轴号
//参数：Pos指令参考值
int16 GTSD_CMD_ST_SetPosRef(int16 axis, int32 Pos_ref, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = POS_REF_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (Pos_ref & 0xffff);									//设置Pos_ref 低16bit
	dspdata[4] = ((Pos_ref >> 16) & 0xffff);							//设置Pos_ref 高16bit

	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//设置FndCurr
//参数：轴号
//参数：FndCurr值
int16 GTSD_CMD_ST_SetFndCurr(int16 axis, int16 fnd, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = FND_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (fnd);											//设置fnd

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
//读取FndCurr
//参数：轴号
//参数：FndCurr
int16 GTSD_CMD_ST_GetFndCurr(int16 axis, int16* fnd, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = FND_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置FnqCurr
//参数：轴号
//参数：FnqCurr值
int16 GTSD_CMD_ST_SetFnqCurr(int16 axis, int16 fnq, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = FNQ_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (fnq);											//设置fnq

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
//读取FnqCurr
//参数：轴号
//参数：FnqCurr
int16 GTSD_CMD_ST_GetFnqCurr(int16 axis, int16* fnq, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = FNQ_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置TidCurr
//参数：轴号
//参数：TidCurr值
int16 GTSD_CMD_ST_SetTidCurr(int16 axis, int32 tid, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = TID_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (tid & 0xffff);										//设置tid低16bit
	dspdata[4] = ((tid >> 16) & 0xffff);								//设置tid高16bit

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
//读取TidCurr
//参数：轴号
//参数：TidCurr
int16 GTSD_CMD_ST_GetTidCurr(int16 axis, int32* tid, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = TID_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置TiqCurr
//参数：轴号
//参数：TiqCurr值
int16 GTSD_CMD_ST_SetTiqCurr(int16 axis, int32 tiq, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = TIQ_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (tiq & 0xffff);										//设置tiq低16bit
	dspdata[4] = ((tiq >> 16) & 0xffff);								//设置tiq高16bit

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
//读取TiqCurr
//参数：轴号
//参数：TiqCurr
int16 GTSD_CMD_ST_GetTiqCurr(int16 axis, int32* tiq, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = TIQ_CURR_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置Fnvel
//参数：轴号
//参数：Fnvel值
int16 GTSD_CMD_ST_SetFnVel(int16 axis, int16 fnvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = FN_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (fnvel);											//设置fnvel

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
//读取Fnvel
//参数：轴号
//参数：Fnvel
int16 GTSD_CMD_ST_GetFnVel(int16 axis, int16* fnvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = FN_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置Tivel
//参数：轴号
//参数：Tivel值
int16 GTSD_CMD_ST_SetTiVel(int16 axis, double tivel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = TI_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (int16)(((tivel)*100.0) + 0.5);							//设置tivel,下面值为0.01ms---1

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
//读取Tivel
//参数：轴号
//参数：Tivel
int16 GTSD_CMD_ST_GetTiVel(int16 axis, double* tivel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = TI_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置abslimvel
//参数：轴号
//参数：abslimvel值
int16 GTSD_CMD_ST_SetAbsLimVel(int16 axis, int16 abslimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = ABS_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int16 tmp;
	tmp = (int16)(abslimvel / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp>32767) || (tmp<0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;										//设置abslimvel

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
//读取abslimvel
//参数：轴号
//参数：abslimvel
int16 GTSD_CMD_ST_GetAbsLimVel(int16 axis, int16* abslimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = ABS_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置poslimvel
//参数：轴号
//参数：poslimvel值
int16 GTSD_CMD_ST_SetPosLimVel(int16 axis, int16 poslimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = POS_LIM_VEL_WR_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int16 tmp;
	tmp = (int16)(poslimvel / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//设置poslimvel

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
//读取poslimvel
//参数：轴号
//参数：poslimvel
int16 GTSD_CMD_ST_GetPosLimVel(int16 axis, int16* poslimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = POS_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置neglimvel
//参数：轴号
//参数：neglimvel值
int16 GTSD_CMD_ST_SetNegLimVel(int16 axis, int16 neglimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = NEG_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值	
	int16 tmp;
	tmp = (int16)(neglimvel / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//设置neglimvel


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
//读取neglimvel
//参数：轴号
//参数：neglimvel
int16 GTSD_CMD_ST_GetNegLimVel(int16 axis, int16* neglimvel, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = NEG_LIM_VEL_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置fnpos
//参数：轴号
//参数：fnpos值
int16 GTSD_CMD_ST_SetFnPos(int16 axis, double fnpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = FN_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (int16)((fnpos)*10.0 + 0.5);											//设置fnpos

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
//读取fnpos
//参数：轴号
//参数：fnpos
int16 GTSD_CMD_ST_GetFnPos(int16 axis, double* fnpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = FN_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置abslimpos
//参数：轴号
//参数：abslimpos值
int16 GTSD_CMD_ST_SetAbsLimPos(int16 axis, int16 abslimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = ABS_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int16 tmp;
	tmp = (int16)(abslimpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//设置abslimpos


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
//读取abslimpos
//参数：轴号
//参数：abslimpos
int16 GTSD_CMD_ST_GetAbsLimPos(int16 axis, int16* abslimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = ABS_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置poslimpos
//参数：轴号
//参数：poslimpos值
int16 GTSD_CMD_ST_SetPosLimPos(int16 axis, int16 poslimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = POS_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值	
	int16 tmp;
	tmp = (int16)(poslimpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//设置poslimpos


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
//读取poslimpos
//参数：轴号
//参数：poslimpos
int16 GTSD_CMD_ST_GetPosLimPos(int16 axis, int16* poslimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = POS_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置neglimpos
//参数：轴号
//参数：neglimpos值
int16 GTSD_CMD_ST_SetNegLimPos(int16 axis, int16 neglimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = NEG_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值	
	int16 tmp;
	tmp = (int16)(neglimpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//设置neglimpos

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
//读取neglimpos
//参数：轴号
//参数：neglimpos	
int16 GTSD_CMD_ST_GetNegLimPos(int16 axis, int16* neglimpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = NEG_LIM_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置kgaffdpos
//参数：轴号
//参数：kgaffdpos值
int16 GTSD_CMD_ST_SetKgaFfdPos(int16 axis, int16 kgaffdpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = KGA_FF_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int16 tmp;
	tmp = (int16)(kgaffdpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//设置kgaffdpos

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
//读取kgaffdpos
//参数：轴号
//参数：kgaffdpos	
int16 GTSD_CMD_ST_GetKgaFfdPos(int16 axis, int16* kgaffdpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = KGA_FF_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//设置kgvffdpos
//参数：轴号
//参数：kgvffdpos值
int16 GTSD_CMD_ST_SetKgvFfdPos(int16 axis, int16 kgvffdpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = KGV_FF_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值
	int16 tmp;
	tmp = (int16)(kgvffdpos / 100.0*((double)(OUTPUT_LIMIT_SCALE)) + 0.5);
	if ((tmp > 32767) || (tmp < 0))
	{
		tmp = (int16)(OUTPUT_LIMIT_SCALE);
	}
	dspdata[3] = tmp;												//设置kgvffdpos

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
//读取kgvffdpos
//参数：轴号
//参数：kgvffdpos	
int16 GTSD_CMD_ST_GetKgvFfdPos(int16 axis, int16* kgvffdpos, int16 com_type)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = KGV_FF_POS_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

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
//通过地址设置16bit的fpga
int16 GTSD_CMD_ST_Set16bitFPGAByAddr(int16 dsp_number, int16 com_addr, int16 value, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	//输入是byte地址，pcdebug操作需要short地址，而等环网操作需要byte地址，
	int16 base_addr;
	int16 comAddr;
	int16 comNum;
	if (com_type == GTSD_COM_TYPE_NET)
	{
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
		comAddr = base_addr + (com_addr >> 1);
		comNum = 1;
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		if (((com_addr >= FPGA_RN_ENC_START_OFST) && (com_addr <= FPGA_RN_ENC_END_OFST))
			|| ((com_addr >= FPGA_RN_AIN_START_OFST) && (com_addr <= FPGA_RN_AIN_END_OFST))
			|| ((com_addr >= FPGA_RN_IO_START_OFST) && (com_addr <= FPGA_RN_IO_END_OFST))
			|| ((com_addr >= FPGA_RN_REF_START_OFST) && (com_addr <= FPGA_RN_REF_END_OFST))
			|| ((com_addr >= FPGA_RN_UART_START_OFST) && (com_addr <= FPGA_RN_UART_END_OFST)))
		{
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
			comAddr = base_addr + (com_addr); //因为对于等环网来说，0x8000是byte地址，输入的也是byte地址，最后需要的也是byte地址
		}
		else if (((com_addr >= FPGA_RN_CTL_START_OFST) && (com_addr <= FPGA_RN_CTL_END_OFST))
			|| ((com_addr >= FPGA_RN_TRIG_START_OFST) && (com_addr <= FPGA_RN_TRIG_END_OFST))
			|| ((com_addr >= FPGA_RN_RMT_START_OFST) && (com_addr <= FPGA_RN_RMT_END_OFST))
			|| ((com_addr >= FPGA_RN_SYS_START_OFST) && (com_addr <= FPGA_RN_SYS_END_OFST))
			|| ((com_addr >= FPGA_RN_RAM_START_OFST) && (com_addr <= FPGA_RN_RAM_END_OFST))
			|| ((com_addr >= FPGA_RN_ILINK_START_OFST) && (com_addr <= FPGA_RN_ILINK_END_OFST))
			|| ((com_addr >= FPGA_RN_USER_START_OFST) && (com_addr <= FPGA_RN_USER_END_OFST))
			|| ((com_addr >= FPGA_RN_APP_START_OFST) && (com_addr <= FPGA_RN_APP_END_OFST)))
		{
			//公用的dspnumber 可以都写为0，因为没有用到，实际上传入的偏移已经是绝对地址了。
			comAddr = (com_addr);
		}
		comNum = 1;
	}
	
	int16 Data = value;
	int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &Data, comNum, stationId);
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
//通过地址读取16bit的fpga
int16 GTSD_CMD_ST_Get16bitFPGAByAddr(int16 dsp_number, int16 com_addr, int16* pvalue, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	//输入是byte地址，pcdebug操作需要short地址，而等环网操作需要byte地址，
	int16 base_addr;
	int16 comAddr;
	int16 comNum;
	if (com_type == GTSD_COM_TYPE_NET)
	{
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
		comAddr = base_addr + (com_addr >> 1);
		comNum = 1;
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		if (((com_addr >= FPGA_RN_ENC_START_OFST) && (com_addr <= FPGA_RN_ENC_END_OFST))
			|| ((com_addr >= FPGA_RN_AIN_START_OFST) && (com_addr <= FPGA_RN_AIN_END_OFST))
			|| ((com_addr >= FPGA_RN_IO_START_OFST) && (com_addr <= FPGA_RN_IO_END_OFST))
			|| ((com_addr >= FPGA_RN_REF_START_OFST) && (com_addr <= FPGA_RN_REF_END_OFST))
			|| ((com_addr >= FPGA_RN_UART_START_OFST) && (com_addr <= FPGA_RN_UART_END_OFST)))
		{
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
			comAddr = base_addr + (com_addr); //因为对于等环网来说，0x8000是byte地址，输入的也是byte地址，最后需要的也是byte地址
		}
		else if (((com_addr >= FPGA_RN_CTL_START_OFST) && (com_addr <= FPGA_RN_CTL_END_OFST))
			|| ((com_addr >= FPGA_RN_TRIG_START_OFST) && (com_addr <= FPGA_RN_TRIG_END_OFST))
			|| ((com_addr >= FPGA_RN_RMT_START_OFST) && (com_addr <= FPGA_RN_RMT_END_OFST))
			|| ((com_addr >= FPGA_RN_SYS_START_OFST) && (com_addr <= FPGA_RN_SYS_END_OFST))
			|| ((com_addr >= FPGA_RN_RAM_START_OFST) && (com_addr <= FPGA_RN_RAM_END_OFST))
			|| ((com_addr >= FPGA_RN_ILINK_START_OFST) && (com_addr <= FPGA_RN_ILINK_END_OFST))
			|| ((com_addr >= FPGA_RN_USER_START_OFST) && (com_addr <= FPGA_RN_USER_END_OFST))
			|| ((com_addr >= FPGA_RN_APP_START_OFST) && (com_addr <= FPGA_RN_APP_END_OFST)))
		{
			//公用的dspnumber 可以都写为0，因为没有用到，实际上传入的偏移已经是绝对地址了。
			comAddr = (com_addr);
		}
		comNum = 1;
	}
	int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, pvalue, comNum, stationId);
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
//通过地址设置32bit的fpga
int16 GTSD_CMD_ST_Set32bitFPGAByAddr(int16 dsp_number, int16 com_addr, int32 value, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	//输入是byte地址，pcdebug操作需要short地址，而等环网操作需要byte地址，
	int16 base_addr;
	int16 comAddr;
	int16 comNum;
	if (com_type == GTSD_COM_TYPE_NET)
	{
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
		comAddr = base_addr + (com_addr >> 1);
		comNum = 1;
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		if (((com_addr >= FPGA_RN_ENC_START_OFST) && (com_addr <= FPGA_RN_ENC_END_OFST))
			|| ((com_addr >= FPGA_RN_AIN_START_OFST) && (com_addr <= FPGA_RN_AIN_END_OFST))
			|| ((com_addr >= FPGA_RN_IO_START_OFST) && (com_addr <= FPGA_RN_IO_END_OFST))
			|| ((com_addr >= FPGA_RN_REF_START_OFST) && (com_addr <= FPGA_RN_REF_END_OFST))
			|| ((com_addr >= FPGA_RN_UART_START_OFST) && (com_addr <= FPGA_RN_UART_END_OFST)))
		{
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
			comAddr = base_addr + (com_addr); //因为对于等环网来说，0x8000是byte地址，输入的也是byte地址，最后需要的也是byte地址
		}
		else if (((com_addr >= FPGA_RN_CTL_START_OFST) && (com_addr <= FPGA_RN_CTL_END_OFST))
			|| ((com_addr >= FPGA_RN_TRIG_START_OFST) && (com_addr <= FPGA_RN_TRIG_END_OFST))
			|| ((com_addr >= FPGA_RN_RMT_START_OFST) && (com_addr <= FPGA_RN_RMT_END_OFST))
			|| ((com_addr >= FPGA_RN_SYS_START_OFST) && (com_addr <= FPGA_RN_SYS_END_OFST))
			|| ((com_addr >= FPGA_RN_RAM_START_OFST) && (com_addr <= FPGA_RN_RAM_END_OFST))
			|| ((com_addr >= FPGA_RN_ILINK_START_OFST) && (com_addr <= FPGA_RN_ILINK_END_OFST))
			|| ((com_addr >= FPGA_RN_USER_START_OFST) && (com_addr <= FPGA_RN_USER_END_OFST))
			|| ((com_addr >= FPGA_RN_APP_START_OFST) && (com_addr <= FPGA_RN_APP_END_OFST)))
		{
			//公用的dspnumber 可以都写为0，因为没有用到，实际上传入的偏移已经是绝对地址了。
			comAddr = (com_addr);
		}
		comNum = 1;
	}
	int32 Data = value;
	int16 high = ((Data >> 16) & 0xffff);
	int16 low = ((Data)& 0xffff);

	int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &low, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	if (com_type == GTSD_COM_TYPE_NET)
	{
		comAddr++;
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		comAddr += 2;
	}
	else
	{
		comAddr++;
	}

	rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &high, comNum, stationId);
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
//通过地址读取32bit的fpga
int16 GTSD_CMD_ST_Get32bitFPGAByAddr(int16 dsp_number, int16 com_addr, int32* pvalue, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	//输入是byte地址，pcdebug操作需要short地址，而等环网操作需要byte地址，
	int16 base_addr;
	int16 comAddr;
	int16 comNum;
	if (com_type == GTSD_COM_TYPE_NET)
	{
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
		comAddr = base_addr + (com_addr >> 1);
		comNum = 1;
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		if (((com_addr >= FPGA_RN_ENC_START_OFST) && (com_addr <= FPGA_RN_ENC_END_OFST))
			|| ((com_addr >= FPGA_RN_AIN_START_OFST) && (com_addr <= FPGA_RN_AIN_END_OFST))
			|| ((com_addr >= FPGA_RN_IO_START_OFST) && (com_addr <= FPGA_RN_IO_END_OFST))
			|| ((com_addr >= FPGA_RN_REF_START_OFST) && (com_addr <= FPGA_RN_REF_END_OFST))
			|| ((com_addr >= FPGA_RN_UART_START_OFST) && (com_addr <= FPGA_RN_UART_END_OFST)))
		{
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
			comAddr = base_addr + (com_addr); //因为对于等环网来说，0x8000是byte地址，输入的也是byte地址，最后需要的也是byte地址
		}
		else if (((com_addr >= FPGA_RN_CTL_START_OFST) && (com_addr <= FPGA_RN_CTL_END_OFST))
			|| ((com_addr >= FPGA_RN_TRIG_START_OFST) && (com_addr <= FPGA_RN_TRIG_END_OFST))
			|| ((com_addr >= FPGA_RN_RMT_START_OFST) && (com_addr <= FPGA_RN_RMT_END_OFST))
			|| ((com_addr >= FPGA_RN_SYS_START_OFST) && (com_addr <= FPGA_RN_SYS_END_OFST))
			|| ((com_addr >= FPGA_RN_RAM_START_OFST) && (com_addr <= FPGA_RN_RAM_END_OFST))
			|| ((com_addr >= FPGA_RN_ILINK_START_OFST) && (com_addr <= FPGA_RN_ILINK_END_OFST))
			|| ((com_addr >= FPGA_RN_USER_START_OFST) && (com_addr <= FPGA_RN_USER_END_OFST))
			|| ((com_addr >= FPGA_RN_APP_START_OFST) && (com_addr <= FPGA_RN_APP_END_OFST)))
		{
			//公用的dspnumber 可以都写为0，因为没有用到，实际上传入的偏移已经是绝对地址了。
			comAddr = (com_addr);
		}
		comNum = 1;
	}
	int16 high;
	int16 low;

	int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &low, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	if (com_type == GTSD_COM_TYPE_NET)
	{
		comAddr++;
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		comAddr += 2;
	}
	else
	{
		comAddr++;
	}
	rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &high, comNum, stationId);
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
//设置曲线配置
int16 GTSD_CMD_ST_SetWaveBuf(int16 dsp_number, WAVE_BUF_PRM wave, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	int16 dsp_comAddr = DSPA_COMADDR;										//地址

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

	int16 dspdata[64] = { 0 };											//通信数组
	int16 Axis = 0;												//轴号都设置为0即可，因为每个dsp有两个轴，设置哪个都一样

	int16 cmd_id = WAVE_BUF_SET_WR_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;									//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = (wave.cmd.all);									//设置controlword
	int16 j = 4;

	if (wave.cmd.bit.NUM >MAX_WAVE_PLOT_NUM)
	{
		return Net_Rt_param_Err;
	}

	for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)
	{
		dspdata[j] = wave.inf[i].bytes;
		dspdata[j + 1] = wave.inf[i].ofst;
		dspdata[j + 2] = wave.inf[i].base;
		j += 3;
	}

	int16 dsp_comNum = j;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//读取曲线配置
int16 GTSD_CMD_ST_GetWaveBuf(int16 dsp_number, tWaveBufCmd* ctrlword, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	int16 dsp_comAddr = DSPA_COMADDR;										//地址

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

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = 0;												//轴号

	int16 cmd_id = WAVE_BUF_SET_WR_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据配置获取曲线数据
int16 GTSD_CMD_ST_GetWaveData(int16 dsp_number, int16* read_num, int16** data, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	//先读取个数
	int16 dsp_comAddr = PLOTWAVE_DSPA_NUM_ADDR;										//地址

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

	//将数据传递出去
	(*data) = Cmd_PlotDataBuffer;

	int16 read_data_num = 0;
	int16 dsp_comNum = 1;
	int rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &read_data_num, dsp_comNum, stationId);
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


	//再根据读取的个数来进行读取曲线数据
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
	int16 Cmd_PlotDataBuffer_times = 0; //次数
	//假如fpga buffer中的数据超过设定值，那么就读取一次
	if (read_data_num > (int16)PLOTWAVE_GET_DATA_MAX_ONCE)
	{
		while (read_data_num > (int16)PLOTWAVE_GET_DATA_MAX_ONCE)
		{
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &Cmd_PlotDataBuffer[Cmd_PlotDataBuffer_index], (int16)PLOTWAVE_GET_DATA_MAX_ONCE, stationId);
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
			//再读取一次将剩下的读完
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &Cmd_PlotDataBuffer[Cmd_PlotDataBuffer_index], (int16)read_data_num, stationId);
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
//清除fpga的FIFO
int16 GTSD_CMD_ST_ClearFpgaFifo(int16 dsp_number, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	//先读取个数
	int16 dsp_comAddr = PLOTWAVE_DSPA_NUM_ADDR;										//地址

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
	int rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &read_data_num, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	//获取数据的地址和个数的地址是不同的。
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
		//读取fifo中的数据
		rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, Cmd_PlotDataBuffer, (int16)PLOTWAVE_GET_DATA_MAX_ONCE, stationId);
		if (rtn != GTSD_COM_SUCCESS)
		{
			return rtn;
		}
		read_data_num -= (int16)PLOTWAVE_GET_DATA_MAX_ONCE;
	}
	//最后将剩余的值一次读取完毕
	rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, Cmd_PlotDataBuffer, read_data_num, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	//再读取一次fifo中的数据个数，如果为0，那么就返回成功
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
	rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, &read_data_num, dsp_comNum, stationId);
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
//根据偏置设置一个16bit的值到FRAM
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Fram_Write16BitByAdr(int16 axis, int16 ofst, int16 value, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };												//通信数组
	int16 Axis = axis;													//轴号
	int16 dsp_comAddr = DSPA_COMADDR;											//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;												//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_FRAM_16BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);						//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;										//写命令	
	dspdata[2] = 0;														//返回值											
	dspdata[3] = ofst;													//设置ofst
	dspdata[4] = value;													//设置value

	int16 dsp_comNum = 5;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据偏置从FRAM读取一个16bit的值
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Fram_Read16BitByAdr(int16 axis, int16 ofst, int16* value, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_FRAM_16BIT_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;									//读命令	
	dspdata[2] = 0;												//返回值																		
	dspdata[3] = ofst;												//设置ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据偏置设置一个32bit的值到FRAM
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Fram_Write32BitByAdr(int16 axis, int16 ofst, int32 value, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_FRAM_32BIT_COMM;								//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = ofst;												//设置ofst
	dspdata[4] = (value & 0xffff);									//设置value,先设置低16bit
	dspdata[5] = ((value >> 16) & 0xffff);							//设置value,再设置高16bit

	int16 dsp_comNum = 6;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据偏置从FRAM读取一个32bit的值
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Fram_Read32BitByAdr(int16 axis, int16 ofst, int32* value, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_FRAM_32BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		
	dspdata[3] = ofst;												//设置ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		*value = (((dspdata[1]) & 0x0000ffff) | ((dspdata[2] << 16) & 0xffff0000));								//高位在后
		return Rt_Success;
	}
}
//////////////////////////////////////////////////////////////////////////
//根据偏置设置一个64bit的值到FRAM
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Fram_Write64BitByAdr(int16 axis, int16 ofst, int64 value, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_FRAM_64BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
	dspdata[2] = 0;												//返回值											
	dspdata[3] = ofst;												//设置ofst
	dspdata[4] = (value & 0xffff);									//设置value,先设置低16bit
	dspdata[5] = ((value >> 16) & 0xffff);							//设置value,再设置  16bit
	dspdata[6] = ((value >> 32) & 0xffff);							//设置value,再设置  16bit
	dspdata[7] = (((value >> 32) >> 16) & 0xffff);					//设置value,再设置高16bit

	int16 dsp_comNum = 8;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
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
//根据偏置从FRAM读取一个64bit的值
//参数：轴号
//参数：偏置
//参数：值
int16 GTSD_CMD_ST_Fram_Read64BitByAdr(int16 axis, int16 ofst, int64* value, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_FRAM_64BIT_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		
	dspdata[3] = ofst;												//设置ofst

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		int64 tmp, tmp1;
		tmp = (((int64)dspdata[4]) << 32);
		tmp1 = ((tmp << 16) & 0xffff000000000000);
		*value = (((int64)(dspdata[1]) & 0x000000000000ffff) | ((((int64)dspdata[2]) << 16) & 0x00000000ffff0000) | ((((int64)dspdata[3]) << 32) & 0x0000ffff00000000) | tmp1);								//高位在后
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_FlashWrite(int16 axis, INTEL_HEX_FRAME* packet, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[100] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_SPI_FLASH_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;												//写命令	
  dspdata[2] = 0;												//返回值
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
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_FlashRead(int16 axis, INTEL_HEX_FRAME* packet_w, INTEL_HEX_FRAME* packet_r, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[200] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = WR_SPI_FLASH_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		
	dspdata[3] = packet_w->lenth;												
	dspdata[4] = ((packet_w->addr >> 16) & 0xffff);
	dspdata[5] = (packet_w->addr & 0xffff);
	int16 dsp_comNum = 8 + packet_w->lenth;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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

int16 GTSD_CMD_ST_FlashErase(int16 axis, int16 blockNum, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;													//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = SPI_ERASE_FLASH_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;									//写命令	
	dspdata[2] = 0;													//返回值	
	dspdata[3] = blockNum;											//需要擦出的block number

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_InterruptSwitch(int16 axis, int16 int_switch, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;													//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = INTERRUPT_SWITCH_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;									//写命令	
	dspdata[2] = 0;													//返回值											
	dspdata[3] = int_switch;											//中断开关							

	int16 dsp_comNum = 4;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_ProcessorGeneralFunc(int16 axis, GENERALFUNCTION* gefunc, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
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
	int16 Axis			 = axis;														//轴号
	int16 dsp_comAddr	 = DSPA_COMADDR;												//地址
	if (Axis > 1)
	{
		dsp_comAddr		 = (int16)(DSPB_COMADDR);
		Axis			 = Axis % 2;													//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}
	gefunc->data[0]		 = GetCmdIDAndAxisNum(gefunc->cmd, Axis);						//合并轴号和命令id
	gefunc->data[1]		 = gefunc->mode;												//命令模式	写：0 读1
	gefunc->data[2]		 = 0;															//返回值	

	int16 dsp_comNum     = gefunc->dataLenth;											//数据长度是包含回来的总长度

	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, (1 - gefunc->mode), dsp_comAddr, gefunc->data, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	return Rt_Success;
	
}

int16 GTSD_CMD_ST_ResetSystem(int16 axis, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;													//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = RESET_SYSTEM_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;									//写命令	
	dspdata[2] = 0;													//返回值											

	int16 dsp_comNum = 3;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_CheckResetFinish(int16 axis, bool& flag_finish, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	flag_finish = 0;

	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = CHECK_RESET_FINISH_COMM;									//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		
	int32 value1, value2;
	int16 dsp_comNum = 15;
	//读取第一次
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		value1 = (((dspdata[0]) & 0x0000ffff) | ((dspdata[1] << 16) & 0xffff0000));								//高位在后
	}
	Sleep(1);
	//读取第二次
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;
	rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		value2 = (((dspdata[0]) & 0x0000ffff) | ((dspdata[1] << 16) & 0xffff0000));								//高位在后
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

int16 GTSD_CMD_ST_ReadProcessorVersion(int16 axis, Uint16& ver, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;												//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = RD_PROCESSOR_VER_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 15;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
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

int16 GTSD_CMD_ST_ReadFirmwareVersion(int16 axis, Uint16& ver, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16 Axis = axis;												//轴号
	int16 dsp_number = GTSD_DSP_A;	
	int16 com_addr = (int16)FPGA_VERSION;
	if (Axis > 1)
	{
		dsp_number = (int16)(GTSD_DSP_B);
	}
	int16 rtn =  GTSD_CMD_ST_Get16bitFPGAByAddr(dsp_number,com_addr, (int16*)(&ver),com_type,stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_ClrAlarm(int16 axis, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[16] = { 0 };											//通信数组
	int16 Axis = axis;													//轴号
	int16 dsp_comAddr = DSPA_COMADDR;										//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = ALARM_CLEAR_COMM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_WRITE;									//写命令	
	dspdata[2] = 0;													//返回值											

	int16 dsp_comNum = 3;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_WRITE, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_OpenSerialPort(int16 axis, int32 baudRate, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	bool rtn;
	rtn = g_serial->open(axis, baudRate, com_type, stationId);

	if (rtn == true)
	{
		return Rt_Success;
	}
	else
	{
		return Net_Rt_SerialPort_Err;
	}

}

int16 GTSD_CMD_ST_CloseSerialPort(int16 axis, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	bool rtn;
	rtn = g_serial->close(axis, com_type, stationId);

	if (rtn == true)
	{
		return Rt_Success;
	}
	else
	{
		return Net_Rt_SerialPort_Err;
	}
}

int16 GTSD_CMD_ST_ReadSerialPort(int16 axis, Uint8 *buf, int32 length, int32 *length_read, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	bool rtn;
	rtn = g_serial->read(axis, buf, length,length_read, com_type, stationId);

	if (rtn == true)
	{
		return Rt_Success;
	}
	else
	{
		return Net_Rt_SerialPort_Err;
	}
}

int16 GTSD_CMD_ST_WriteSerialPort(int16 axis, Uint8 *buf, int32 length, int32 *length_written, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	bool rtn;
	rtn = g_serial->write(axis, buf, length, length_written, com_type, stationId);

	if (rtn == true)
	{
		return Rt_Success;
	}
	else
	{
		return Net_Rt_SerialPort_Err;
	}
}

int16 GTSD_CMD_ST_ReadLogAlarmCode(int16 axis, Uint32* alarmCode, Uint16& lenth, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[64] = { 0 };											//通信数组
	int16 Axis = axis;													//轴号
	int16 dsp_comAddr = DSPA_COMADDR;									//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = RD_ALARM_LOG_CODE_COM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	Uint16 cur_index = 0;
	Uint16 bit32lenth = 0;
	int16 dsp_comNum = 40;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		//第一个数据是当前的index
		cur_index = dspdata[0];
		//假如读到的报警index是-1，那么就说明还没有报警，正常的时候会将他们初始化为0
		if (((int16)cur_index) <0)
		{
			cur_index = 0;
		}

		//第二个数据是32bit数据的长度
		bit32lenth = dspdata[1];

		//根据当前index和总的数据长度进行排序，将最近的报警排在最前面。
		for (int16 i = 0; i < lenth; ++i)
		{
			alarmCode[i] = (((dspdata[(2 + 2 * cur_index)] << 16) & 0xffff0000) | (dspdata[(2 + 2 * cur_index + 1)]&0x0000ffff));
			cur_index++;
			if ((cur_index) >= bit32lenth)
			{
				cur_index = 0;
			}
		}

		return Rt_Success;
	}
}

int16 GTSD_CMD_ST_ReadLogAlarmTimes(int16 axis, Uint16* alarmTimes, Uint16& lenth, int16 com_type /*= GTSD_COM_TYPE_NET*/, int16 stationId /*= 0xf0*/)
{
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}

	int16 dspdata[64] = { 0 };											//通信数组
	int16 Axis = axis;													//轴号
	int16 dsp_comAddr = DSPA_COMADDR;									//地址
	if (Axis > 1)
	{
		dsp_comAddr = (int16)(DSPB_COMADDR);
		Axis = Axis % 2;											//如果是2，那么是另一个dsp，但是需将轴号更改为0
	}

	int16 cmd_id = RD_ALARM_LOG_TIMES_COM;							//cmd id
	dspdata[0] = GetCmdIDAndAxisNum(cmd_id, Axis);					//合并轴号和命令id
	dspdata[1] = GTSD_DSP_READ;												//读命令	
	dspdata[2] = 0;												//返回值																		

	int16 dsp_comNum = 40;
	int rtn = g_AbsCom->GTSD_Com_Processor_handler(com_type, GTSD_COM_MODE_READ, dsp_comAddr, dspdata, dsp_comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return rtn;
	}
	else
	{
		for (int16 i = 0; i < lenth; ++i)
		{
			alarmTimes[i] = dspdata[i];
		}
		return Rt_Success;
	}
}
