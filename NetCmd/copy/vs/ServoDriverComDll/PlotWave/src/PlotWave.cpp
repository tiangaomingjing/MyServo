//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	plot wave function 		 									//
//	file				:	PlotWave.cpp													//
//	Description			:	plot all the var in the arm  								//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <new>
using namespace std;
#include "gdef.h"
#include "mt_timer.h"
#include "PlotWave.h"
#include "cpu_timer.h"
#include "ServoDriverComDll.h"

CPlotWave* g_plotWave = NULL;

CPlotWave::CPlotWave(void)
{
	//定义对象
	m_cpu_timer = new CCpu_Timer;
	m_mt_timer	= new CMtimer;

	raw_Buffer_dspA[RAW_BUFFER_LENTH] = { 0 };
	raw_CacheBuffer_dspA[RAW_CACHE_BUFFER_LENTH] = { 0 };

	raw_Buffer_dspB[RAW_BUFFER_LENTH] = { 0 };
	raw_CacheBuffer_dspB[RAW_CACHE_BUFFER_LENTH] = { 0 };
	
	pw_RunFlag[(pw_MaxAxis >> 1)] = { false };

	cpu_timer_openFlag = false;
}

CPlotWave::~CPlotWave(void)
{
	if (m_cpu_timer!= NULL)
	{
		delete m_cpu_timer;
	}
	if (m_mt_timer!= NULL)
	{
		delete m_mt_timer;
	}
}
//初始化dspAwave
int16 CPlotWave::InitDspAWaveVar(WAVE_BUF_PRM& wave)
{
	dataFrame_Lenth_dspA				= 0;	
	for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)										//计算整个数据帧的长度
	{
		waveLine_RdIndex_dspA[i]		= 0;
		waveLine_WrIndex_dspA[i]		= 0;
		waveLine_WrNumOnce_dspA[i]		= 0;
		waveLine_WrNumSum_dspA[i]		= 0;
		dataFrame_Lenth_dspA			+= wave.inf[i].bytes;
	}
	dataFrame_Lenth_dspA				= (dataFrame_Lenth_dspA >> 1);					//转化到int16的个数

	//初始化变量
	recv_SyncFrame_Num_dspA				= 0;
	recv_DataFrame_Num_dspA				= 0;
	waveLine_Index_dspA					= 0;
	syncFrame_Flag_dspA					= 0;
	m_dspA_line_number					= wave.cmd.bit.NUM;
	raw_Data_GoBack_Flag_dspA			= false;
	raw_Data_GoBack_Aux_Flag_dspA		= false;
	raw_Data_ParseLeftNumber_dspA		= 0;
	raw_Data_ParseIndex_dspA			= 0;
	raw_Data_Index_dspA					= 0;

#ifdef TIME_TEST
	value_exceed = 0;
#endif

	return 0;
}
//初始化dspBwave
int16 CPlotWave::InitDspBWaveVar(WAVE_BUF_PRM& wave)
{
	dataFrame_Lenth_dspB = 0;
	for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)										//计算整个数据帧的长度
	{
		waveLine_RdIndex_dspB[i]		= 0;
		waveLine_WrIndex_dspB[i]		= 0;
		waveLine_WrNumOnce_dspB[i]		= 0;
		waveLine_WrNumSum_dspB[i]		= 0;
		dataFrame_Lenth_dspB			+= wave.inf[i].bytes;
	}
	dataFrame_Lenth_dspB				= (dataFrame_Lenth_dspB >> 1);					//转化到int16的个数

	//初始化变量
	recv_SyncFrame_Num_dspB				= 0;
	recv_DataFrame_Num_dspB				= 0;
	waveLine_Index_dspB					= 0;
	syncFrame_Flag_dspB					= 0;
	m_dspB_line_number					= wave.cmd.bit.NUM;
	raw_Data_GoBack_Flag_dspB			= false;
	raw_Data_GoBack_Aux_Flag_dspB		= false;
	raw_Data_ParseLeftNumber_dspB		= 0;
	raw_Data_ParseIndex_dspB			= 0;
	raw_Data_Index_dspB					= 0;

	return 0;
}
//关闭dspAwave
int16 CPlotWave::CloseDspAWave(WAVE_BUF_PRM& wave)
{
	//停止dspA向FPGA的FIFO中写数据
	wave.cmd.bit.ENP = 0;
	GTSD_CMD_SetWaveBuf(GTSD_DSP_A, wave);

	//停止对DSPA的操作
	g_dspA_wave_prm.cmd.bit.ENP = 0;

#ifdef GETDATA_TYPE_THREAD
	if (cpu_timer_openFlag == true)
	{
		m_cpu_timer->StopCpuTimer();
		cpu_timer_openFlag = false;
	}
	
#else
	m_mt_timer->Stop_MutimediaTimer();
#endif
	return 0;
}
//关闭dspBwave
int16 CPlotWave::CloseDspBWave(WAVE_BUF_PRM& wave)
{
	//停止dspB
	wave.cmd.bit.ENP = 0;
	GTSD_CMD_SetWaveBuf(GTSD_DSP_B, wave);

	//停止对DSPB的操作
	g_dspB_wave_prm.cmd.bit.ENP = 0;

#ifdef GETDATA_TYPE_THREAD
	if (cpu_timer_openFlag == true)
	{
		m_cpu_timer->StopCpuTimer();
		cpu_timer_openFlag = false;
	}
#else
	m_mt_timer->Stop_MutimediaTimer();
#endif
	return 0;
}

int16 CPlotWave::ParseDspAPlotWaveData(WAVE_BUF_PRM& wave, int32 number)
{
	//先清空计数器
	for (int16 w = 0; w < wave.cmd.bit.NUM; ++w)
	{
		waveLine_WrNumOnce_dspA[w] = 0;
	}
	//过圈的处理
	if (raw_Data_GoBack_Flag_dspA == true)
	{
		//先将遗留的数据和新的这一组数据放到新的cache中处理
		int32 leftnumber = RAW_BUFFER_LENTH - raw_Data_ParseIndex_dspA;
		memcpy_s(raw_CacheBuffer_dspA, leftnumber*sizeof(int16), &(raw_Buffer_dspA[raw_Data_ParseIndex_dspA]), leftnumber*sizeof(int16));
		memcpy_s(&(raw_CacheBuffer_dspA[leftnumber]), (number + raw_Data_ParseLeftNumber_dspA - leftnumber)*sizeof(int16), &(raw_Buffer_dspA[0]), (number + raw_Data_ParseLeftNumber_dspA - leftnumber)*sizeof(int16));
		int16 j;
		for (j = 0; j < (number + raw_Data_ParseLeftNumber_dspA);)
		{
			if (((number + raw_Data_ParseLeftNumber_dspA - j) <= dataFrame_Lenth_dspA) && (waveLine_Index_dspA == wave.cmd.bit.NUM))
			{
				//复位该信号
				raw_Data_ParseLeftNumber_dspA = number + raw_Data_ParseLeftNumber_dspA - j;
				//分两种情况，如果刚刚过圈一点的时候可能因为要解析到一帧结束而并没有解析过圈
				if (j<leftnumber)
				{
					//记录该位置的值用于下次的i的起始值
					raw_Data_ParseIndex_dspA += j;
					raw_Data_GoBack_Aux_Flag_dspA = true;
				}
				else
				{
					raw_Data_ParseIndex_dspA = j - leftnumber;
					raw_Data_GoBack_Aux_Flag_dspA = false;
				}
				
				//曲线的index复位
				waveLine_Index_dspA = 0;
				//跳出该循环
				break;
			}
			//假如找到同步帧的低位，连续又找到同步帧的高位
			if ((raw_CacheBuffer_dspA[j] == (int16)FRAME_KEYWORD_LOW) && (raw_CacheBuffer_dspA[j + 1] == (int16)FRAME_KEYWORD_HIGH))
			{
				//说明是同步帧，标志置位
				syncFrame_Flag_dspA = 1;
				j += 2;

				//如果只有一条16bit的曲线时需要做特殊处理，假如正好在最后有同步帧，那么j直接跳出了，
				//不会更新g_pcbuffer_dspA_parseindex和g_pcbuffer_dspA_ParseLeftNumber
				if (j == (number + raw_Data_ParseLeftNumber_dspA))
				{
					raw_Data_ParseIndex_dspA			= j - leftnumber;
					raw_Data_ParseLeftNumber_dspA		= 0;
				} 
			
				//记录收到的同步帧个数
				recv_SyncFrame_Num_dspA++;
				//这里因为是同步帧所以不需要写入数据，跳出此次循环
				continue;
			}
			//reset waveLine_Index_dspA
			if (waveLine_Index_dspA >= wave.cmd.bit.NUM)
			{
				waveLine_Index_dspA = 0;
				//完整帧的个数加1
				recv_DataFrame_Num_dspA++;
			}
			if (syncFrame_Flag_dspA == 1)//假如出现一个同步帧
			{
				if (waveLine_Index_dspA != 0)
				{
					//补齐缺少的值
					for (int32 p = waveLine_Index_dspA; p < wave.cmd.bit.NUM; ++p)
					{
						(waveLine_WrIndex_dspA[p])++;

						//该组数据实际写入曲线中的个数
						waveLine_WrNumOnce_dspA[p]++;
						//写指针复位
						if ((waveLine_WrIndex_dspA[p]) >= WAVE_LINE_BUFFER_LENTH)
						{
							(waveLine_WrIndex_dspA[p]) = 0;
						}
					}
					//重新定位到0曲线位置
					waveLine_Index_dspA = 0;
				}
			}
			//将当前值作为数据写入,根据占据的字节数不同，从buffer中取得个数不同
			switch (wave.inf[waveLine_Index_dspA].bytes)
			{
			case 2:
			{
				waveLine_Buffer_dspA[waveLine_Index_dspA][(waveLine_WrIndex_dspA[waveLine_Index_dspA])++] = (double)raw_CacheBuffer_dspA[j];
				j++;
				break;
			}
			case 4:
			{
				waveLine_Buffer_dspA[waveLine_Index_dspA][(waveLine_WrIndex_dspA[waveLine_Index_dspA])++] = (double)((int32)(((raw_CacheBuffer_dspA[j] & 0x0000ffff) | ((raw_CacheBuffer_dspA[j + 1] << 16) & 0xffff0000))));
				j += 2;
				break;
			}
			case 8:
			{
				waveLine_Buffer_dspA[waveLine_Index_dspA][(waveLine_WrIndex_dspA[waveLine_Index_dspA])++] =
					(double)((int64)((((int64)(raw_CacheBuffer_dspA[j] & 0x0000ffff)) | ((((int64)raw_CacheBuffer_dspA[j + 1]) << 16) & 0xffff0000) | ((((int64)raw_CacheBuffer_dspA[j + 2]) << 32) & 0x0000ffff00000000) | ((((int64)raw_CacheBuffer_dspA[j + 3]) << 48) & 0xffff000000000000))));
				j += 4;
				break;
			}
			default:
				return -2;
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//test
#ifdef TIME_TEST
			if (waveLine_Buffer_dspA[waveLine_Index_dspA][(waveLine_WrIndex_dspA[waveLine_Index_dspA]) - 1] > 50000.0)
			{
				value_exceed++;
			}
#endif
			//////////////////////////////////////////////////////////////////////////

			//该组数据实际写入曲线中的个数
			waveLine_WrNumOnce_dspA[waveLine_Index_dspA]++;

			//写指针复位
			if ((waveLine_WrIndex_dspA[waveLine_Index_dspA]) >= WAVE_LINE_BUFFER_LENTH)
			{
				(waveLine_WrIndex_dspA[waveLine_Index_dspA]) = 0;
			}
			//曲线index顺序增加
			waveLine_Index_dspA++;

			//清空同步帧的标志
			syncFrame_Flag_dspA = 0;
		}
#ifdef TIME_TEST
		allnumber_test[allnumber_test_index] = waveLine_WrNumOnce_dspA[0];
		allnumber_test_index++;
		if (allnumber_test_index >= 10000)
			allnumber_test_index = 0;
#endif	
	}
	else
	{
		int32 i;
#ifdef TIME_TEST
		if (raw_Data_ParseIndex_dspA>raw_Data_Index_dspA)
		{
			i = raw_Data_ParseIndex_dspA;
		}
#endif
		//该次解析的范围是从上一次解析的最后到目前从fpga读取到的数据
		for (i = raw_Data_ParseIndex_dspA; i < (raw_Data_Index_dspA);)
		{
			//只要剩下的个数小于或者等于整个数据帧的长度并且曲线的index是最后一条曲线的index
			if (((raw_Data_Index_dspA - i) <= dataFrame_Lenth_dspA) && (waveLine_Index_dspA == wave.cmd.bit.NUM))
			{	
				raw_Data_ParseIndex_dspA		= i;								//记录该位置的值用于下次的i的起始值			
				waveLine_Index_dspA				= 0;								//曲线的index复位				
				raw_Data_ParseLeftNumber_dspA = raw_Data_Index_dspA - i;		//更新剩下的个数
				break;																//跳出该循环
			}
			//首先判断数据中是否有同步帧
			//假如找到同步帧的低位，连续又找到同步帧的高位
			if ((raw_Buffer_dspA[i] == (int16)FRAME_KEYWORD_LOW) && (raw_Buffer_dspA[i + 1] == (int16)FRAME_KEYWORD_HIGH))
			{
				syncFrame_Flag_dspA				= 1;								//说明是同步帧，标志置位
				i+= 2;																//更新i

				//如果只有一条16bit的曲线时需要做特殊处理，假如正好在最后有同步帧，那么j直接跳出了，
				//不会更新g_pcbuffer_dspA_parseindex和g_pcbuffer_dspA_ParseLeftNumber
				if (i == (raw_Data_Index_dspA))
				{
					raw_Data_ParseIndex_dspA		= raw_Data_Index_dspA;
					raw_Data_ParseLeftNumber_dspA = 0;
				}

				recv_SyncFrame_Num_dspA++;											//记录收到的同步帧个数
				continue;															//这里因为是同步帧所以不需要写入数据，跳出此次循环，继续循环
			}
			//复位曲线条数index
			if (waveLine_Index_dspA >= wave.cmd.bit.NUM)							
			{
				waveLine_Index_dspA = 0;			
				recv_DataFrame_Num_dspA++;												//完整帧的个数加1
			}
	
			if (syncFrame_Flag_dspA == 1)//假如出现一个同步帧		
			{
				if (waveLine_Index_dspA != 0)
				{
					//补齐缺少的值
					for (int32 p = waveLine_Index_dspA; p < wave.cmd.bit.NUM; ++p)
					{
						(waveLine_WrIndex_dspA[p])++;
						waveLine_WrNumOnce_dspA[p]++;								//该组数据实际写入曲线中的个数
						if ((waveLine_WrIndex_dspA[p]) >= WAVE_LINE_BUFFER_LENTH)
						{
							(waveLine_WrIndex_dspA[p]) = 0;					//写指针复位
						}
					}
					waveLine_Index_dspA = 0;											//重置曲线条数index					
				}
			}
			//将当前值作为数据写入,根据占据的字节数不同，从buffer中取得个数不同
			switch (wave.inf[waveLine_Index_dspA].bytes)
			{
				case 2:
				{
					waveLine_Buffer_dspA[waveLine_Index_dspA][(waveLine_WrIndex_dspA[waveLine_Index_dspA])++] = (double)raw_Buffer_dspA[i];
					i++;
					break;
				}
				case 4:
				{
					waveLine_Buffer_dspA[waveLine_Index_dspA][(waveLine_WrIndex_dspA[waveLine_Index_dspA])++] = (double)((int32)((raw_Buffer_dspA[i] & 0x0000ffff) | ((raw_Buffer_dspA[i + 1] << 16) & 0xffff0000)));
					i += 2;
					break;
				}
				case 8:
				{
					waveLine_Buffer_dspA[waveLine_Index_dspA][(waveLine_WrIndex_dspA[waveLine_Index_dspA])++] =
						(double)((int64)(((int64)(raw_Buffer_dspA[i] & 0x0000ffff) | ((((int64)raw_Buffer_dspA[i + 1]) << 16) & 0xffff0000) | ((((int64)raw_Buffer_dspA[i + 2]) << 32) & 0x0000ffff00000000) | ((((int64)raw_Buffer_dspA[i + 3]) << 48) & 0xffff000000000000))));
					i += 4;
					break;
				}
				default:
					return -2;
					break;
			}
			//////////////////////////////////////////////////////////////////////////
			//test

#ifdef TIME_TEST
			if (waveLine_Buffer_dspA[waveLine_Index_dspA][(waveLine_WrIndex_dspA[waveLine_Index_dspA]) - 1] > 50000.0)
			{
				value_exceed++;
			}
			if (value_exceed != 0)
			{
				value_exceed++;
				if (value_exceed == 3000)
				{
					value_exceed = 0;
				}
			}
#endif
/**/
			//////////////////////////////////////////////////////////////////////////

			//该组数据实际写入曲线中的个数
			waveLine_WrNumOnce_dspA[waveLine_Index_dspA]++;

			//写指针复位
			if ((waveLine_WrIndex_dspA[waveLine_Index_dspA]) >= WAVE_LINE_BUFFER_LENTH)
			{
				(waveLine_WrIndex_dspA[waveLine_Index_dspA]) = 0;
			}
			//曲线index顺序增加
			waveLine_Index_dspA++;

			//清空同步帧的标志
			syncFrame_Flag_dspA = 0;
		}

	}
	return 0;
}
int16 CPlotWave::ParseDspBPlotWaveData(WAVE_BUF_PRM& wave, int32 number)
{
	//先清空计数器
	for (int16 w = 0; w < wave.cmd.bit.NUM; ++w)
	{
		waveLine_WrNumOnce_dspB[w] = 0;
	}
	//过圈的处理
	if (raw_Data_GoBack_Flag_dspB == true)
	{
		//先将遗留的数据和新的这一组数据放到新的cache中处理
		int32 leftnumber = RAW_BUFFER_LENTH - raw_Data_ParseIndex_dspB;
		memcpy_s(raw_CacheBuffer_dspB, leftnumber*sizeof(int16), &(raw_Buffer_dspB[raw_Data_ParseIndex_dspB]), leftnumber*sizeof(int16));
		memcpy_s(&(raw_CacheBuffer_dspB[leftnumber]), (number + raw_Data_ParseLeftNumber_dspB - leftnumber)*sizeof(int16), &(raw_Buffer_dspB[0]), (number + raw_Data_ParseLeftNumber_dspB - leftnumber)*sizeof(int16));
		int16 j;
		for (j = 0; j < (number + raw_Data_ParseLeftNumber_dspB);)
		{
			if (((number + raw_Data_ParseLeftNumber_dspB - j) <= dataFrame_Lenth_dspB) && (waveLine_Index_dspB == wave.cmd.bit.NUM))
			{
				//复位该信号
				raw_Data_ParseLeftNumber_dspB = number + raw_Data_ParseLeftNumber_dspB - j;
				//分两种情况，如果刚刚过圈一点的时候可能因为要解析到一帧结束而并没有解析过圈
				if (j < leftnumber)
				{
					//记录该位置的值用于下次的i的起始值
					raw_Data_ParseIndex_dspB += j;
					raw_Data_GoBack_Aux_Flag_dspB = true;
				}
				else
				{
					raw_Data_ParseIndex_dspB = j - leftnumber;
					raw_Data_GoBack_Aux_Flag_dspB = false;
				}

				//曲线的index复位
				waveLine_Index_dspB = 0;
				//跳出该循环
				break;
			}
			//假如找到同步帧的低位，连续又找到同步帧的高位
			if ((raw_CacheBuffer_dspB[j] == (int16)FRAME_KEYWORD_LOW) && (raw_CacheBuffer_dspB[j + 1] == (int16)FRAME_KEYWORD_HIGH))
			{
				//说明是同步帧，标志置位
				syncFrame_Flag_dspB = 1;
				j += 2;
				//记录收到的同步帧个数
				recv_SyncFrame_Num_dspB++;
				//这里因为是同步帧所以不需要写入数据，跳出此次循环
				continue;
			}
			//reset g_dspB_wave_index
			if (waveLine_Index_dspB >= wave.cmd.bit.NUM)
			{
				waveLine_Index_dspB = 0;
				//完整帧的个数加1
				recv_DataFrame_Num_dspB++;
			}
			if (syncFrame_Flag_dspB == 1)//假如出现一个同步帧
			{
				if (waveLine_Index_dspB != 0)
				{
					//补齐缺少的值
					for (int32 p = waveLine_Index_dspB; p < wave.cmd.bit.NUM; ++p)
					{
						(waveLine_WrIndex_dspB[p])++;

						//该组数据实际写入曲线中的个数
						waveLine_WrNumOnce_dspB[p]++;
						//写指针复位
						if ((waveLine_WrIndex_dspB[p]) >= WAVE_LINE_BUFFER_LENTH)
						{
							(waveLine_WrIndex_dspB[p]) = 0;
						}
					}
					//重新定位到0曲线位置
					waveLine_Index_dspB = 0;
				}
			}
			//将当前值作为数据写入,根据占据的字节数不同，从buffer中取得个数不同
			switch (wave.inf[waveLine_Index_dspB].bytes)
			{
			case 2:
			{
				waveLine_Buffer_dspB[waveLine_Index_dspB][(waveLine_WrIndex_dspB[waveLine_Index_dspB])++] = (double)raw_CacheBuffer_dspB[j];
				j++;
				break;
			}
			case 4:
			{
				waveLine_Buffer_dspB[waveLine_Index_dspB][(waveLine_WrIndex_dspB[waveLine_Index_dspB])++] = (double)((int32)(((raw_CacheBuffer_dspB[j] & 0x0000ffff) | ((raw_CacheBuffer_dspB[j + 1] << 16) & 0xffff0000))));
				j += 2;
				break;
			}
			case 8:
			{
				waveLine_Buffer_dspB[waveLine_Index_dspB][(waveLine_WrIndex_dspB[waveLine_Index_dspB])++] =
					(double)((int64)((((int64)(raw_CacheBuffer_dspB[j] & 0x0000ffff)) | ((((int64)raw_CacheBuffer_dspB[j + 1]) << 16) & 0xffff0000) | ((((int64)raw_CacheBuffer_dspB[j + 2]) << 32) & 0x0000ffff00000000) | ((((int64)raw_CacheBuffer_dspB[j + 3]) << 48) & 0xffff000000000000))));
				j += 4;
				break;
			}
			default:
				return -2;
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//test
#ifdef TIME_TEST
			if (waveLine_Buffer_dspB[waveLine_Index_dspB][(waveLine_WrIndex_dspB[waveLine_Index_dspB]) - 1] > 50000.0)
			{
				value_exceed++;
			}
#endif
			/**/
			//////////////////////////////////////////////////////////////////////////

			//该组数据实际写入曲线中的个数
			waveLine_WrNumOnce_dspB[waveLine_Index_dspB]++;

			//写指针复位
			if ((waveLine_WrIndex_dspB[waveLine_Index_dspB]) >= WAVE_LINE_BUFFER_LENTH)
			{
				(waveLine_WrIndex_dspB[waveLine_Index_dspB]) = 0;
			}
			//曲线index顺序增加
			waveLine_Index_dspB++;

			//清空同步帧的标志
			syncFrame_Flag_dspB = 0;
		}
#ifdef TIME_TEST
		allnumber_test[allnumber_test_index] = waveLine_WrNumOnce_dspB[0];
		allnumber_test_index++;
		if (allnumber_test_index >= 10000)
			allnumber_test_index = 0;

#endif	
	}
	else
	{
		int32 i;
#ifdef TIME_TEST
		if (raw_Data_ParseIndex_dspB > raw_Data_Index_dspB)
		{
			i = raw_Data_ParseIndex_dspB;
		}
#endif
		//该次解析的范围是从上一次解析的最后到目前从fpga读取到的数据
		for (i = raw_Data_ParseIndex_dspB; i < (raw_Data_Index_dspB);)
		{
			//只要剩下的个数小于或者等于整个数据帧的长度并且曲线的index是最后一条曲线的index
			if (((raw_Data_Index_dspB - i) <= dataFrame_Lenth_dspB) && (waveLine_Index_dspB == wave.cmd.bit.NUM))
			{
				raw_Data_ParseIndex_dspB = i;								//记录该位置的值用于下次的i的起始值			
				waveLine_Index_dspB = 0;								//曲线的index复位				
				raw_Data_ParseLeftNumber_dspB = raw_Data_Index_dspB - i;		//更新剩下的个数
				break;																//跳出该循环
			}
			//首先判断数据中是否有同步帧
			//假如找到同步帧的低位，连续又找到同步帧的高位
			if ((raw_Buffer_dspB[i] == (int16)FRAME_KEYWORD_LOW) && (raw_Buffer_dspB[i + 1] == (int16)FRAME_KEYWORD_HIGH))
			{
				syncFrame_Flag_dspB = 1;								//说明是同步帧，标志置位
				i += 2;																//更新i
				recv_SyncFrame_Num_dspB++;											//记录收到的同步帧个数
				continue;															//这里因为是同步帧所以不需要写入数据，跳出此次循环，继续循环
			}
			//复位曲线条数index
			if (waveLine_Index_dspB >= wave.cmd.bit.NUM)
			{
				waveLine_Index_dspB = 0;
				recv_DataFrame_Num_dspB++;												//完整帧的个数加1
			}

			if (syncFrame_Flag_dspB == 1)//假如出现一个同步帧		
			{
				if (waveLine_Index_dspB != 0)
				{
					//补齐缺少的值
					for (int32 p = waveLine_Index_dspB; p < wave.cmd.bit.NUM; ++p)
					{
						(waveLine_WrIndex_dspB[p])++;
						waveLine_WrNumOnce_dspB[p]++;								//该组数据实际写入曲线中的个数
						if ((waveLine_WrIndex_dspB[p]) >= WAVE_LINE_BUFFER_LENTH)
						{
							(waveLine_WrIndex_dspB[p]) = 0;					//写指针复位
						}
					}
					waveLine_Index_dspB = 0;											//重置曲线条数index					
				}
			}
			//将当前值作为数据写入,根据占据的字节数不同，从buffer中取得个数不同
			switch (wave.inf[waveLine_Index_dspB].bytes)
			{
			case 2:
			{
				waveLine_Buffer_dspB[waveLine_Index_dspB][(waveLine_WrIndex_dspB[waveLine_Index_dspB])++] = (double)raw_Buffer_dspB[i];
				i++;
				break;
			}
			case 4:
			{
				waveLine_Buffer_dspB[waveLine_Index_dspB][(waveLine_WrIndex_dspB[waveLine_Index_dspB])++] = (double)((int32)((raw_Buffer_dspB[i] & 0x0000ffff) | ((raw_Buffer_dspB[i + 1] << 16) & 0xffff0000)));
				i += 2;
				break;
			}
			case 8:
			{
				waveLine_Buffer_dspB[waveLine_Index_dspB][(waveLine_WrIndex_dspB[waveLine_Index_dspB])++] =
					(double)((int64)(((int64)(raw_Buffer_dspB[i] & 0x0000ffff) | ((((int64)raw_Buffer_dspB[i + 1]) << 16) & 0xffff0000) | ((((int64)raw_Buffer_dspB[i + 2]) << 32) & 0x0000ffff00000000) | ((((int64)raw_Buffer_dspB[i + 3]) << 48) & 0xffff000000000000))));
				i += 4;
				break;
			}
			default:
				return -2;
				break;
			}
			//////////////////////////////////////////////////////////////////////////
			//test

#ifdef TIME_TEST
			if (waveLine_Buffer_dspB[waveLine_Index_dspB][(waveLine_WrIndex_dspB[waveLine_Index_dspB]) - 1] > 50000.0)
			{
				value_exceed++;
			}
			if (value_exceed != 0)
			{
				value_exceed++;
				if (value_exceed == 3000)
				{
					value_exceed = 0;
				}
			}
#endif
			/**/
			//////////////////////////////////////////////////////////////////////////

			//该组数据实际写入曲线中的个数
			waveLine_WrNumOnce_dspB[waveLine_Index_dspB]++;

			//写指针复位
			if ((waveLine_WrIndex_dspB[waveLine_Index_dspB]) >= WAVE_LINE_BUFFER_LENTH)
			{
				(waveLine_WrIndex_dspB[waveLine_Index_dspB]) = 0;
			}
			//曲线index顺序增加
			waveLine_Index_dspB++;

			//清空同步帧的标志
			syncFrame_Flag_dspB = 0;
		}

	}
	return 0;
}
//从fpga的buffer中读取数据，根据dsp不同要从不通的地址读取，并且放到不同的pcbuffer中
int16 CPlotWave::ReadPlotWaveDataFromFPGA(int16 dsp_number, WAVE_BUF_PRM& wave)
{
	int16	read_num;
	int16*	data = NULL;
	
	//获取数据
	GTSD_CMD_GetWaveData(dsp_number,&read_num,&data);
#ifdef TIME_TEST
	if (dsp_number == GTSD_DSP_A)
	{
		readnumber_testA[readnumber_test_indexA++] = read_num;
		if (readnumber_test_indexA >= 10000)
		{
			readnumber_test_indexA = 0;
		}
		if (read_num>6000)
		{
			readnumber_test_indexA = readnumber_test_indexA;
		}
		
	}
	else
	{
		readnumber_testB[readnumber_test_indexB++] = read_num;
		if (readnumber_test_indexB >= 1000)
		{
			readnumber_test_indexB = 0;
		}
	}
#endif

	//将数据放到pcbuffer中
	if (read_num != 0)
	{
		if (dsp_number == GTSD_DSP_A)
		{
			//将数据放到dspa的pcbuffer中
			int32 tmp;
			tmp = raw_Data_Index_dspA + read_num;
			//如果已经过圈，需要复制两部分，将溢出部分复制到最前面
			if (tmp >=(RAW_BUFFER_LENTH))
			{
				errno_t err = memcpy_s(&(raw_Buffer_dspA[raw_Data_Index_dspA]), (RAW_BUFFER_LENTH - raw_Data_Index_dspA)*sizeof(int16), data, (RAW_BUFFER_LENTH - raw_Data_Index_dspA)*sizeof(int16));
				if (err != 0)
				{
					err = 0;
				}
				err = memcpy_s(&(raw_Buffer_dspA[0]), (tmp - RAW_BUFFER_LENTH)*sizeof(int16), &(data[(RAW_BUFFER_LENTH - raw_Data_Index_dspA)]), (tmp - RAW_BUFFER_LENTH)*sizeof(int16));
				if (err != 0)
				{
					err = 0;
				}
				raw_Data_GoBack_Flag_dspA = true;		//过圈标志
			}
			else
			{
				errno_t err = memcpy_s(&(raw_Buffer_dspA[raw_Data_Index_dspA]), read_num*sizeof(int16), data, read_num*sizeof(int16));
				if (err!=0)
				{
					err = 0;
				}
				raw_Data_GoBack_Flag_dspA = false;		//过圈标志
				if (raw_Data_GoBack_Aux_Flag_dspA == true)
				{
					raw_Data_GoBack_Flag_dspA = true;
				}
			}
			
			//更新index
			raw_Data_Index_dspA += read_num;

			if (raw_Data_Index_dspA > (RAW_BUFFER_LENTH-1))
			{
				raw_Data_Index_dspA = raw_Data_Index_dspA - (RAW_BUFFER_LENTH);

#ifdef TIME_TEST
				overflow_flag++;
#endif
/**/
			}
			//开始解析数据到每条曲线的buffer中,一次读到多少就解析多少
		
			//解析数据到最后一个完整的帧，剩下的留到下一次解析
			ParseDspAPlotWaveData(wave, read_num);

			EnterCriticalSection(&g_cs_dspA);
			for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)
			{
				//buffer中实际有的数据个数，应该每条曲线都是一样的才对
				waveLine_WrNumSum_dspA[i] += waveLine_WrNumOnce_dspA[i];
			}
	
			LeaveCriticalSection(&g_cs_dspA);


		}
		else if (dsp_number == GTSD_DSP_B)
		{
			//将数据放到dspa的pcbuffer中
			int32 tmp;
			tmp = raw_Data_Index_dspB + read_num;
			//如果已经过圈，需要复制两部分，将溢出部分复制到最前面
			if (tmp >= (RAW_BUFFER_LENTH))
			{
				errno_t err = memcpy_s(&(raw_Buffer_dspB[raw_Data_Index_dspB]), (RAW_BUFFER_LENTH - raw_Data_Index_dspB)*sizeof(int16), data, (RAW_BUFFER_LENTH - raw_Data_Index_dspB)*sizeof(int16));
				if (err != 0)
				{
					err = 0;
				}
				err = memcpy_s(&(raw_Buffer_dspB[0]), (tmp - RAW_BUFFER_LENTH)*sizeof(int16), &(data[(RAW_BUFFER_LENTH - raw_Data_Index_dspB)]), (tmp - RAW_BUFFER_LENTH)*sizeof(int16));
				if (err != 0)
				{
					err = 0;
				}
				raw_Data_GoBack_Flag_dspB = true;		//过圈标志
			}
			else
			{
				errno_t err = memcpy_s(&(raw_Buffer_dspB[raw_Data_Index_dspB]), read_num*sizeof(int16), data, read_num*sizeof(int16));
				if (err != 0)
				{
					err = 0;
				}
				raw_Data_GoBack_Flag_dspB = false;		//过圈标志
				if (raw_Data_GoBack_Aux_Flag_dspB == true)
				{
					raw_Data_GoBack_Flag_dspB = true;
				}
			}

			//更新index
			raw_Data_Index_dspB += read_num;

			if (raw_Data_Index_dspB > (RAW_BUFFER_LENTH - 1))
			{
				raw_Data_Index_dspB = raw_Data_Index_dspB - (RAW_BUFFER_LENTH);

#ifdef TIME_TEST
				overflow_flag++;
#endif
				/**/
			}
		
			//解析数据到最后一个完整的帧，剩下的留到下一次解析
			ParseDspBPlotWaveData(wave, read_num);
				
			EnterCriticalSection(&g_cs_dspB);
			for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)
			{
				//buffer中实际有的数据个数，应该每条曲线都是一样的才对
				waveLine_WrNumSum_dspB[i] += waveLine_WrNumOnce_dspB[i];
			}
			LeaveCriticalSection(&g_cs_dspB);	
		}
		else
		{
			return -1;
		}
	}
	return 0;
}
//number实际取到的数据，数据的指针
int16 CPlotWave::PW_PcGetDspAWaveData(double** data, int32& number)
{
	//因为每条线中的数据都是一样的，同步的，所以这里只判断一条线的个数即可
	int32 realnumber;
	//EnterCriticalSection(&g_cs_dspA);
	realnumber = waveLine_WrNumSum_dspA[0];
	//LeaveCriticalSection(&g_cs_dspA);

	if (realnumber == 0)
	{
		*data = NULL;
		number = 0;
		return 0;
	}

	(*data)		= (double*)plotDataBuffer_dspA;
	for (int16 i = 0; i < m_dspA_line_number; ++i)
	{
		int32 tmp;
		int32 left;
		tmp = waveLine_RdIndex_dspA[i] + realnumber;
		if (tmp >= WAVE_LINE_BUFFER_LENTH)
		{
			left = tmp - WAVE_LINE_BUFFER_LENTH;
			memcpy_s(&(plotDataBuffer_dspA[i][0]), (WAVE_LINE_BUFFER_LENTH - waveLine_RdIndex_dspA[i])*sizeof(double), &(waveLine_Buffer_dspA[i][waveLine_RdIndex_dspA[i]]), (WAVE_LINE_BUFFER_LENTH - waveLine_RdIndex_dspA[i])*sizeof(double));
			memcpy_s((&(plotDataBuffer_dspA[i][(WAVE_LINE_BUFFER_LENTH - waveLine_RdIndex_dspA[i])])), left*sizeof(double), &(waveLine_Buffer_dspA[i][0]), left*sizeof(double));
		}
		else
		{
			memcpy_s(&(plotDataBuffer_dspA[i][0]), realnumber * sizeof(double), &(waveLine_Buffer_dspA[i][waveLine_RdIndex_dspA[i]]), realnumber * sizeof(double));
		}
		//读计数值增加
		(waveLine_RdIndex_dspA[i]) += realnumber;
		//假如读的数据计数值超过了曲线buffer的总长度，环回到开头
		if (waveLine_RdIndex_dspA[i] >= WAVE_LINE_BUFFER_LENTH)
		{
			waveLine_RdIndex_dspA[i] = waveLine_RdIndex_dspA[i] - WAVE_LINE_BUFFER_LENTH;
		}
		EnterCriticalSection(&g_cs_dspA);
		//buffer中实际有的数据个数，应该每条曲线都是一样的才对
		waveLine_WrNumSum_dspA[i] -= realnumber;
		LeaveCriticalSection(&g_cs_dspA); 

	}
#ifdef TIME_TEST

	readpointerA[pointerindexA] = waveLine_RdIndex_dspA[0];
	writepointerA[pointerindexA] = waveLine_WrIndex_dspA[0];
	if (readpointerA[pointerindexA] > writepointerA[pointerindexA])
	{
		pointerindexA = pointerindexA;
	}
	pointerindexA++;
	if (pointerindexA >= 1000)
	{
		pointerindexA = 0;
	}

#endif	
	//返回读取的个数
	number = realnumber;
	return 0;
}
int16 CPlotWave::PW_PcGetDspBWaveData(double** data, int32& number)
{
	//因为每条线中的数据都是一样的，同步的，所以这里只判断一条线的个数即可
	int32 realnumber;
	//EnterCriticalSection(&g_cs_dspB);
	realnumber = waveLine_WrNumSum_dspB[0];
	//LeaveCriticalSection(&g_cs_dspB);

	if (realnumber == 0)
	{
		*data = NULL;
		number = 0;
		return 0;
	}

	(*data) = (double*)plotDataBuffer_dspB;
	for (int16 i = 0; i < m_dspB_line_number; ++i)
	{
		int32 tmp;
		int32 left;
		tmp = waveLine_RdIndex_dspB[i] + realnumber;
		if (tmp >= WAVE_LINE_BUFFER_LENTH)
		{
			left = tmp - WAVE_LINE_BUFFER_LENTH;
			memcpy_s(&(plotDataBuffer_dspB[i][0]), (WAVE_LINE_BUFFER_LENTH - waveLine_RdIndex_dspB[i])*sizeof(double), &(waveLine_Buffer_dspB[i][waveLine_RdIndex_dspB[i]]), (WAVE_LINE_BUFFER_LENTH - waveLine_RdIndex_dspB[i])*sizeof(double));
			memcpy_s((&(plotDataBuffer_dspB[i][(WAVE_LINE_BUFFER_LENTH - waveLine_RdIndex_dspB[i])])), left*sizeof(double), &(waveLine_Buffer_dspB[i][0]), left*sizeof(double));
		}
		else
		{
			memcpy_s(&(plotDataBuffer_dspB[i][0]), realnumber * sizeof(double), &(waveLine_Buffer_dspB[i][waveLine_RdIndex_dspB[i]]), realnumber * sizeof(double));
		}
		//读计数值增加
		(waveLine_RdIndex_dspB[i]) += realnumber;
		//假如读的数据计数值超过了曲线buffer的总长度，环回到开头
		if (waveLine_RdIndex_dspB[i] >= WAVE_LINE_BUFFER_LENTH)
		{
			waveLine_RdIndex_dspB[i] = waveLine_RdIndex_dspB[i] - WAVE_LINE_BUFFER_LENTH;
		}
		EnterCriticalSection(&g_cs_dspB);
		//buffer中实际有的数据个数，应该每条曲线都是一样的才对
		waveLine_WrNumSum_dspB[i] -= realnumber;
		LeaveCriticalSection(&g_cs_dspB);

	}
#ifdef TIME_TEST

	readpointerB[pointerindexB] = waveLine_RdIndex_dspB[0];
	writepointerB[pointerindexB] = waveLine_WrIndex_dspB[0];
	if (readpointerB[pointerindexB] > writepointerB[pointerindexB])
	{
		pointerindexB = pointerindexB;
	}
	pointerindexB++;
	if (pointerindexB >= 1000)
	{
		pointerindexB = 0;
	}

#endif	
	//返回读取的个数
	number = realnumber;
	return 0;
}
int16 CPlotWave::PW_StartDspAPlot(WAVE_BUF_PRM& wave)
{
	//根据wave中的表述 初始化dspA，并且会某种定时器去获取dspA的数据。
	InitDspAWaveVar(wave);

	//读空FPGA的fifo
	int16 rtn;
	rtn = GTSD_CMD_ClearFpgaFifo(GTSD_DSP_A);
	if (rtn!=0)
	{
		return -1;
	}

#ifdef GETDATA_TYPE_THREAD
	//创建线程读取fpga数据
	//createPlotWaveThread();
	if (cpu_timer_openFlag == false)
	{
		double tPeriod = 1.0;
		m_cpu_timer->StartCpuTimer(tPeriod);
		m_cpu_timer->createCpuTimerThread();
		cpu_timer_openFlag = true;
	}
#else
	m_mt_timer->Start_MutimediaTimer();
#endif

	//拷贝曲线配置到本地
	memcpy_s(&g_dspA_wave_prm, sizeof(WAVE_BUF_PRM), &wave, sizeof(WAVE_BUF_PRM));

	//将wave写入dspA，此时dspA开始产生数据
	GTSD_CMD_SetWaveBuf(GTSD_DSP_A,wave);
	return 0;
}
int16 CPlotWave::PW_StartDspBPlot(WAVE_BUF_PRM& wave)
{
	//根据wave中的表述 初始化dspB，并且会启动多媒体定时器去获取dspB的数据。
	InitDspBWaveVar(wave);

	//读空FPGA的fifo
	int16 rtn;
	rtn = GTSD_CMD_ClearFpgaFifo(GTSD_DSP_B);
	if (rtn != 0)
	{
		return -1;
	}

#ifdef GETDATA_TYPE_THREAD
	//创建线程读取fpga数据
	//createPlotWaveThread();
	if (cpu_timer_openFlag == false)
	{
		double tPeriod = 1.0;
		m_cpu_timer->StartCpuTimer(tPeriod);
		m_cpu_timer->createCpuTimerThread();
		cpu_timer_openFlag = true;
	}
#else
	m_mt_timer->Start_MutimediaTimer();
#endif

	memcpy_s(&g_dspB_wave_prm, sizeof(WAVE_BUF_PRM), &wave, sizeof(WAVE_BUF_PRM));

	//将wave写入dspB
	GTSD_CMD_SetWaveBuf(GTSD_DSP_B, wave);
	return 0;
}
int16 CPlotWave::PW_StopDspAPlot(WAVE_BUF_PRM& wave)
{
	//关闭数据通道
	CloseDspAWave(wave);
	return 0;
}
int16 CPlotWave::PW_StopDspBPlot(WAVE_BUF_PRM& wave)
{
	//关闭数据通道
	CloseDspBWave(wave);
	return 0;
}
