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
	//�������
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
//��ʼ��dspAwave
int16 CPlotWave::InitDspAWaveVar(WAVE_BUF_PRM& wave)
{
	dataFrame_Lenth_dspA				= 0;	
	for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)										//������������֡�ĳ���
	{
		waveLine_RdIndex_dspA[i]		= 0;
		waveLine_WrIndex_dspA[i]		= 0;
		waveLine_WrNumOnce_dspA[i]		= 0;
		waveLine_WrNumSum_dspA[i]		= 0;
		dataFrame_Lenth_dspA			+= wave.inf[i].bytes;
	}
	dataFrame_Lenth_dspA				= (dataFrame_Lenth_dspA >> 1);					//ת����int16�ĸ���

	//��ʼ������
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
//��ʼ��dspBwave
int16 CPlotWave::InitDspBWaveVar(WAVE_BUF_PRM& wave)
{
	dataFrame_Lenth_dspB = 0;
	for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)										//������������֡�ĳ���
	{
		waveLine_RdIndex_dspB[i]		= 0;
		waveLine_WrIndex_dspB[i]		= 0;
		waveLine_WrNumOnce_dspB[i]		= 0;
		waveLine_WrNumSum_dspB[i]		= 0;
		dataFrame_Lenth_dspB			+= wave.inf[i].bytes;
	}
	dataFrame_Lenth_dspB				= (dataFrame_Lenth_dspB >> 1);					//ת����int16�ĸ���

	//��ʼ������
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
//�ر�dspAwave
int16 CPlotWave::CloseDspAWave(WAVE_BUF_PRM& wave)
{
	//ֹͣdspA��FPGA��FIFO��д����
	wave.cmd.bit.ENP = 0;
	GTSD_CMD_SetWaveBuf(GTSD_DSP_A, wave);

	//ֹͣ��DSPA�Ĳ���
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
//�ر�dspBwave
int16 CPlotWave::CloseDspBWave(WAVE_BUF_PRM& wave)
{
	//ֹͣdspB
	wave.cmd.bit.ENP = 0;
	GTSD_CMD_SetWaveBuf(GTSD_DSP_B, wave);

	//ֹͣ��DSPB�Ĳ���
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
	//����ռ�����
	for (int16 w = 0; w < wave.cmd.bit.NUM; ++w)
	{
		waveLine_WrNumOnce_dspA[w] = 0;
	}
	//��Ȧ�Ĵ���
	if (raw_Data_GoBack_Flag_dspA == true)
	{
		//�Ƚ����������ݺ��µ���һ�����ݷŵ��µ�cache�д���
		int32 leftnumber = RAW_BUFFER_LENTH - raw_Data_ParseIndex_dspA;
		memcpy_s(raw_CacheBuffer_dspA, leftnumber*sizeof(int16), &(raw_Buffer_dspA[raw_Data_ParseIndex_dspA]), leftnumber*sizeof(int16));
		memcpy_s(&(raw_CacheBuffer_dspA[leftnumber]), (number + raw_Data_ParseLeftNumber_dspA - leftnumber)*sizeof(int16), &(raw_Buffer_dspA[0]), (number + raw_Data_ParseLeftNumber_dspA - leftnumber)*sizeof(int16));
		int16 j;
		for (j = 0; j < (number + raw_Data_ParseLeftNumber_dspA);)
		{
			if (((number + raw_Data_ParseLeftNumber_dspA - j) <= dataFrame_Lenth_dspA) && (waveLine_Index_dspA == wave.cmd.bit.NUM))
			{
				//��λ���ź�
				raw_Data_ParseLeftNumber_dspA = number + raw_Data_ParseLeftNumber_dspA - j;
				//���������������ոչ�Ȧһ���ʱ�������ΪҪ������һ֡��������û�н�����Ȧ
				if (j<leftnumber)
				{
					//��¼��λ�õ�ֵ�����´ε�i����ʼֵ
					raw_Data_ParseIndex_dspA += j;
					raw_Data_GoBack_Aux_Flag_dspA = true;
				}
				else
				{
					raw_Data_ParseIndex_dspA = j - leftnumber;
					raw_Data_GoBack_Aux_Flag_dspA = false;
				}
				
				//���ߵ�index��λ
				waveLine_Index_dspA = 0;
				//������ѭ��
				break;
			}
			//�����ҵ�ͬ��֡�ĵ�λ���������ҵ�ͬ��֡�ĸ�λ
			if ((raw_CacheBuffer_dspA[j] == (int16)FRAME_KEYWORD_LOW) && (raw_CacheBuffer_dspA[j + 1] == (int16)FRAME_KEYWORD_HIGH))
			{
				//˵����ͬ��֡����־��λ
				syncFrame_Flag_dspA = 1;
				j += 2;

				//���ֻ��һ��16bit������ʱ��Ҫ�����⴦�����������������ͬ��֡����ôjֱ�������ˣ�
				//�������g_pcbuffer_dspA_parseindex��g_pcbuffer_dspA_ParseLeftNumber
				if (j == (number + raw_Data_ParseLeftNumber_dspA))
				{
					raw_Data_ParseIndex_dspA			= j - leftnumber;
					raw_Data_ParseLeftNumber_dspA		= 0;
				} 
			
				//��¼�յ���ͬ��֡����
				recv_SyncFrame_Num_dspA++;
				//������Ϊ��ͬ��֡���Բ���Ҫд�����ݣ������˴�ѭ��
				continue;
			}
			//reset waveLine_Index_dspA
			if (waveLine_Index_dspA >= wave.cmd.bit.NUM)
			{
				waveLine_Index_dspA = 0;
				//����֡�ĸ�����1
				recv_DataFrame_Num_dspA++;
			}
			if (syncFrame_Flag_dspA == 1)//�������һ��ͬ��֡
			{
				if (waveLine_Index_dspA != 0)
				{
					//����ȱ�ٵ�ֵ
					for (int32 p = waveLine_Index_dspA; p < wave.cmd.bit.NUM; ++p)
					{
						(waveLine_WrIndex_dspA[p])++;

						//��������ʵ��д�������еĸ���
						waveLine_WrNumOnce_dspA[p]++;
						//дָ�븴λ
						if ((waveLine_WrIndex_dspA[p]) >= WAVE_LINE_BUFFER_LENTH)
						{
							(waveLine_WrIndex_dspA[p]) = 0;
						}
					}
					//���¶�λ��0����λ��
					waveLine_Index_dspA = 0;
				}
			}
			//����ǰֵ��Ϊ����д��,����ռ�ݵ��ֽ�����ͬ����buffer��ȡ�ø�����ͬ
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

			//��������ʵ��д�������еĸ���
			waveLine_WrNumOnce_dspA[waveLine_Index_dspA]++;

			//дָ�븴λ
			if ((waveLine_WrIndex_dspA[waveLine_Index_dspA]) >= WAVE_LINE_BUFFER_LENTH)
			{
				(waveLine_WrIndex_dspA[waveLine_Index_dspA]) = 0;
			}
			//����index˳������
			waveLine_Index_dspA++;

			//���ͬ��֡�ı�־
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
		//�ôν����ķ�Χ�Ǵ���һ�ν��������Ŀǰ��fpga��ȡ��������
		for (i = raw_Data_ParseIndex_dspA; i < (raw_Data_Index_dspA);)
		{
			//ֻҪʣ�µĸ���С�ڻ��ߵ�����������֡�ĳ��Ȳ������ߵ�index�����һ�����ߵ�index
			if (((raw_Data_Index_dspA - i) <= dataFrame_Lenth_dspA) && (waveLine_Index_dspA == wave.cmd.bit.NUM))
			{	
				raw_Data_ParseIndex_dspA		= i;								//��¼��λ�õ�ֵ�����´ε�i����ʼֵ			
				waveLine_Index_dspA				= 0;								//���ߵ�index��λ				
				raw_Data_ParseLeftNumber_dspA = raw_Data_Index_dspA - i;		//����ʣ�µĸ���
				break;																//������ѭ��
			}
			//�����ж��������Ƿ���ͬ��֡
			//�����ҵ�ͬ��֡�ĵ�λ���������ҵ�ͬ��֡�ĸ�λ
			if ((raw_Buffer_dspA[i] == (int16)FRAME_KEYWORD_LOW) && (raw_Buffer_dspA[i + 1] == (int16)FRAME_KEYWORD_HIGH))
			{
				syncFrame_Flag_dspA				= 1;								//˵����ͬ��֡����־��λ
				i+= 2;																//����i

				//���ֻ��һ��16bit������ʱ��Ҫ�����⴦�����������������ͬ��֡����ôjֱ�������ˣ�
				//�������g_pcbuffer_dspA_parseindex��g_pcbuffer_dspA_ParseLeftNumber
				if (i == (raw_Data_Index_dspA))
				{
					raw_Data_ParseIndex_dspA		= raw_Data_Index_dspA;
					raw_Data_ParseLeftNumber_dspA = 0;
				}

				recv_SyncFrame_Num_dspA++;											//��¼�յ���ͬ��֡����
				continue;															//������Ϊ��ͬ��֡���Բ���Ҫд�����ݣ������˴�ѭ��������ѭ��
			}
			//��λ��������index
			if (waveLine_Index_dspA >= wave.cmd.bit.NUM)							
			{
				waveLine_Index_dspA = 0;			
				recv_DataFrame_Num_dspA++;												//����֡�ĸ�����1
			}
	
			if (syncFrame_Flag_dspA == 1)//�������һ��ͬ��֡		
			{
				if (waveLine_Index_dspA != 0)
				{
					//����ȱ�ٵ�ֵ
					for (int32 p = waveLine_Index_dspA; p < wave.cmd.bit.NUM; ++p)
					{
						(waveLine_WrIndex_dspA[p])++;
						waveLine_WrNumOnce_dspA[p]++;								//��������ʵ��д�������еĸ���
						if ((waveLine_WrIndex_dspA[p]) >= WAVE_LINE_BUFFER_LENTH)
						{
							(waveLine_WrIndex_dspA[p]) = 0;					//дָ�븴λ
						}
					}
					waveLine_Index_dspA = 0;											//������������index					
				}
			}
			//����ǰֵ��Ϊ����д��,����ռ�ݵ��ֽ�����ͬ����buffer��ȡ�ø�����ͬ
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

			//��������ʵ��д�������еĸ���
			waveLine_WrNumOnce_dspA[waveLine_Index_dspA]++;

			//дָ�븴λ
			if ((waveLine_WrIndex_dspA[waveLine_Index_dspA]) >= WAVE_LINE_BUFFER_LENTH)
			{
				(waveLine_WrIndex_dspA[waveLine_Index_dspA]) = 0;
			}
			//����index˳������
			waveLine_Index_dspA++;

			//���ͬ��֡�ı�־
			syncFrame_Flag_dspA = 0;
		}

	}
	return 0;
}
int16 CPlotWave::ParseDspBPlotWaveData(WAVE_BUF_PRM& wave, int32 number)
{
	//����ռ�����
	for (int16 w = 0; w < wave.cmd.bit.NUM; ++w)
	{
		waveLine_WrNumOnce_dspB[w] = 0;
	}
	//��Ȧ�Ĵ���
	if (raw_Data_GoBack_Flag_dspB == true)
	{
		//�Ƚ����������ݺ��µ���һ�����ݷŵ��µ�cache�д���
		int32 leftnumber = RAW_BUFFER_LENTH - raw_Data_ParseIndex_dspB;
		memcpy_s(raw_CacheBuffer_dspB, leftnumber*sizeof(int16), &(raw_Buffer_dspB[raw_Data_ParseIndex_dspB]), leftnumber*sizeof(int16));
		memcpy_s(&(raw_CacheBuffer_dspB[leftnumber]), (number + raw_Data_ParseLeftNumber_dspB - leftnumber)*sizeof(int16), &(raw_Buffer_dspB[0]), (number + raw_Data_ParseLeftNumber_dspB - leftnumber)*sizeof(int16));
		int16 j;
		for (j = 0; j < (number + raw_Data_ParseLeftNumber_dspB);)
		{
			if (((number + raw_Data_ParseLeftNumber_dspB - j) <= dataFrame_Lenth_dspB) && (waveLine_Index_dspB == wave.cmd.bit.NUM))
			{
				//��λ���ź�
				raw_Data_ParseLeftNumber_dspB = number + raw_Data_ParseLeftNumber_dspB - j;
				//���������������ոչ�Ȧһ���ʱ�������ΪҪ������һ֡��������û�н�����Ȧ
				if (j < leftnumber)
				{
					//��¼��λ�õ�ֵ�����´ε�i����ʼֵ
					raw_Data_ParseIndex_dspB += j;
					raw_Data_GoBack_Aux_Flag_dspB = true;
				}
				else
				{
					raw_Data_ParseIndex_dspB = j - leftnumber;
					raw_Data_GoBack_Aux_Flag_dspB = false;
				}

				//���ߵ�index��λ
				waveLine_Index_dspB = 0;
				//������ѭ��
				break;
			}
			//�����ҵ�ͬ��֡�ĵ�λ���������ҵ�ͬ��֡�ĸ�λ
			if ((raw_CacheBuffer_dspB[j] == (int16)FRAME_KEYWORD_LOW) && (raw_CacheBuffer_dspB[j + 1] == (int16)FRAME_KEYWORD_HIGH))
			{
				//˵����ͬ��֡����־��λ
				syncFrame_Flag_dspB = 1;
				j += 2;
				//��¼�յ���ͬ��֡����
				recv_SyncFrame_Num_dspB++;
				//������Ϊ��ͬ��֡���Բ���Ҫд�����ݣ������˴�ѭ��
				continue;
			}
			//reset g_dspB_wave_index
			if (waveLine_Index_dspB >= wave.cmd.bit.NUM)
			{
				waveLine_Index_dspB = 0;
				//����֡�ĸ�����1
				recv_DataFrame_Num_dspB++;
			}
			if (syncFrame_Flag_dspB == 1)//�������һ��ͬ��֡
			{
				if (waveLine_Index_dspB != 0)
				{
					//����ȱ�ٵ�ֵ
					for (int32 p = waveLine_Index_dspB; p < wave.cmd.bit.NUM; ++p)
					{
						(waveLine_WrIndex_dspB[p])++;

						//��������ʵ��д�������еĸ���
						waveLine_WrNumOnce_dspB[p]++;
						//дָ�븴λ
						if ((waveLine_WrIndex_dspB[p]) >= WAVE_LINE_BUFFER_LENTH)
						{
							(waveLine_WrIndex_dspB[p]) = 0;
						}
					}
					//���¶�λ��0����λ��
					waveLine_Index_dspB = 0;
				}
			}
			//����ǰֵ��Ϊ����д��,����ռ�ݵ��ֽ�����ͬ����buffer��ȡ�ø�����ͬ
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

			//��������ʵ��д�������еĸ���
			waveLine_WrNumOnce_dspB[waveLine_Index_dspB]++;

			//дָ�븴λ
			if ((waveLine_WrIndex_dspB[waveLine_Index_dspB]) >= WAVE_LINE_BUFFER_LENTH)
			{
				(waveLine_WrIndex_dspB[waveLine_Index_dspB]) = 0;
			}
			//����index˳������
			waveLine_Index_dspB++;

			//���ͬ��֡�ı�־
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
		//�ôν����ķ�Χ�Ǵ���һ�ν��������Ŀǰ��fpga��ȡ��������
		for (i = raw_Data_ParseIndex_dspB; i < (raw_Data_Index_dspB);)
		{
			//ֻҪʣ�µĸ���С�ڻ��ߵ�����������֡�ĳ��Ȳ������ߵ�index�����һ�����ߵ�index
			if (((raw_Data_Index_dspB - i) <= dataFrame_Lenth_dspB) && (waveLine_Index_dspB == wave.cmd.bit.NUM))
			{
				raw_Data_ParseIndex_dspB = i;								//��¼��λ�õ�ֵ�����´ε�i����ʼֵ			
				waveLine_Index_dspB = 0;								//���ߵ�index��λ				
				raw_Data_ParseLeftNumber_dspB = raw_Data_Index_dspB - i;		//����ʣ�µĸ���
				break;																//������ѭ��
			}
			//�����ж��������Ƿ���ͬ��֡
			//�����ҵ�ͬ��֡�ĵ�λ���������ҵ�ͬ��֡�ĸ�λ
			if ((raw_Buffer_dspB[i] == (int16)FRAME_KEYWORD_LOW) && (raw_Buffer_dspB[i + 1] == (int16)FRAME_KEYWORD_HIGH))
			{
				syncFrame_Flag_dspB = 1;								//˵����ͬ��֡����־��λ
				i += 2;																//����i
				recv_SyncFrame_Num_dspB++;											//��¼�յ���ͬ��֡����
				continue;															//������Ϊ��ͬ��֡���Բ���Ҫд�����ݣ������˴�ѭ��������ѭ��
			}
			//��λ��������index
			if (waveLine_Index_dspB >= wave.cmd.bit.NUM)
			{
				waveLine_Index_dspB = 0;
				recv_DataFrame_Num_dspB++;												//����֡�ĸ�����1
			}

			if (syncFrame_Flag_dspB == 1)//�������һ��ͬ��֡		
			{
				if (waveLine_Index_dspB != 0)
				{
					//����ȱ�ٵ�ֵ
					for (int32 p = waveLine_Index_dspB; p < wave.cmd.bit.NUM; ++p)
					{
						(waveLine_WrIndex_dspB[p])++;
						waveLine_WrNumOnce_dspB[p]++;								//��������ʵ��д�������еĸ���
						if ((waveLine_WrIndex_dspB[p]) >= WAVE_LINE_BUFFER_LENTH)
						{
							(waveLine_WrIndex_dspB[p]) = 0;					//дָ�븴λ
						}
					}
					waveLine_Index_dspB = 0;											//������������index					
				}
			}
			//����ǰֵ��Ϊ����д��,����ռ�ݵ��ֽ�����ͬ����buffer��ȡ�ø�����ͬ
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

			//��������ʵ��д�������еĸ���
			waveLine_WrNumOnce_dspB[waveLine_Index_dspB]++;

			//дָ�븴λ
			if ((waveLine_WrIndex_dspB[waveLine_Index_dspB]) >= WAVE_LINE_BUFFER_LENTH)
			{
				(waveLine_WrIndex_dspB[waveLine_Index_dspB]) = 0;
			}
			//����index˳������
			waveLine_Index_dspB++;

			//���ͬ��֡�ı�־
			syncFrame_Flag_dspB = 0;
		}

	}
	return 0;
}
//��fpga��buffer�ж�ȡ���ݣ�����dsp��ͬҪ�Ӳ�ͨ�ĵ�ַ��ȡ�����ҷŵ���ͬ��pcbuffer��
int16 CPlotWave::ReadPlotWaveDataFromFPGA(int16 dsp_number, WAVE_BUF_PRM& wave)
{
	int16	read_num;
	int16*	data = NULL;
	
	//��ȡ����
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

	//�����ݷŵ�pcbuffer��
	if (read_num != 0)
	{
		if (dsp_number == GTSD_DSP_A)
		{
			//�����ݷŵ�dspa��pcbuffer��
			int32 tmp;
			tmp = raw_Data_Index_dspA + read_num;
			//����Ѿ���Ȧ����Ҫ���������֣���������ָ��Ƶ���ǰ��
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
				raw_Data_GoBack_Flag_dspA = true;		//��Ȧ��־
			}
			else
			{
				errno_t err = memcpy_s(&(raw_Buffer_dspA[raw_Data_Index_dspA]), read_num*sizeof(int16), data, read_num*sizeof(int16));
				if (err!=0)
				{
					err = 0;
				}
				raw_Data_GoBack_Flag_dspA = false;		//��Ȧ��־
				if (raw_Data_GoBack_Aux_Flag_dspA == true)
				{
					raw_Data_GoBack_Flag_dspA = true;
				}
			}
			
			//����index
			raw_Data_Index_dspA += read_num;

			if (raw_Data_Index_dspA > (RAW_BUFFER_LENTH-1))
			{
				raw_Data_Index_dspA = raw_Data_Index_dspA - (RAW_BUFFER_LENTH);

#ifdef TIME_TEST
				overflow_flag++;
#endif
/**/
			}
			//��ʼ�������ݵ�ÿ�����ߵ�buffer��,һ�ζ������پͽ�������
		
			//�������ݵ����һ��������֡��ʣ�µ�������һ�ν���
			ParseDspAPlotWaveData(wave, read_num);

			EnterCriticalSection(&g_cs_dspA);
			for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)
			{
				//buffer��ʵ���е����ݸ�����Ӧ��ÿ�����߶���һ���ĲŶ�
				waveLine_WrNumSum_dspA[i] += waveLine_WrNumOnce_dspA[i];
			}
	
			LeaveCriticalSection(&g_cs_dspA);


		}
		else if (dsp_number == GTSD_DSP_B)
		{
			//�����ݷŵ�dspa��pcbuffer��
			int32 tmp;
			tmp = raw_Data_Index_dspB + read_num;
			//����Ѿ���Ȧ����Ҫ���������֣���������ָ��Ƶ���ǰ��
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
				raw_Data_GoBack_Flag_dspB = true;		//��Ȧ��־
			}
			else
			{
				errno_t err = memcpy_s(&(raw_Buffer_dspB[raw_Data_Index_dspB]), read_num*sizeof(int16), data, read_num*sizeof(int16));
				if (err != 0)
				{
					err = 0;
				}
				raw_Data_GoBack_Flag_dspB = false;		//��Ȧ��־
				if (raw_Data_GoBack_Aux_Flag_dspB == true)
				{
					raw_Data_GoBack_Flag_dspB = true;
				}
			}

			//����index
			raw_Data_Index_dspB += read_num;

			if (raw_Data_Index_dspB > (RAW_BUFFER_LENTH - 1))
			{
				raw_Data_Index_dspB = raw_Data_Index_dspB - (RAW_BUFFER_LENTH);

#ifdef TIME_TEST
				overflow_flag++;
#endif
				/**/
			}
		
			//�������ݵ����һ��������֡��ʣ�µ�������һ�ν���
			ParseDspBPlotWaveData(wave, read_num);
				
			EnterCriticalSection(&g_cs_dspB);
			for (int16 i = 0; i < wave.cmd.bit.NUM; ++i)
			{
				//buffer��ʵ���е����ݸ�����Ӧ��ÿ�����߶���һ���ĲŶ�
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
//numberʵ��ȡ�������ݣ����ݵ�ָ��
int16 CPlotWave::PW_PcGetDspAWaveData(double** data, int32& number)
{
	//��Ϊÿ�����е����ݶ���һ���ģ�ͬ���ģ���������ֻ�ж�һ���ߵĸ�������
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
		//������ֵ����
		(waveLine_RdIndex_dspA[i]) += realnumber;
		//����������ݼ���ֵ����������buffer���ܳ��ȣ����ص���ͷ
		if (waveLine_RdIndex_dspA[i] >= WAVE_LINE_BUFFER_LENTH)
		{
			waveLine_RdIndex_dspA[i] = waveLine_RdIndex_dspA[i] - WAVE_LINE_BUFFER_LENTH;
		}
		EnterCriticalSection(&g_cs_dspA);
		//buffer��ʵ���е����ݸ�����Ӧ��ÿ�����߶���һ���ĲŶ�
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
	//���ض�ȡ�ĸ���
	number = realnumber;
	return 0;
}
int16 CPlotWave::PW_PcGetDspBWaveData(double** data, int32& number)
{
	//��Ϊÿ�����е����ݶ���һ���ģ�ͬ���ģ���������ֻ�ж�һ���ߵĸ�������
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
		//������ֵ����
		(waveLine_RdIndex_dspB[i]) += realnumber;
		//����������ݼ���ֵ����������buffer���ܳ��ȣ����ص���ͷ
		if (waveLine_RdIndex_dspB[i] >= WAVE_LINE_BUFFER_LENTH)
		{
			waveLine_RdIndex_dspB[i] = waveLine_RdIndex_dspB[i] - WAVE_LINE_BUFFER_LENTH;
		}
		EnterCriticalSection(&g_cs_dspB);
		//buffer��ʵ���е����ݸ�����Ӧ��ÿ�����߶���һ���ĲŶ�
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
	//���ض�ȡ�ĸ���
	number = realnumber;
	return 0;
}
int16 CPlotWave::PW_StartDspAPlot(WAVE_BUF_PRM& wave)
{
	//����wave�еı��� ��ʼ��dspA�����һ�ĳ�ֶ�ʱ��ȥ��ȡdspA�����ݡ�
	InitDspAWaveVar(wave);

	//����FPGA��fifo
	int16 rtn;
	rtn = GTSD_CMD_ClearFpgaFifo(GTSD_DSP_A);
	if (rtn!=0)
	{
		return -1;
	}

#ifdef GETDATA_TYPE_THREAD
	//�����̶߳�ȡfpga����
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

	//�����������õ�����
	memcpy_s(&g_dspA_wave_prm, sizeof(WAVE_BUF_PRM), &wave, sizeof(WAVE_BUF_PRM));

	//��waveд��dspA����ʱdspA��ʼ��������
	GTSD_CMD_SetWaveBuf(GTSD_DSP_A,wave);
	return 0;
}
int16 CPlotWave::PW_StartDspBPlot(WAVE_BUF_PRM& wave)
{
	//����wave�еı��� ��ʼ��dspB�����һ�������ý�嶨ʱ��ȥ��ȡdspB�����ݡ�
	InitDspBWaveVar(wave);

	//����FPGA��fifo
	int16 rtn;
	rtn = GTSD_CMD_ClearFpgaFifo(GTSD_DSP_B);
	if (rtn != 0)
	{
		return -1;
	}

#ifdef GETDATA_TYPE_THREAD
	//�����̶߳�ȡfpga����
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

	//��waveд��dspB
	GTSD_CMD_SetWaveBuf(GTSD_DSP_B, wave);
	return 0;
}
int16 CPlotWave::PW_StopDspAPlot(WAVE_BUF_PRM& wave)
{
	//�ر�����ͨ��
	CloseDspAWave(wave);
	return 0;
}
int16 CPlotWave::PW_StopDspBPlot(WAVE_BUF_PRM& wave)
{
	//�ر�����ͨ��
	CloseDspBWave(wave);
	return 0;
}
