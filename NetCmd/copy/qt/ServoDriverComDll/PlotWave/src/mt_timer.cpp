//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	mutimedia timer high resolution 		 					//
//	file				:	mt_timer.cpp													//
//	Description			:	high resolution interface mutimedia timer 					//
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
#include "mt_timer.h"
#include "PlotWave.h"

//增加多媒体定时器的支持
#include <Mmsystem.h>
#pragma comment(lib, "winmm.lib")

const int32  MMTIMER_READ_TIMES = 2; //读取次数

CMtimer::CMtimer(void)
{
	TimerAccuracy = 0;
	TimerID = 0;
}

CMtimer::~CMtimer(void)
{

}

void CMtimer::Start_MutimediaTimer()
{
	//初始化cs
	InitializeCriticalSection(&g_cs_dspA);
	//初始化cs
	InitializeCriticalSection(&g_cs_dspB);

#ifdef TIME_TEST
	QueryPerformanceFrequency(&m_timeFreq);
#endif

	TIMECAPS timecaps;

	int TimerResolution;

	//从系统获得关于定时器服务能力的信息，
	//分辨率不能超出系统许可值（1到16毫秒）

	if (timeGetDevCaps(&timecaps,sizeof(TIMECAPS))==TIMERR_NOERROR)
		TimerAccuracy=min(max(timecaps.wPeriodMin,
		1),timecaps.wPeriodMax);

	timeBeginPeriod(TimerAccuracy);

	//设置定时器分辨率`

	TimerResolution = 1; //设置定时间隔为1毫秒

	//产生间隔1毫秒，周期执行的定时器事件；启动定时器

	TimerID = timeSetEvent(TimerResolution,TimerAccuracy,(LPTIMECALLBACK)MutimediaTimer,0,TIME_PERIODIC);
}

void CMtimer::Stop_MutimediaTimer()
{
	timeKillEvent(TimerID);
	timeEndPeriod(TimerAccuracy);
	
	Sleep(100);

	//删除关键区
	DeleteCriticalSection(&g_cs_dspA);
	DeleteCriticalSection(&g_cs_dspB);
}

/**/
void CALLBACK  MutimediaTimer(UINT wTimerID, UINT msg,DWORD dwUser, DWORD dw1, DWORD dw2)
{
#ifdef TIME_TEST
	QueryPerformanceCounter(&m_timeStartClock);
#endif
	if (g_dspA_wave_prm.cmd.bit.ENP || g_dspB_wave_prm.cmd.bit.ENP)
	{
		for (int16 i = 0; i < (int16)MMTIMER_READ_TIMES; ++i)
		{
			//假如dspA画图使能
			if (g_dspA_wave_prm.cmd.bit.ENP)
			{
				g_plotWave->ReadPlotWaveDataFromFPGA(GTSD_DSP_A, g_dspA_wave_prm, g_plotWave->m_comType);
			}
			//假如dspB画图使能
			if (g_dspB_wave_prm.cmd.bit.ENP)
			{
				g_plotWave->ReadPlotWaveDataFromFPGA(GTSD_DSP_B, g_dspB_wave_prm, g_plotWave->m_comType);
			}
		}
	}
	
#ifdef TIME_TEST
	QueryPerformanceCounter(&m_timeNowClock);
	m_fDelayTime = (double)(m_timeNowClock.QuadPart - m_timeStartClock.QuadPart) * 1000 / (double)m_timeFreq.QuadPart;
	m_time[m_time_index++] = m_fDelayTime;
	if (m_time_index == 100000)
	{
		m_time_index = 0;
	}
	if (m_fDelayTime >= 50.0)
	{
		m_exceed_1ms_times++;
	}
#endif
}
