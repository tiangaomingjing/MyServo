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
	static const int32		RAW_BUFFER_LENTH				= 200000;		//PC��ȡFPGA�����buffer��û�з�������
	static const int32		WAVE_LINE_BUFFER_LENTH			= 200000;		//����buffuer
	static const int32		READ_LENTH_ONCE					= 10000;		//pcһ�ζ�ȡ��double���ݵĸ�����ÿһ���ߵ����ݸ���,����FPGAbuffer�Ĵ�С������
	static const int32		RAW_CACHE_BUFFER_LENTH			= 10000;		//raw data buffer lenth

	
	static const int32		FRAME_KEYWORD_LOW				= 0xaaaa;		//ͬ��֡�Ķ���
	static const int32		FRAME_KEYWORD_HIGH				= 0x5555;
	static const int16		pw_MaxAxis						= 4;			//�����������

	//////////////////////////////////////////////////////////////////////////
	int16		raw_Buffer_dspA[RAW_BUFFER_LENTH];					//��������buffer���ֱ����DSPA��raw���ݺ�DSPB��raw����
	int16		raw_Buffer_dspB[RAW_BUFFER_LENTH];

	int16		raw_CacheBuffer_dspA[RAW_CACHE_BUFFER_LENTH];		//��������cache�����ڴ��������buffer���ص�ʱ���raw����
	int16		raw_CacheBuffer_dspB[RAW_CACHE_BUFFER_LENTH];

	int32		raw_Data_Index_dspA;								//��FPGA��FIFO�ж�ȡ��raw���ݵ�index
	int32		raw_Data_Index_dspB;

	int32		raw_Data_ParseIndex_dspA;							//������FPGA��FIFO�ж�ȡ��raw���ݵ�index��
	int32		raw_Data_ParseIndex_dspB;							//��¼ʵ�ʽ�����index����Ϊÿ�ν�����������֡�����ĵط���

	int32		raw_Data_ParseLeftNumber_dspA;						//���������ز��ֵ�ʱ����Ҫ����β����ʣ�¶�����Ҫ����
	int32		raw_Data_ParseLeftNumber_dspB;
	
	bool		raw_Data_GoBack_Flag_dspA;							//raw����buffer���صı�־
	bool		raw_Data_GoBack_Flag_dspB;

	bool	    raw_Data_GoBack_Aux_Flag_dspA;						//��Ȧ��־������־����Ϊ��ʱ�򲢲��������Ĺ�Ȧ����ΪҪ�ҵ�һ֡�Ľ���
	bool	    raw_Data_GoBack_Aux_Flag_dspB;

	int32		dataFrame_Lenth_dspA;								//����֡���ܳ��ȣ�int16�ĳ��ȣ�
	int32		dataFrame_Lenth_dspB;								//����֡�ĳ��ȼ�¼

	int32		waveLine_Index_dspA;								//����ѭ��Ҫ��ȡ�����ߵĸ���������Ҫ��5�����ߣ�ÿһ֡����5�����ߵ�������ɵĽṹ
	int32		waveLine_Index_dspB;								//����ѭ��Ҫ��ȡ�����ߵĸ���������Ҫ��5�����ߣ�ÿһ֡����5�����ߵ�������ɵĽṹ

	int16		syncFrame_Flag_dspA;								//�ҵ�֡�ı�־
	int16		syncFrame_Flag_dspB;								//�ҵ�֡�ı�־

	
	int64		recv_SyncFrame_Num_dspA;							//��⵽��ͬ��֡�ĸ���//���Լ�¼ʹ��
	int64		recv_SyncFrame_Num_dspB;							//��⵽��ͬ��֡�ĸ���

	int64		recv_DataFrame_Num_dspA;							//��⵽������֡�ĸ���
	int64		recv_DataFrame_Num_dspB;							//��⵽������֡�ĸ���

	
	double		waveLine_Buffer_dspA[MAX_WAVE_PLOT_NUM][WAVE_LINE_BUFFER_LENTH];		//���ڴ�Ž�����ɵ�����
	double		waveLine_Buffer_dspB[MAX_WAVE_PLOT_NUM][WAVE_LINE_BUFFER_LENTH];


	int32		waveLine_RdIndex_dspA[MAX_WAVE_PLOT_NUM];			//�������ݵĶ�index
	int32		waveLine_RdIndex_dspB[MAX_WAVE_PLOT_NUM];

	int32		waveLine_WrIndex_dspA[MAX_WAVE_PLOT_NUM];			//�������ݵ�дindex
	int32		waveLine_WrIndex_dspB[MAX_WAVE_PLOT_NUM];
	
	int32		waveLine_WrNumOnce_dspA[MAX_WAVE_PLOT_NUM];			//һ��д��ĸ���
	int32		waveLine_WrNumOnce_dspB[MAX_WAVE_PLOT_NUM];
	
	int32		waveLine_WrNumSum_dspA[MAX_WAVE_PLOT_NUM];			//�ܵĸ������ۼ�ÿһ��д��ĸ���
	int32		waveLine_WrNumSum_dspB[MAX_WAVE_PLOT_NUM];

	int16		m_dspA_line_number;									//��ȡ�����ߵĸ������ݴ�
	int16		m_dspB_line_number;									//��ȡ�����ߵĸ������ݴ�
	
	double		plotDataBuffer_dspA[MAX_WAVE_PLOT_NUM][READ_LENTH_ONCE];//���ڴ���ϲ��ȡ������
	double		plotDataBuffer_dspB[MAX_WAVE_PLOT_NUM][READ_LENTH_ONCE];

	bool		pw_RunFlag[(pw_MaxAxis >> 1)];						

	int16 InitDspAWaveVar(WAVE_BUF_PRM& wave);
	int16 InitDspBWaveVar(WAVE_BUF_PRM& wave);
	int16 CloseDspAWave(WAVE_BUF_PRM& wave);
	int16 CloseDspBWave(WAVE_BUF_PRM& wave);
	int16 ReadPlotWaveDataFromFPGA(int16 dsp_number, WAVE_BUF_PRM& wave);
	int16 ParseDspAPlotWaveData(WAVE_BUF_PRM& wave, int32 number);
	int16 ParseDspBPlotWaveData(WAVE_BUF_PRM& wave, int32 number);

	//����ֹͣDSPA�Ļ�ͼ
	int16 PW_StartDspAPlot(WAVE_BUF_PRM& wave);
	int16 PW_StopDspAPlot(WAVE_BUF_PRM& wave);

	//��DSPA������DSPB��ȡ����
	int16 PW_PcGetDspAWaveData(double** data, int32& number);
	int16 PW_PcGetDspBWaveData(double** data, int32& number);

	//����ֹͣDSPB�Ļ�ͼ
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


