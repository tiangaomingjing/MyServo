//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	plot wave function 		 									//
//	file				:	PlotWave.h													//
//	Description			:	plot all the var in the arm  								//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef  __GTSD_PLOT_WAVE__
#define	 __GTSD_PLOT_WAVE__	

#include "Basetype_def.h"
#include "gdef.h"
#include "cpu_timer.h"
#include "mt_timer.h"
//////////////////////////////////////////////////////////////////////////

class CPlotWave
{
public:
	CPlotWave(void);
	~CPlotWave(void);
	CCpu_Timer*		m_cpu_timer;
	CMtimer*		m_mt_timer;

public:
	static const int32		RAW_BUFFER_LENTH				= 200000;		//PC读取FPGA缓存的buffer，没有分离曲线
	static const int32		WAVE_LINE_BUFFER_LENTH			= 200000;		//曲线buffuer
	static const int32		READ_LENTH_ONCE					= 10000;		//pc一次读取的double数据的个数，每一条线的数据个数,根据FPGAbuffer的大小而定义
	static const int32		RAW_CACHE_BUFFER_LENTH			= 10000;		//raw data buffer lenth

	
	static const int32		FRAME_KEYWORD_LOW				= 0xaaaa;		//同步帧的定义
	static const int32		FRAME_KEYWORD_HIGH				= 0x5555;
	static const int16		pw_MaxAxis						= 4;			//最大轴数定义

	//////////////////////////////////////////////////////////////////////////
	int16		raw_Buffer_dspA[RAW_BUFFER_LENTH];					//定义两个buffer，分别放置DSPA的raw数据和DSPB的raw数据
	int16		raw_Buffer_dspB[RAW_BUFFER_LENTH];

	int16		raw_CacheBuffer_dspA[RAW_CACHE_BUFFER_LENTH];		//定义两个cache，用于处理上面的buffer环回的时候的raw数据
	int16		raw_CacheBuffer_dspB[RAW_CACHE_BUFFER_LENTH];

	int32		raw_Data_Index_dspA;								//从FPGA的FIFO中读取的raw数据的index
	int32		raw_Data_Index_dspB;

	int32		raw_Data_ParseIndex_dspA;							//解析从FPGA的FIFO中读取的raw数据的index，
	int32		raw_Data_ParseIndex_dspB;							//记录实际解析的index，因为每次解析都解析到帧结束的地方。

	int32		raw_Data_ParseLeftNumber_dspA;						//解析处理环回部分的时候需要记忆尾部还剩下多少需要解析
	int32		raw_Data_ParseLeftNumber_dspB;
	
	bool		raw_Data_GoBack_Flag_dspA;							//raw数据buffer环回的标志
	bool		raw_Data_GoBack_Flag_dspB;

	bool	    raw_Data_GoBack_Aux_Flag_dspA;						//过圈标志辅助标志，因为有时候并不是真正的过圈，因为要找到一帧的结束
	bool	    raw_Data_GoBack_Aux_Flag_dspB;

	int32		dataFrame_Lenth_dspA;								//数据帧的总长度（int16的长度）
	int32		dataFrame_Lenth_dspB;								//数据帧的长度记录

	int32		waveLine_Index_dspA;								//用来循环要读取的曲线的个数，例如要看5条曲线，每一帧就是5条曲线的数据组成的结构
	int32		waveLine_Index_dspB;								//用来循环要读取的曲线的个数，例如要看5条曲线，每一帧就是5条曲线的数据组成的结构

	int16		syncFrame_Flag_dspA;								//找到帧的标志
	int16		syncFrame_Flag_dspB;								//找到帧的标志

	
	int64		recv_SyncFrame_Num_dspA;							//检测到的同步帧的个数//测试记录使用
	int64		recv_SyncFrame_Num_dspB;							//检测到的同步帧的个数

	int64		recv_DataFrame_Num_dspA;							//检测到的数据帧的个数
	int64		recv_DataFrame_Num_dspB;							//检测到的数据帧的个数

	
	double		waveLine_Buffer_dspA[MAX_WAVE_PLOT_NUM][WAVE_LINE_BUFFER_LENTH];		//用于存放解析完成的数据
	double		waveLine_Buffer_dspB[MAX_WAVE_PLOT_NUM][WAVE_LINE_BUFFER_LENTH];


	int32		waveLine_RdIndex_dspA[MAX_WAVE_PLOT_NUM];			//曲线数据的读index
	int32		waveLine_RdIndex_dspB[MAX_WAVE_PLOT_NUM];

	int32		waveLine_WrIndex_dspA[MAX_WAVE_PLOT_NUM];			//曲线数据的写index
	int32		waveLine_WrIndex_dspB[MAX_WAVE_PLOT_NUM];
	
	int32		waveLine_WrNumOnce_dspA[MAX_WAVE_PLOT_NUM];			//一次写入的个数
	int32		waveLine_WrNumOnce_dspB[MAX_WAVE_PLOT_NUM];
	
	int32		waveLine_WrNumSum_dspA[MAX_WAVE_PLOT_NUM];			//总的个数，累加每一次写入的个数
	int32		waveLine_WrNumSum_dspB[MAX_WAVE_PLOT_NUM];

	int16		m_dspA_line_number;									//读取的曲线的个数，暂存
	int16		m_dspB_line_number;									//读取的曲线的个数，暂存
	
	double		plotDataBuffer_dspA[MAX_WAVE_PLOT_NUM][READ_LENTH_ONCE];//用于存放上层读取的数据
	double		plotDataBuffer_dspB[MAX_WAVE_PLOT_NUM][READ_LENTH_ONCE];

	bool		pw_RunFlag[(pw_MaxAxis >> 1)];						

	int16 InitDspAWaveVar(WAVE_BUF_PRM& wave);
	int16 InitDspBWaveVar(WAVE_BUF_PRM& wave);
	int16 CloseDspAWave(WAVE_BUF_PRM& wave);
	int16 CloseDspBWave(WAVE_BUF_PRM& wave);
	int16 ReadPlotWaveDataFromFPGA(int16 dsp_number, WAVE_BUF_PRM& wave);
	int16 ParseDspAPlotWaveData(WAVE_BUF_PRM& wave, int32 number);
	int16 ParseDspBPlotWaveData(WAVE_BUF_PRM& wave, int32 number);

	//启动停止DSPA的画图
	int16 PW_StartDspAPlot(WAVE_BUF_PRM& wave);
	int16 PW_StopDspAPlot(WAVE_BUF_PRM& wave);

	//从DSPA或者是DSPB获取数据
	int16 PW_PcGetDspAWaveData(double** data, int32& number);
	int16 PW_PcGetDspBWaveData(double** data, int32& number);

	//启动停止DSPB的画图
	int16 PW_StartDspBPlot(WAVE_BUF_PRM& wave);
	int16 PW_StopDspBPlot(WAVE_BUF_PRM& wave);

	bool cpu_timer_openFlag;


#ifdef  TIME_TEST

	int16 readnumber_testA[10000];
	int16 readnumber_test_indexA;
	int16 readnumber_testB[1000];
	int16 readnumber_test_indexB;

	int32 readpointerA[1000];
	int32 writepointerA[1000];
	int16 pointerindexA;
	int16 readpointerB[1000];
	int16 writepointerB[1000];
	int16 pointerindexB;

	int32 value_exceed;
	int32 overflow_flag;
	int16 allnumber_test[10000];
	int32 allnumber_test_index;
#endif
};

extern CPlotWave* g_plotWave;

#endif


