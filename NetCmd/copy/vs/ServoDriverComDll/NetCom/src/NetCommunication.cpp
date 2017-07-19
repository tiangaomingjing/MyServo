//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Communicaiton top level		 								//
//	file				:	NetCommunication.cpp											//
//	Description			:	use for pc and fpga dsp communicaiton						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseReturn_def.h"
#include "NetCommunication.h"


//////////////////////////////////////////////////////////////////////////
const int32			FPGA_MODE_RD		= 0x0;							//FPGA������
const int32			FPGA_MODE_WR		= 0x1;							//FPGAд����	
	
const int32			MAX_DMA_NUM			= 509;							//�������󳤶�(���ͣ�short��

//��ַΪdsp��fpga֮��ͨ�ŵ�fpga��ַ����ͬ��dsp��Ӧ��ͬ��fpga��ַ��
const int32			NET_DSPA_COMADDR	= 0x0400;						//��ַΪshort��ַ
const int32			NET_DSPB_COMADDR	= 0x8400;

CNetCom::CNetCom(void)
{
	p_Net = NULL;
}

CNetCom::~CNetCom(void)
{

}

/*******************************************************************************************
���ܣ�		��ȡ���������豸����Ϣ�б�
devsList:	�豸�б��ַ���ָ�룬��Ҫ�ڵ��øú���ʱ����ռ䣬���ɵ����߸����ͷ�
btyeSize��	�豸�б�ռ��byte��С
���أ�		��Ϣ�б��ַ����������������ƣ���$���֣�����ͬ�豸��Ϣ��"|"���֡�
			�������ֵ��ʾ�ռ䲻������Ҫ��������ռ䲢�ٴε���
*******************************************************************************************/
int16 CNetCom::NC_GetDevs(int8* devsList, int32& btyeSize)
{
	pcap_if_t*	alldevs;																//wpcap�豸�б����ָ��
	pcap_if_t*	devs;
	int8		errbuf[PCAP_ERRBUF_SIZE];												//����

	if (pcap_findalldevs(&alldevs, errbuf) == -1)										//��ȡ�豸��Ϣ�б�
		return Net_Rt_Not_Get_Net_DevList;
	if (NULL == alldevs)																//û���ҵ��豸������
		return Net_Rt_Not_Find_Net_Device;

	devs = alldevs;																		//���ص�alldevs��һ��������Ϣ���洢��pcap_if_t�ṹ��
	string sztemp;
	while (devs)																		//����Ϣ�б�����������ƣ�GUID�����з�װ
	{
		sztemp += devs->description;
		sztemp += " $ ";
		sztemp += devs->name;
		sztemp += " | ";
		devs = devs->next;
	}

	pcap_freealldevs(alldevs);															//�ͷ�����

	int32 iSize = sztemp.size();														//�ȽϷ���Ŀռ��Ƿ��ܹ�����Ҫ��
	if (btyeSize < iSize)
	{
		return Net_Rt_Not_Enough_Space;													//������ⲿ�ռ䲻������Ҫ���·���
	}
	
	if (iSize > 0)																		//�����ҵ��豸�б���Ϣ
	{
		if (NULL != devsList)
		{
			memset(devsList, 0, iSize);
			memcpy_s(devsList, (iSize + 1)*sizeof(int8),sztemp.c_str(), (iSize + 1)*sizeof(int8));
		}
		else
		{
			return Net_Rt_Pointer_Invalid;												//����ָ����Ч
		}
	}
	else
	{
		return Net_Rt_Net_DevListLenth_Err;												//�豸��Ϣ�б��ȴ���										
	}

	return Rt_Success;
}
/*******************************************************************************************
���ܣ�		��ȡ������������
list:		��������������Ϣ
nameList��	��������������Ϣ
���أ�		0--�ɹ�
*******************************************************************************************/
int16 CNetCom::NC_GetAdapterDesp(vector<wstring> &list, vector<wstring> &nameList)
{
	int8* devslist = NULL;
	int32 byteSize = 1024;
	devslist = new int8[byteSize];
	int16 rtn;
	rtn = NC_GetDevs(devslist, byteSize);
	if (rtn != Rt_Success)
	{
		return rtn;
	}

	//��ȡת������Ҫ�Ŀռ�
	int size = MultiByteToWideChar(CP_ACP, 0, devslist, -1, NULL, 0);
	wchar_t *idevslist = new wchar_t[size];
	wmemset(idevslist, 0, size);
	//�����ֽ�ת��Ϊ���ַ�
	if (!MultiByteToWideChar(CP_ACP, 0, devslist, -1, idevslist, size))
	{
		return Net_Rt_MBTOWC_Err;
	}

	wstring ilist = idevslist;
	wstring ilistall = ilist;
	int iStart = 0;
	int iEnd = 0;
	int pos1 = 0;
	int pos2 = 0;
	wstring tmp;
	wstring tmp1;
	int iMid = 0;
	wstring  sDesp;
	wstring  sName;

	for (int i = 0; i < (int32)(ilistall.size());)
	{
		iEnd = ilist.find((L" | "), iStart);							//��istartλ�ÿ�ʼ���ַ���
		tmp = ilist.substr(iStart, iEnd - iStart);						//��ȡ���ַ���
		iMid = tmp.find((L" $ "), 0);									//�����ַ����п�ʼλ�����ַ���

		sDesp = tmp.substr(0, iMid);									//�����ַ������ҵ�����
		tmp1 = tmp.substr(iMid + 3, tmp.length() - iMid - 3);			//�����ַ������ҵ�����
		pos1 = tmp1.find((L"{"), 0);
		pos2 = tmp1.find((L"}"), 0);
		sName = tmp1.substr(pos1 + 1, pos2 - pos1 - 1);

		list.push_back(sDesp);
		nameList.push_back(sName);

		ilist.erase(iStart, iEnd - iStart + 3);
		i += iEnd + 3;
	}
	delete devslist;
	delete idevslist;
	return Rt_Success;
}
/*******************************************************************************************
���ܣ�		���豸
���룺		adapter: �豸�����ƣ�GUID��
port:		��������
���أ�		0�ɹ��������ο������б�
*******************************************************************************************/
int16 CNetCom::NC_Open(const int8* adapter, int32 port)
{
	if (NULL != p_Net && NULL != p_Net->adhandle) //����豸�Ѿ��򿪣�ֱ�ӷ���
		return Rt_Success;

	p_Net = new CNet(adapter, port);
	if (NULL != p_Net && NULL != p_Net->adhandle)
	{
		return Rt_Success;
	}
	return Net_Rt_Open_Device_Err;
}
/*******************************************************************************************
���ܣ��ر��豸
���룺   ��
���أ�   ��
*******************************************************************************************/
int16 CNetCom::NC_Close()
{
	if (NULL != p_Net)
	{
		delete p_Net;
		p_Net = NULL;
	}
	return Rt_Success;
}
/*******************************************************************************************
���ܣ�		��ɨ�赽���豸���ҵ�����Ӧ���豸���򿪸��豸���ҵ�һ������Ӧ���豸����˳�
���룺		��
���أ�		0�ɹ��������ο������б�
*******************************************************************************************/
int16 CNetCom::NetCom_Open()
{
	int16 counter = 0;
	int16 success_times = 0;
	vector<wstring> adapterList;
	vector<wstring> adpNameList;
	NC_GetAdapterDesp(adapterList, adpNameList);

	for (int32 i = 0; i < (int32)(adpNameList.size()); ++i)
	{
		//��ȡת�������Ŀ�껺���С  
		int32 dBufSize = WideCharToMultiByte(CP_OEMCP, 0, adpNameList[i].c_str(), -1, NULL, 0, NULL, FALSE);
		//����Ŀ�껺��  
		int8* dBuf = new int8[dBufSize];
		memset(dBuf, 0, dBufSize);
		//ת��  
		int32 nRet = WideCharToMultiByte(CP_OEMCP, 0, adpNameList[i].c_str(), -1, dBuf, dBufSize, NULL, FALSE);
		//��λ�ɹ�����
		success_times = 0;
		if (Rt_Success == NC_Open(dBuf, 65536))  //���豸
		{
			int16 dma_address		= NET_DSPA_COMADDR;		//FPGA ˫��ram��ַ
			int16 fulldataAry		= 0;
			int16 sLen				= 1;

			//����10�Σ�����ɹ���������5����Ϊ�豸��ͨ��
			for (counter = 0; counter < 10;counter++)
			{
				if (Rt_Success == NetCom_FPGA_ComHandler(FPGA_MODE_RD, dma_address, &fulldataAry, sLen)) //����������ȷ�����豸�Ƿ�ɽ�����Ӧ
				{
					success_times++;
				}
			}
			if (success_times > 5)
			{
				delete dBuf;
				dBuf = NULL;
				//�ҵ��豸�������ӳɹ�
				return Rt_Success;
			}
			
			delete dBuf;
			dBuf = NULL;
			NC_Close();
		}
		if (dBuf != NULL)
		{
			delete dBuf;
			dBuf = NULL;
		}
	}
	return Net_Rt_Not_Find_ScanDevice;
}
/*******************************************************************************************
���ܣ��ر�ɨ�赽������Ӧ���豸
���룺   ��
���أ�   ��
*******************************************************************************************/
int16 CNetCom::NetCom_Close()
{
	NC_Close();
	return Rt_Success;
}
/*******************************************************************************************
���ܣ�	��д��ɱ�־λ������
���룺
FpgaMode:	��������, FPGA_MODE_RD��ʾ�� FPGA_MODE_WR��ʾд
cmd:		����ID����ţ�bit[0- 11]��ʾ����ID��bit[12 - 15]��ʾ���
���أ�		true��ʾ�ɹ�
*******************************************************************************************/
bool CNetCom::ReturnFlaghandler(int16 FpgaMode, int16 cmd, int16 addr)
{
	int16	axisNum		= (cmd & 0xf000) >> 12;												//�������
	int16	dma_addr	= (int16)(FPGA_QUERY_ADDR0);										//���������������Ҫ��ѯ��FPGA��ַ�������ĸ�DSP��ÿ��DSP��Ӧ������
	if (addr == ((int16)NET_DSPB_COMADDR))
	{
		dma_addr		= (int16)(FPGA_QUERY_ADDR1);
	}
	int16	dma_num		= 0;																//��ʱdma_numΪ0
	if (FpgaMode == FPGA_MODE_WR)
	{
		dma_num			= 1;																//дʱdma_numΪ1	
	}
		
	int16	dat			= 0;
	p_Net->fillPacket(dma_addr, &dat, dma_num, FpgaMode);									//������ݰ���ͷ
	if (0 == p_Net->tx_packet())															//��������
	{
		int32 iret = -1;
		iret = p_Net->wait_response();														//�ȴ���Ӧ
		if (iret == 1)
		{
			return p_Net->decoder_return_flag(FpgaMode);
		}
	}
	return false;
}

/*******************************************************************************************
���ܣ�FPGA����ͨ�Ž�������
���룺
mode:			1:д  0����
dma_addr:		dma��ַ
pData:			��������
dma_num:		�������ݳ���
�����pData:    �������
dma_num:		������ݳ���
���أ�0�ɹ��������ο������б�
*******************************************************************************************/
int16 CNetCom::NetCom_FPGA_ComHandler(int16 mode, int16 dma_addr, int16* pData, int16 dma_num)
{
	int16 ret = Rt_Success;
	if ((NULL != p_Net) && (NULL != p_Net->adhandle))										//�����Ƿ�����
	{
		if (p_Net->bHaveMac)
		{
			int32 iret = -1;

			if (dma_num > MAX_DMA_NUM)
				return Net_Rt_Send_Data_TooLong;

			p_Net->fillPacket(dma_addr, pData, dma_num, mode);								//������ݰ���ͷ
			if (0 == p_Net->tx_packet())													//��������
			{
				iret = p_Net->wait_response();												//�ȴ���Ӧ
				if (iret == 1)
				{
					ret = p_Net->decoder_fpga_packet(p_Net->get_tx_index(), pData, dma_num); //�Է������ݽ��
				}
				else
				{
					ret = Net_Rt_Timeout;
				}
			}
			else
			{
				ret = Net_Rt_Send_Err;
			}

		}
		else
		{
			ret = Net_Rt_Mac_Err;
		}
	}

	return ret;
}

/*******************************************************************************************
���ܣ�dsp����ͨ�Ž�������
���룺
mode:			1:д  0���� modeû�о������ã�����
dma_addr:		dma��ַ
pData:          ��������
dma_num:		�������ݳ���
�����pData:    �������
dma_num:		������ݳ���
���أ�			0�ɹ��������ο������б�
*******************************************************************************************/
int16 CNetCom::NetCom_DSP_ComHandler(int16 mode, int16 dma_addr, int16* pData, int16 dma_num)
{
	int16 ret = Rt_Success;
	if ((NULL != p_Net) && (NULL != p_Net->adhandle))										//�����Ƿ�����
	{
		if (p_Net->bHaveMac)
		{
			int32 iret = -1;

			if (dma_num > MAX_DMA_NUM)
				return Net_Rt_Send_Data_TooLong;

			int16 cmd			= *pData;
			bool  bFinishFlag	= false;
			p_Net->fillPacket(dma_addr, pData, dma_num, FPGA_MODE_WR);					    //���DSP����,���ȶ���дһ������

			int32 icount = 0; //��ѯ����ɱ�־λ�ظ�����
			int32 lt_time = 100;
			//////////////////////////////////////////////////////////////////////////
			//�������ͬ���ӳٵȴ�ʱ�䲻ͬ
			int16 lt_cmd = cmd;
			switch (cmd &0x0fff)
			{
				//���г�ʱ���ָ����ڴ�����
			case 38:
			case 39:
				lt_time = 1000000;
				break;
			default:
				break;
			}			 
			//////////////////////////////////////////////////////////////////////////

			if (0 == p_Net->tx_packet())  //��������
			{
				iret = p_Net->wait_response();  //�ȴ���Ӧ
				if (iret == 1)
				{
					while ((!bFinishFlag) && (icount < lt_time))
					{
						bFinishFlag = ReturnFlaghandler(FPGA_MODE_RD, cmd, dma_addr);					//��ѯ������ɱ�־λ
						if (bFinishFlag)
						{
							p_Net->fillPacket(dma_addr, pData, dma_num, FPGA_MODE_RD);		//������������������ݰ���ͷ
							if (0 == p_Net->tx_packet()) //��������
							{
								iret = p_Net->wait_response();  //�ȴ���Ӧ
								if (iret == 1)
								{
									ret = p_Net->decoder_dsp_packet(p_Net->get_tx_index(), pData, dma_num);	  //�Է������ݽ��
									break;
								}
							}
						}
						icount++;
					}
					if (bFinishFlag == false)
					{
						ret = Net_Rt_Timeout;
					}
				}
			}
			else
			{
				ret = Net_Rt_Send_Err;

			}
			//������ر�־
			if (ReturnFlaghandler(FPGA_MODE_WR, cmd, dma_addr) == false)
			{
				ret = Net_Rt_Flag_Err;
			}		
		}
		else
		{
			ret = Net_Rt_Mac_Err;
		}
	}

	return ret;
}



