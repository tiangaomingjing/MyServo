//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	cpu timer high resolution 		 							//
//	file				:	cpu_timer.h													//
//	Description			:	high resolution interface cpu timer 						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __GTSD_CPU_TIMER__
#define	 __GTSD_CPU_TIMER__	

class CCpu_Timer
{
public:
	CCpu_Timer(void);
	~CCpu_Timer(void);
public:	
	HANDLE	g_cpuTimerThread;

	LARGE_INTEGER   freq;
	int64			tiCount;
	LARGE_INTEGER	n1;
	LARGE_INTEGER   n2;

	//����һ����־
	bool			g_CpuTimerFlag;
	//�������õ�ms��
	double			g_MsTime;
	void StartCpuTimer(double& MstimeInterval);
	void StopCpuTimer(void);
	void createCpuTimerThread(void);
	void Cpu_GetData(void);
};
unsigned __stdcall CpuTimerThread(void *para);

#endif