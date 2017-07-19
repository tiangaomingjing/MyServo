//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	cpu timer high resolution 		 							//
//	file				:	cpu_timer.cpp												//
//	Description			:	high resolution interface cpu timer 						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <process.h>
#include "gdef.h"
#include "PlotWave.h"
#include "cpu_timer.h"


const int32  CPU_TIMER_READ_TIMES  =  1;

CCpu_Timer::CCpu_Timer(void)
{
	g_cpuTimerThread = NULL;
	g_CpuTimerFlag = false;
	g_MsTime = 0;
}
CCpu_Timer::~CCpu_Timer(void)
{

}
void CCpu_Timer::StartCpuTimer(double& MstimeInterval)
{
  //初始化cs
	InitializeCriticalSection(&g_cs_dspA);
	//初始化cs
	InitializeCriticalSection(&g_cs_dspB);
	//置位标志
	g_CpuTimerFlag	= true;
	//设置时间
	g_MsTime		= MstimeInterval;
}
void CCpu_Timer::StopCpuTimer(void)
{
	//标志清空
	g_CpuTimerFlag = false;
	Sleep(100);
	//删除关键区
	DeleteCriticalSection(&g_cs_dspA);
	DeleteCriticalSection(&g_cs_dspB);
}
void CCpu_Timer::createCpuTimerThread(void)
{
	g_cpuTimerThread	= (HANDLE)_beginthreadex(NULL, 0, &CpuTimerThread, this, 0, NULL);
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS/*HIGH_PRIORITY_CLASS*/);
	int32 processLevel	= GetPriorityClass(GetCurrentProcess());
	SetThreadPriority(g_cpuTimerThread, THREAD_PRIORITY_TIME_CRITICAL);
	int32 threadLevel	= GetThreadPriority(g_cpuTimerThread);
}
void CCpu_Timer::Cpu_GetData(void)
{
	if (g_dspA_wave_prm.cmd.bit.ENP || g_dspB_wave_prm.cmd.bit.ENP)
	{
		for (int16 i = 0; i < (int16)CPU_TIMER_READ_TIMES; ++i)
		{
			//假如dspA画图使能
			if (g_dspA_wave_prm.cmd.bit.ENP)
			{
				g_plotWave->ReadPlotWaveDataFromFPGA(GTSD_DSP_A, g_dspA_wave_prm,g_plotWave->m_comType);
			}
			//假如dspB画图使能
			if (g_dspB_wave_prm.cmd.bit.ENP)
			{
				g_plotWave->ReadPlotWaveDataFromFPGA(GTSD_DSP_B, g_dspB_wave_prm, g_plotWave->m_comType);
			}
		}
	}
}
unsigned __stdcall CpuTimerThread(void *para)
{
	//传递this指针到该函数
	CCpu_Timer *handler = (CCpu_Timer*)para;
	//获取频率
	if (QueryPerformanceFrequency(&(handler->freq)) == NULL)
	{
		return -1;
	}
	//时间乘以频率得到计数个数
	handler->tiCount = (int64)(handler->g_MsTime / 1000.0 * (handler->freq.QuadPart));
	
	//获取起始计数值
	QueryPerformanceCounter(&(handler->n1));
	do 
	{
		QueryPerformanceCounter(&(handler->n2));
		while ((handler->n2.QuadPart - handler->n1.QuadPart) >= handler->tiCount)
		{

#ifdef TIME_TEST
			QueryPerformanceCounter(&m_timeStartClock);
#endif		
			handler->n1.QuadPart += handler->tiCount;
			handler->Cpu_GetData();
			Sleep(0); 
#ifdef TIME_TEST
			QueryPerformanceCounter(&m_timeNowClock);
			m_fDelayTime = (double)(m_timeNowClock.QuadPart - m_timeStartClock.QuadPart) * 1000 / (double)(handler->freq.QuadPart);
			m_time[m_time_index++] = m_fDelayTime;
			if (m_time_index == 100000)
			{
				m_time_index = 0;
			}
			if (m_fDelayTime >= 32.0)
			{
				m_exceed_1ms_times++;
			}
#endif
		}

	
	} while (handler->g_CpuTimerFlag);
	return 0;
}


