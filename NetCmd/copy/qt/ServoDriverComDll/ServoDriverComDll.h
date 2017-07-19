//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Communicaiton cmd layer Define		 						//
//	file				:	ServoDriverComDll.h											//
//	Description			:	use for cmd define											//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef			__GTSD_SERVO_DRIVER_COM_DLL__
#define			__GTSD_SERVO_DRIVER_COM_DLL__	



// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SERVODRIVERCOMDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// SERVODRIVERCOMDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef SERVODRIVERCOMDLL_EXPORTS
#define SERVODRIVERCOMDLL_API __declspec(dllexport)
#else
#define SERVODRIVERCOMDLL_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
//命令返回值

//23---说明arm超时没有响应
//25---说明网络通信不正常
//27---说明上一条指令没有完成，这时可忽略通信错误计数
//////////////////////////////////////////////////////////////////////////
#include "Basetype_def.h"
#include <iostream>
#include <string>

using namespace std;
//////////////////////////////////////////////////////////////////////////
//命令返回值
//0---right command
//23---说明arm超时没有响应
//25---说明网络通信不正常
//27---说明上一条指令没有完成，这时可忽略通信错误计数
typedef enum com_error
{
	COM_OK,
	COM_ARM_OUT_TIME = 23,
	COM_NET_ERROR = 25,
	COM_NOT_FINISH = 27
}COM_ERROR;

//////////////////////////////////////////////////////////////////////////
const int32						MAX_WAVE_PLOT_NUM = 20;

/************************************************************************/
/* 定义通信类型                                                        */
/************************************************************************/
typedef enum com_type
{
	GTSD_COM_TYPE_NET,			//裸机网口
	GTSD_COM_TYPE_RNNET,		//等环网	
	GTSD_COM_TYPE_TCPIP,		//TCP/IP协议网口
	GTSD_COM_TYPE_USB2UART,		//usb转串口
	GTSD_COM_TYPE_VIRTUAL,		//虚拟设备
	
	GTSD_COM_TYPE_MAX			//最大值
}COM_TYPE;

typedef enum
{
	SERVO_EN_COMM = 0,																// MpiServoEnable()	----------> ID|0
	SERVO_TASK_COMM = 1,															// MpiServoTaskMode() -----------> ID|1
	MIX_MODE_COMM = 2,																// MpiMixMode()
	WR_16BIT_COMM = 3,																// MpiWr16BitByAdr
	WR_32BIT_COMM = 4,																// MpiWr32BitByAdr
	WR_64BIT_COMM = 5,																// MpiWr64BitByAdr
	ID_REF_WR_COMM = 6,																// id reference read/write
	IQ_REF_WR_COMM = 7,																// iq reference read/write
	SPD_REF_WR_COMM = 8,															// speed reference read/write
	PRE_REF_WR_COMM = 9,
	UD_REF_WR_COMM = 10,															// d axis voltage reference read/write
	UQ_REF_WR_COMM = 11,															// q axis voltage reference read/write
	UA_REF_WR_COMM = 12,															// a phase voltage reference read/write	
	UB_REF_WR_COMM = 13,															// b phase voltage reference read/write
	UC_REF_WR_COMM = 14,															// c phase voltage reference read/write
	POS_ADJ_IREF_COMM = 15,															// motor position adjust reference read/write
	POS_REF_WR_COMM = 16,															// position reference read/write 
	FND_CURR_WR_COMM = 17,															// d axis fn value set in current loop
	FNQ_CURR_WR_COMM = 18,															// q axis fn value set in current loop
	TID_CURR_WR_COMM = 19,															// d axis integrate time constant set	
	TIQ_CURR_WR_COMM = 20,															// q axis integrate time constant set	
	FN_VEL_WR_COMM = 21,															// velocity loop fn value set
	TI_VEL_WR_COMM = 22,															// velocity loop integrate time constant set
	ABS_LIM_VEL_WR_COMM = 23,														// absolute saturate limit rate of of velocity loop output 			
	POS_LIM_VEL_WR_COMM = 24,														// positive saturate limit rate of of velocity loop output 
	NEG_LIM_VEL_WR_COMM = 25,														// negative saturate limit rate of of velocity loop output
	FN_POS_WR_COMM = 26,															// fn value of position loop 
	ABS_LIM_POS_WR_COMM = 27,														// absolute limit rate value of position loop output
	POS_LIM_POS_WR_COMM = 28,														// positive limit rate value of position loop output
	NEG_LIM_POS_WR_COMM = 29,														// negative limit rate velue of position loop output
	KGA_FF_POS_WR_COMM = 30,														// kga value of accelerate feedforward 	
	KGV_FF_POS_WR_COMM = 31,														// kgv value of velocity feedforward
	TFA_FF_POS_WR_COMM = 32,														// tfa value of accelerate feedforward		
	TFV_FF_POS_WR_COMM = 33,														// tfv value of accelerate feedforward	
	WAVE_BUF_SET_WR_COMM = 34,														//设置曲线		
	WR_FRAM_16BIT_COMM = 35,														// MpiFramWr16BitByAdr
	WR_FRAM_32BIT_COMM = 36,														// MpiFramWr32BitByAdr
	WR_FRAM_64BIT_COMM = 37,														// MpiFramWr64BitByAdr
	WR_SPI_FLASH_COMM = 38,	                                                        // burn spi flash 
	SPI_ERASE_FLASH_COMM = 39,                                                      // erase all flash
	INTERRUPT_SWITCH_COMM = 40,														// interrupt enable and disable 
	GET_DRV_VAR_INFO_COMM = 41,                                                     // get var information according to the id 
	RESET_SYSTEM_COMM = 42,                                                         // reset system to reset isr routine
	CHECK_RESET_FINISH_COMM = 43,													// check reset finish
	RD_PROCESSOR_VER_COMM = 44,														// read processor version
	ALARM_CLEAR_COMM = 45,															// clear alarm
	RD_ALARM_LOG_CODE_COM = 46,                                                     // read alarm log code ,just log ten recently
	RD_ALARM_LOG_TIMES_COM = 47,                                                    // read different kind of alarm times 
	TUNNING_MESSAGE_MAX
}tTuningMessageId;


typedef struct servo_state
{
	int16 act_on_tmp;
	int16 act_on;
	int16 sec_on;
	int16 sof_st_on;
	int16 serv_ready;
}SERVO_STATE;

typedef struct servo_mode
{
	int16 usr_mode;
	int16 cmd_id;
	int16 curr_id;
	int16 curr_state;
}SERVO_MODE;

typedef struct id_state
{
	double chd_idr_tmp;
	double tsk_id_ref;
	double ctl_id_ref;
	double ctl_id;
}ID_STATE;

typedef struct iq_state
{
	double chd_iqr_tmp;
	double tsk_iq_ref;
	double ctl_iq_ref;
	double ctl_iq;
}IQ_STATE;

typedef struct spd_state
{
	double chd_spd_tmp;
	double tsk_spd_ref;
	double ctl_spd_ref;
	double ctl_spd_fd;
	double rsv_mot_spd;
}SPD_STATE;

typedef struct ud_state
{
	double chd_ud_tmp;
	double tsk_ud_ref;
	double cur_ud;
}UD_STATE;
typedef struct uq_state
{
	double chd_uq_tmp;
	double tsk_uq_ref;
	double cur_uq;
}UQ_STATE;

typedef struct ua_state
{
	double chd_ua_tmp;
	double tsk_ua_ref;
	double cur_ua;
}UA_STATE;
typedef struct ub_state
{
	double chd_ub_tmp;
	double tsk_ub_ref;
	double cur_ub;
}UB_STATE;
typedef struct uc_state
{
	double chd_uc_tmp;
	double tsk_uc_ref;
	double cur_uc;
}UC_STATE;
typedef struct pos_adj_state
{
	double chd_pos_adj_tmp;
	double tsk_pos_adj_ref;
	double ctl_id_ref;
	double ctl_id;
	double ctl_iq_ref;
	double ctl_iq;
	int16 rsv_pos_elec;
	int32 rsv_pos;
	int32 rsv_pos_in;
	int16 mfj_pos_adj_flag;

}POS_ADJ_STATE;


typedef struct		wave_buf_cmd
{
	Uint16		TIM : 8;												// sample times
	Uint16		NUM : 5;												// wave number
	Uint16		ENP : 1;												// wave plot enable bit
	Uint16		res : 2;												// rsvd
}WAVE_BUF_CMD;

typedef	union
{
	Uint16					all;
	WAVE_BUF_CMD			bit;
}tWaveBufCmd;

//-------------------------------------------------------------------------------------
typedef		struct	wave_info
{
	Uint16		bytes;																	// variable storage bytes
	Uint16		ofst;																	// variable storage offset	
}WAVE_INFO;



typedef		struct	wave_buf_prm
{
	tWaveBufCmd			cmd;															// wave buffer command information
	WAVE_INFO			inf[MAX_WAVE_PLOT_NUM];											// wave plot variable information
}WAVE_BUF_PRM;

#define INTEL_HEX_FRAME_DAT_LENTH       100


//////////////////////////////////////////////////////////////////////////

//define INTEL HEX struct 
typedef struct intel_hex_frame
{
	Uint16			lenth;									//长度   (unit :byte)   
	Uint32			addr;									//地址      
	Uint16			type;									//记录类型，0－数据，1－终止     
	int16			data[INTEL_HEX_FRAME_DAT_LENTH];		//数据     
	int16			checksum;								//校验和 
}INTEL_HEX_FRAME;


//////////////////////////////////////////////////////////////////////////


typedef struct generalFunction
{
	Uint16			mode;
	Uint16			cmd;
	Uint16			dataLenth;
	int16*			data;
}GENERALFUNCTION;

typedef enum
{
	UART_PROGRAM = 1,
	UART_ERASE = 4
}UARTBOOT;

//////////////////////////////////////////////////////////////////////////
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Open(int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Close(int16 com_type = GTSD_COM_TYPE_NET);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetServoOn(int16 axis, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetServoOff(int16 axis, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetServoState(int16 axis, SERVO_STATE* serv, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetServoTaskMode(int16 axis, int16 mode, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetServoTaskMode(int16 axis, SERVO_MODE* mode, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Write16BitByAdr(int16 axis, int16 ofst, int16 value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Read16BitByAdr(int16 axis, int16 ofst, int16* value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Write32BitByAdr(int16 axis, int16 ofst, int32 value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Read32BitByAdr(int16 axis, int16 ofst, int32* value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Write64BitByAdr(int16 axis, int16 ofst, int64 value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Read64BitByAdr(int16 axis, int16 ofst, int64* value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetIdRef(int16 axis, double id_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetIdRef(int16 axis, ID_STATE* id_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetIqRef(int16 axis, double iq_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetIqRef(int16 axis, IQ_STATE* iq_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetSpdRef(int16 axis, double spd_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetSpdRef(int16 axis, SPD_STATE* spd_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetUdRef(int16 axis, double ud_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetUdRef(int16 axis, UD_STATE* ud_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetUqRef(int16 axis, double uq_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetUqRef(int16 axis, UQ_STATE* uq_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetUaRef(int16 axis, double ua_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetUaRef(int16 axis, UA_STATE* ua_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetUbRef(int16 axis, double ub_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetUbRef(int16 axis, UB_STATE* ub_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetUcRef(int16 axis, double uc_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetUcRef(int16 axis, UC_STATE* uc_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetPosAdjRef(int16 axis, double PosAdj_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetPosAdjRef(int16 axis, POS_ADJ_STATE* pos_adj_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetPosRef(int16 axis, int32 Pos_ref, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
/*
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetFndCurr(int16 axis, int16 fnd, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetFndCurr(int16 axis, int16* fnd, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetFnqCurr(int16 axis, int16 fnq, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetFnqCurr(int16 axis, int16* fnq, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetTidCurr(int16 axis, int32 tid, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetTidCurr(int16 axis, int32* tid, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetTiqCurr(int16 axis, int32 tiq, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetTiqCurr(int16 axis, int32* tiq, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetFnVel(int16 axis, int16 fnvel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetFnVel(int16 axis, int16* fnvel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetTiVel(int16 axis, double tivel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetTiVel(int16 axis, double* tivel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetAbsLimVel(int16 axis, int16 abslimvel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetAbsLimVel(int16 axis, int16* abslimvel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetPosLimVel(int16 axis, int16 poslimvel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetPosLimVel(int16 axis, int16* poslimvel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetNegLimVel(int16 axis, int16 neglimvel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetNegLimVel(int16 axis, int16* neglimvel, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetFnPos(int16 axis, double fnpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetFnPos(int16 axis, double* fnpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetAbsLimPos(int16 axis, int16 abslimpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetAbsLimPos(int16 axis, int16* abslimpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetPosLimPos(int16 axis, int16 poslimpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetPosLimPos(int16 axis, int16* poslimpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetNegLimPos(int16 axis, int16 neglimpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetNegLimPos(int16 axis, int16* neglimpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetKgaFfdPos(int16 axis, int16 kgaffdpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetKgaFfdPos(int16 axis, int16* kgaffdpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetKgvFfdPos(int16 axis, int16 kgvffdpos, int16 com_type = GTSD_COM_TYPE_NET);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetKgvFfdPos(int16 axis, int16* kgvffdpos, int16 com_type = GTSD_COM_TYPE_NET);
*/
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Set16bitFPGAByAddr(int16 dsp_number, int16 com_addr, int16 value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Get16bitFPGAByAddr(int16 dsp_number, int16 com_addr, int16* pvalue, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Set32bitFPGAByAddr(int16 dsp_number, int16 com_addr, int32 value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Get32bitFPGAByAddr(int16 dsp_number, int16 com_addr, int32* pvalue, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_SetWaveBuf(int16 dsp_number, WAVE_BUF_PRM wave, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetWaveBuf(int16 dsp_number, tWaveBufCmd* ctrlword, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_GetWaveData(int16 dsp_number, int16* read_num, int16** data, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_ClearFpgaFifo(int16 dsp_number, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_Fram_Write16BitByAdr(int16 axis, int16 ofst, int16 value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Fram_Read16BitByAdr(int16 axis, int16 ofst, int16* value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Fram_Write32BitByAdr(int16 axis, int16 ofst, int32 value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Fram_Read32BitByAdr(int16 axis, int16 ofst, int32* value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Fram_Write64BitByAdr(int16 axis, int16 ofst, int64 value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Fram_Read64BitByAdr(int16 axis, int16 ofst, int64* value, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_StartPlot(int16& axis, WAVE_BUF_PRM& wave, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);						//启动ARM画图
SERVODRIVERCOMDLL_API int16 GTSD_CMD_StopPlot(int16& axis, WAVE_BUF_PRM& wave, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);							//停止ARM画图
SERVODRIVERCOMDLL_API int16 GTSD_CMD_PcGetWaveData(int16& axis, double** data, int32& number, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);			//从ARM获取数据
SERVODRIVERCOMDLL_API bool GTSD_CMD_CheckPlotState(int16& axis, int16 stationId = 0xf0);										//查询画图状态

typedef	void(*tpfUpdataProgressPt)(void*, int16*);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_ProcessorFlashHandler(int16 axis, string& filePath, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_FirmwareFlashHandler(int16 axis, string& filePath, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

//不需要导出的函数，内部使用
int16 GTSD_CMD_FlashWrite(int16 axis, INTEL_HEX_FRAME* packet, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
int16 GTSD_CMD_FlashRead(int16 axis, INTEL_HEX_FRAME* packet_w, INTEL_HEX_FRAME* packet_r, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
int16 GTSD_CMD_FlashErase(int16 axis, int16 blockNum,int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
int16 GTSD_CMD_InterruptSwitch(int16 axis,int16 int_switch, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);


SERVODRIVERCOMDLL_API int16 GTSD_CMD_ProcessorGeneralFunc(int16 axis, GENERALFUNCTION* gefunc, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_ResetSystem(int16 axis,int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_CheckResetFinish(int16 axis, bool& flag_finish, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_ReadProcessorVersion(int16 axis, Uint16& ver,int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_ReadFirmwareVersion(int16 axis, Uint16& ver, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_ClrAlarm(int16 axis,int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

//----------------------------------------------------------------------------------------------------------------
SERVODRIVERCOMDLL_API int16 GTSD_CMD_ReadLogAlarmCode(int16 axis, Uint32* alarmCode, Uint16& lenth, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

SERVODRIVERCOMDLL_API int16 GTSD_CMD_ReadLogAlarmTimes(int16 axis, Uint32* alarmTimes, Uint16& lenth, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

//-----------------------------------------------------------------------------------------------------

//use for uart boot .out 转化为 .ldr
SERVODRIVERCOMDLL_API int16 GTSD_CMD_Hex2Ldr(string& HexFile, string& LdrFile, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);

//可以使用hex文件或者是.out转化的ldr文件，ldr本身已经是二进制的文件了，hex还需要单独提取二进制信息。
SERVODRIVERCOMDLL_API int16 GTSD_CMD_OpenSerialPort(int16 axis, int32 baudRate, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_CloseSerialPort(int16 axis, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_ReadSerialPort(int16 axis, Uint8 *buf, int32 length, int32 *length_read, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);
SERVODRIVERCOMDLL_API int16 GTSD_CMD_WriteSerialPort(int16 axis, Uint8 *buf, int32 length, int32 *length_written, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);


SERVODRIVERCOMDLL_API int16 GTSD_CMD_ProcessorUartBootHandler(int16 axis, string& filePath, int32 baudRate, int16 cmd, string& inputKey, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type = GTSD_COM_TYPE_NET, int16 stationId = 0xf0);


extern SERVODRIVERCOMDLL_API int16 Cmd_PlotDataBuffer[10000];

#endif
