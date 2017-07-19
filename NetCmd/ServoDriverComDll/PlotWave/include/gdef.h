//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	global var define 		 									//
//	file				:	gdef.h														//
//	Description			:	global var define include test var 							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef		__GTSD_PLOT_WAVE_GDEF__
#define		__GTSD_PLOT_WAVE_GDEF__

//////////////////////////////////////////////////////////////////////////
#include "Basetype_def.h"
#include "ST_GTSD_Cmd.h"
//////////////////////////////////////////////////////////////////////////
//#define		TIME_TEST

#define		GETDATA_TYPE_THREAD
//////////////////////////////////////////////////////////////////////////

extern WAVE_BUF_PRM g_dspA_wave_prm;
extern WAVE_BUF_PRM g_dspB_wave_prm;

extern CRITICAL_SECTION	g_cs_dspA;
extern CRITICAL_SECTION	g_cs_dspB;

#ifdef TIME_TEST
//////////////////////////////////////////////////////////////////////////
//internal clock
extern LARGE_INTEGER	m_timeStartClock;
extern LARGE_INTEGER	m_timeFreq;
extern LARGE_INTEGER	m_timeNowClock;
extern double			m_fDelayTime;
extern double			m_time[100000];
extern int32			m_time_index;

//计算时间超过1ms的次数
extern int64			m_exceed_1ms_times;
//////////////////////////////////////////////////////////////////////////
#endif

#endif
