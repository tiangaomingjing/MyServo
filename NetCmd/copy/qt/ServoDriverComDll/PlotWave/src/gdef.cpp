//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	global var define 		 									//
//	file				:	gdef.CPP														//
//	Description			:	global var define include test var 							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "gdef.h"

WAVE_BUF_PRM g_dspA_wave_prm;
WAVE_BUF_PRM g_dspB_wave_prm;

//定义关键区，用于在不同的线程中都去操作数据个数
CRITICAL_SECTION	g_cs_dspA;
CRITICAL_SECTION	g_cs_dspB;

#ifdef TIME_TEST
//////////////////////////////////////////////////////////////////////////
//internal clock
LARGE_INTEGER	m_timeStartClock;
LARGE_INTEGER	m_timeFreq;
LARGE_INTEGER	m_timeNowClock;
double			m_fDelayTime;
double			m_time[100000];
int32			m_time_index = 0;

//计算时间超过1ms的次数
int64			m_exceed_1ms_times = 0;
//////////////////////////////////////////////////////////////////////////
#endif


