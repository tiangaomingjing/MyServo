//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	RN no timer						 							//
//	file				:	rn_notimer.h												//
//	Description			:	rn can sent data automatic ,has callback thread to receive  //
//							data  so do not need another thread							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/15	|	googoltech		|		2017 - 2020			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef  __GTSD_RN_NOTIMER__
#define	 __GTSD_RN_NOTIMER__	

class CRn_NoTimer
{
public:
	CRn_NoTimer(void);
	~CRn_NoTimer(void);
public:
	void StartNoTimer(int32 dspNo);
	void StopNoTimer(int32 dspNo);
	//人为分开两部分，来接收不同dsp的数据
	void GetPlotWaveDataFromRn(int16 dsp_Number);
};
#endif