//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	mutimedia timer high resolution 		 					//
//	file				:	mt_timer.h													//
//	Description			:	high resolution interface mutimedia timer 					//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef		__GTSD_MT_TIMER__
#define		__GTSD_MT_TIMER__

class CMtimer
{
public:
	CMtimer(void);
	~CMtimer(void);
public:
	//定义精度

	UINT		TimerAccuracy;
	UINT		TimerID;
	void Start_MutimediaTimer(void);
	void Stop_MutimediaTimer(void);
};
void CALLBACK  MutimediaTimer(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

#endif
