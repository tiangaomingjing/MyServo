//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Communicaiton cmd layer Define		 						//
//	file				:	ST_GTSD_Cmd.h												//
//	Description			:	use for cmd define											//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef			__GTSD_ST_CMD__
#define			__GTSD_ST_CMD__	
//////////////////////////////////////////////////////////////////////////

#include "AbsCom.h"
#include "ServoDriverComDll.h"
const int32						GTSD_DSP_WRITE			= 0;					  //dsp定义的读写	
const int32						GTSD_DSP_READ			= 1;

//地址为dsp与fpga之间通信的fpga地址，不同的dsp对应不同的fpga地址。
const int32						DSPA_COMADDR			= 0x0400;				 //地址为short地址
const int32						DSPB_COMADDR			= 0x8400;

const int32						FPGA_DSPA_BASEADDR		= 0;
const int32						FPGA_DSPB_BASEADDR		= 0x8000;

const int32						COM_AXIS_MAX			= 4;					//轴最大个数为4	

const int32						GTSD_DSP_A				= 0;
const int32						GTSD_DSP_B				= 1;

const int32						PLOTWAVE_DSPA_NUM_ADDR	= 0x0006;
const int32						PLOTWAVE_DSPB_NUM_ADDR	= 0x8006;

const int32						PLOTWAVE_DSPA_DATA_ADDR = 0x0600;//0x0800//
const int32						PLOTWAVE_DSPB_DATA_ADDR = 0x8600;//0x8800//

const int32						FPGA_VERSION = 0x0000;

const int32						PLOTWAVE_GET_DATA_MAX_ONCE = 480;


//本地函数声明
static int16 GetCmdIDAndAxisNum(short cmdID, short motorNum);

//////////////////////////////////////////////////////////////////////////

int16 GTSD_CMD_ST_OPEN(int16 comType = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_CLOSE(int16 comType = GTSD_COM_TYPE_NET);


int16 GTSD_CMD_ST_SetServoOn(int16 axis, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetServoOff(int16 axis, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetServoState(int16 axis, SERVO_STATE* serv, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetServoTaskMode(int16 axis, int16 mode, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetServoTaskMode(int16 axis, SERVO_MODE* mode, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Write16BitByAdr(int16 axis, int16 ofst, int16 value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Read16BitByAdr(int16 axis, int16 ofst, int16* value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Write32BitByAdr(int16 axis, int16 ofst, int32 value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Read32BitByAdr(int16 axis, int16 ofst, int32* value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Write64BitByAdr(int16 axis, int16 ofst, int64 value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Read64BitByAdr(int16 axis, int16 ofst, int64* value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetIdRef(int16 axis, double id_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetIdRef(int16 axis, ID_STATE* id_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetIqRef(int16 axis, double iq_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetIqRef(int16 axis, IQ_STATE* iq_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetSpdRef(int16 axis, double spd_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetSpdRef(int16 axis, SPD_STATE* spd_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetUdRef(int16 axis, double ud_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetUdRef(int16 axis, UD_STATE* ud_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetUqRef(int16 axis, double uq_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetUqRef(int16 axis, UQ_STATE* uq_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetUaRef(int16 axis, double ua_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetUaRef(int16 axis, UA_STATE* ua_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetUbRef(int16 axis, double ub_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetUbRef(int16 axis, UB_STATE* ub_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetUcRef(int16 axis, double uc_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetUcRef(int16 axis, UC_STATE* uc_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetPosAdjRef(int16 axis, double PosAdj_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetPosAdjRef(int16 axis, POS_ADJ_STATE* pos_adj_ref, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetPosRef(int16 axis, int32 Pos_ref, int16 com_type = GTSD_COM_TYPE_NET);

/*
int16 GTSD_CMD_ST_SetFndCurr(int16 axis, int16 fnd, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetFndCurr(int16 axis, int16* fnd, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetFnqCurr(int16 axis, int16 fnq, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetFnqCurr(int16 axis, int16* fnq, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetTidCurr(int16 axis, int32 tid, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetTidCurr(int16 axis, int32* tid, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetTiqCurr(int16 axis, int32 tiq, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetTiqCurr(int16 axis, int32* tiq, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetFnVel(int16 axis, int16 fnvel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetFnVel(int16 axis, int16* fnvel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetTiVel(int16 axis, double tivel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetTiVel(int16 axis, double* tivel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetAbsLimVel(int16 axis, int16 abslimvel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetAbsLimVel(int16 axis, int16* abslimvel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetPosLimVel(int16 axis, int16 poslimvel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetPosLimVel(int16 axis, int16* poslimvel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetNegLimVel(int16 axis, int16 neglimvel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetNegLimVel(int16 axis, int16* neglimvel, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetFnPos(int16 axis, double fnpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetFnPos(int16 axis, double* fnpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetAbsLimPos(int16 axis, int16 abslimpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetAbsLimPos(int16 axis, int16* abslimpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetPosLimPos(int16 axis, int16 poslimpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetPosLimPos(int16 axis, int16* poslimpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetNegLimPos(int16 axis, int16 neglimpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetNegLimPos(int16 axis, int16* neglimpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetKgaFfdPos(int16 axis, int16 kgaffdpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetKgaFfdPos(int16 axis, int16* kgaffdpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_SetKgvFfdPos(int16 axis, int16 kgvffdpos, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetKgvFfdPos(int16 axis, int16* kgvffdpos, int16 com_type = GTSD_COM_TYPE_NET);
*/
int16 GTSD_CMD_ST_Set16bitFPGAByAddr(int16 dsp_number, int16 com_addr, int16 value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Get16bitFPGAByAddr(int16 dsp_number, int16 com_addr, int16* pvalue, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Set32bitFPGAByAddr(int16 dsp_number, int16 com_addr, int32 value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Get32bitFPGAByAddr(int16 dsp_number, int16 com_addr, int32* pvalue, int16 com_type = GTSD_COM_TYPE_NET);

int16 GTSD_CMD_ST_SetWaveBuf(int16 dsp_number, WAVE_BUF_PRM wave, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_GetWaveBuf(int16 dsp_number, tWaveBufCmd* ctrlword, int16 com_type = GTSD_COM_TYPE_NET);

int16 GTSD_CMD_ST_GetWaveData(int16 dsp_number, int16* read_num, int16** data, int16 com_type = GTSD_COM_TYPE_NET);

int16 GTSD_CMD_ST_ClearFpgaFifo(int16 dsp_number, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Fram_Write16BitByAdr(int16 axis, int16 ofst, int16 value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Fram_Read16BitByAdr(int16 axis, int16 ofst, int16* value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Fram_Write32BitByAdr(int16 axis, int16 ofst, int32 value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Fram_Read32BitByAdr(int16 axis, int16 ofst, int32* value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Fram_Write64BitByAdr(int16 axis, int16 ofst, int64 value, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_Fram_Read64BitByAdr(int16 axis, int16 ofst, int64* value, int16 com_type = GTSD_COM_TYPE_NET);

int16 GTSD_CMD_ST_FlashWrite(int16 axis, INTEL_HEX_FRAME* packet, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_FlashRead(int16 axis, INTEL_HEX_FRAME* packet_w, INTEL_HEX_FRAME* packet_r, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_FlashErase(int16 axis, int16 blockNum, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_InterruptSwitch(int16 axis, int16 int_switch, int16 com_type = GTSD_COM_TYPE_NET);

int16 GTSD_CMD_ST_ProcessorGeneralFunc(int16 axis, GENERALFUNCTION* gefunc, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_ResetSystem(int16 axis, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_CheckResetFinish(int16 axis, bool& flag_finish, int16 com_type = GTSD_COM_TYPE_NET);

int16 GTSD_CMD_ST_ReadProcessorVersion(int16 axis, Uint16& ver, int16 com_type = GTSD_COM_TYPE_NET);
int16 GTSD_CMD_ST_ReadFirmwareVersion(int16 axis, Uint16& ver, int16 com_type = GTSD_COM_TYPE_NET);

int16 GTSD_CMD_ST_ClrAlarm(int16 axis,int16 com_type = GTSD_COM_TYPE_NET);
#endif  