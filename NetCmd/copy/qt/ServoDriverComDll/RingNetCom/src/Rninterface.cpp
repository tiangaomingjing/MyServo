//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	rn Communicaiton top level		 							//
//	file				:	Rninterface.CPP												//
//	Description			:	use for pc and fpga dsp communicaiton						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/3	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BaseReturn_def.h"
#include "Rninterface.h"


//////////////////////////////////////////////////////////////////////////
HANDLE Com_Rcv_Event;                 //Event for RN receive


//////////////////////////////////////////////////////////////////////////
static const int32			FPGA_MODE_RD = 0x0;							//FPGA������
static const int32			FPGA_MODE_WR = 0x1;							//FPGAд����

static const int32			DSP_MODE_RD = 0x0;							//DSP������
static const int32			DSP_MODE_WR = 0x1;							//DSPд����


static const int32			VIRTUAL_DSPA_COMADDR = 0x0400;				 //��ַΪshort��ַ
static const int32			VIRTUAL_DSPB_COMADDR = 0x8400;


CRnNetInterface::CRnNetInterface(void)
{
  p_Net = NULL;
  Com_Rcv_Event = CreateEvent(NULL, FALSE, FALSE, NULL);			//Create an Auto-Reset Event
}

CRnNetInterface::~CRnNetInterface(void)
{
  CloseHandle(Com_Rcv_Event);										//Close the Event Handle
}

/*******************************************************************************************
���ܣ�		��ȡ���������豸����Ϣ�б�
devsList:	�豸�б��ַ���ָ�룬��Ҫ�ڵ��øú���ʱ����ռ䣬���ɵ����߸����ͷ�
btyeSize��	�豸�б�ռ��byte��С
���أ�		��Ϣ�б��ַ����������������ƣ���$���֣�����ͬ�豸��Ϣ��"|"���֡�
�������ֵ��ʾ�ռ䲻������Ҫ��������ռ䲢�ٴε���
*******************************************************************************************/
int16 CRnNetInterface::NC_GetDevs(int8* devsList, int32& btyeSize)
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
      memcpy_s(devsList, (iSize + 1)*sizeof(int8), sztemp.c_str(), (iSize + 1)*sizeof(int8));
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
int16 CRnNetInterface::NC_GetAdapterDesp(vector<wstring> &list, vector<wstring> &nameList)
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
int16 CRnNetInterface::NC_Open(const int8* adapter, int32 port)
{
  if ((NULL != p_Net) && (NULL != p_Net->adhandle)) //����豸�Ѿ��򿪣�ֱ�ӷ���
    return Rt_Success;

  p_Net = new CRnNet(adapter, port);
  if ((NULL != p_Net) && (NULL != p_Net->adhandle))
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
int16 CRnNetInterface::NC_Close()
{
  pcap_breakloop(p_Net->adhandle);
  Sleep(200);
  //���û���յ���Ӧ����ô�͹رս���loop���ӳ٣��ȴ��̹߳رգ�Ȼ���ٹرոö˿�
  while (isInCallBack != -2)
  {
    //����������һ����־�������ǣ��ڻص������е�ʱ���������ڽ��н���,�ȴ��߳��˳���
    //Sleep(1);
  }
  Sleep(100);
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
int16 CRnNetInterface::RnNetCom_Open()
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

    if (Rt_Success == NC_Open(dBuf, 65536))  //���豸
    {
      //����ķ�ʽ����ͨ��һ�����Ժ�FPGA���������ͱ�������������ʱ��ͨ�������ñ������ͺ�checkesum�ȣ�����ǶԵı��ľ���λ��־λ��
      //�������жϱ�־λ���ж��Ƿ����������������������ڸñ���̫�̣�PC�ղ���������̼��Ჹ����С����64byte��
      //Sleep(200);
      //if (p_Net->checkComSts())
      //{
      //	delete dBuf;
      //	dBuf = NULL;
      //	//�ҵ��豸�������ӳɹ�
      //	return Rt_Success;
      //}
      //��Ϊ�Ȼ������ݰ�̫�̣�С��64��byte�����Կ�ʼ��ʱ���޷����յ��Ȼ��������������ݣ������ٸ��Ĺ̼����в��䡣
      //���ڽ��������PC������һ������FPGA����ȡ������FPGA��0x0000��ʼ��64��byte��ַ�����ݣ���Ϊ��ַ��short���͵ģ����ԾͶ�ȡ����32��short��ַ���ɡ�
      int16 mode		= FPGA_MODE_RD;
      int16 addr		= 0x0000;
      int16 des_id	= 0xff;
      int16 data[64]  = {0};
      int16 num		= 32;
      for (int16 j = 0; j < 2; ++j)
      {
        if (Rt_Success == RnNetCom_FPGA_ComHandler(mode, addr, data, num, des_id))
        {
          if (data[0]!= 0)
          {
            delete dBuf;
            dBuf = NULL;
            //�ҵ��豸�������ӳɹ�
            return Rt_Success;
          }
        }
      }
      pcap_breakloop(p_Net->adhandle);
      Sleep(200);
      //���û���յ���Ӧ����ô�͹رս���loop���ӳ٣��ȴ��̹߳رգ�Ȼ���ٹرոö˿�
      while(isInCallBack != -2)
      {
        //����������һ����־�������ǣ��ڻص������е�ʱ���������ڽ��н���,�ȴ��߳��˳���
        //Sleep(1);
      }
      Sleep(100);
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
int16 CRnNetInterface::RnNetCom_Close()
{
  NC_Close();
  return Rt_Success;
}

/*******************************************************************************************
���ܣ�FPGA����ͨ�Ž�������
���룺
mode:			1:д  0����
addr:			fpga��ַ
pData:			��������
num:			�������ݳ���
�����pData:    �������
num:			������ݳ���
���أ�0�ɹ��������ο������б�
*******************************************************************************************/
int16 CRnNetInterface::RnNetCom_FPGA_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id)
{
  int16 ret = Rt_Success;
  if ((NULL != p_Net) && (NULL != p_Net->adhandle))									//�����Ƿ�����
  {
    int32 iret = -1;

    if (num > (RN_MAX_PACKET_LEN>>1))
      return RN_Net_Rt_Send_Data_TooLong;

    int16 des_ch = RN_FPGA_CH_ID;
    int16 cmd;
    int16 needReq;
    int16 com_mode		= RN_MODE_MAIL;
    int16 IsReq			= RN_ISNOT_REQ;
    if (FPGA_MODE_RD == mode)
    {
      cmd				= RN_ARD;
      needReq			= RN_NEED_REQ;
    }
    else if (FPGA_MODE_WR == mode)
    {
      cmd				= RN_AWR;
      needReq			= RN_NOTNEED_REQ;
    }
    else
    {
      cmd				= RN_ARD;
      needReq			= RN_NEED_REQ;
    }

    p_Net->fillPacket(des_id, des_ch, pData, num, cmd, needReq, IsReq, addr, com_mode);								//������ݰ���ͷ
    if (0 == p_Net->tx_packet())																					//��������
    {
      if (needReq == RN_NEED_REQ)
      {
        //�ȴ�1s������źţ�˵���õ��˷���ֵ
        if (WaitForSingleObject(Com_Rcv_Event, 1000)== WAIT_OBJECT_0)
        {
          memcpy_s(pData, num*sizeof(int16), parsePacket, num*sizeof(int16));
        }
        else
        {
          ret = RN_Net_Rt_Timeout;
        }
      }
      else
      {
        ret = Rt_Success;
      }

    }
    else
    {
      ret = RN_Net_Rt_Send_Err;
    }
  }

  return ret;
}

/*******************************************************************************************
���ܣ�dsp����ͨ�Ž�������
���룺
mode:			1:д  0����
addr:			fpga��ַ
pData:			��������
num:			�������ݳ���
�����pData:    �������
num:			������ݳ���
���أ�0�ɹ��������ο������б�
*******************************************************************************************/
int16 CRnNetInterface::RnNetCom_DSP_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id)
{
  int16 ret = Rt_Success;
  if ((NULL != p_Net) && (NULL != p_Net->adhandle))									//�����Ƿ�����
  {
    int32 iret = -1;

    if (num > (RN_MAX_PACKET_LEN >> 1))
      return RN_Net_Rt_Send_Data_TooLong;

    //���ݵ�ַ�жϣ�����֮ǰ��PCdebug�ĵ�ַ����Ȼ�����ַ����û���ô�
    int16 des_ch;
    if ((Uint16)addr == (Uint16)VIRTUAL_DSPA_COMADDR)
    {
      des_ch = RN_DSP_CH_ID;
    }
    else if ((Uint16)addr == (((Uint16)VIRTUAL_DSPB_COMADDR)&0xffff))
    {
      des_ch = RN_PCI_CH_ID;
    }

    //����ģʽ����cmd���Ƿ���Ҫ��Ӧ
    int16 cmd;
    int16 needReq;
    int16 com_mode = RN_MODE_MAIL;
    int16 IsReq = RN_ISNOT_REQ;
    if (DSP_MODE_RD == mode)
    {
      cmd = RN_ARD;
      needReq = RN_NEED_REQ;
    }
    else if (DSP_MODE_WR == mode)
    {
      cmd = RN_AWR;
      needReq = RN_NEED_REQ;
    }
    else
    {
      cmd = RN_ARD;
      needReq = RN_NEED_REQ;
    }

    int16 order = *pData;
    int32 delay_time = 1000;
    //////////////////////////////////////////////////////////////////////////
    //�������ͬ���ӳٵȴ�ʱ�䲻ͬ
    switch (order & 0x0fff)
    {
      //���г�ʱ���ָ����ڴ�����
    case 38:
    case 39:
      delay_time = 120000;
      break;
    default:
      break;
    }

    p_Net->fillPacket(des_id, des_ch, pData, num, cmd, needReq, IsReq, addr, com_mode);								//������ݰ���ͷ
    if (0 == p_Net->tx_packet())																				//��������
    {
      if (needReq == RN_NEED_REQ)
      {
        //�ȴ�1s������źţ�˵���õ��˷���ֵ
        if (WaitForSingleObject(Com_Rcv_Event, delay_time) == WAIT_OBJECT_0)
        {
          //�����dsp��Ӧ������ӵ�������ʼȡ����Ϊ֮ǰ����������ŵ��ڽ��������Ѿ���֤������������֮ǰ���Ǽ��ݵġ�
          memcpy_s(pData, num*sizeof(int16), &parsePacket[3], num*sizeof(int16));
        }
        else
        {
          ret = RN_Net_Rt_Timeout;
        }
      }
      else
      {
        ret = Rt_Success;
      }
    }
    else
    {
      ret = RN_Net_Rt_Send_Err;
    }
  }

  return ret;
}



