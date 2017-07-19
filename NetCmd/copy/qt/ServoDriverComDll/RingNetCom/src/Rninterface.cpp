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
static const int32			FPGA_MODE_RD = 0x0;							//FPGA读操作
static const int32			FPGA_MODE_WR = 0x1;							//FPGA写操作

static const int32			DSP_MODE_RD = 0x0;							//DSP读操作
static const int32			DSP_MODE_WR = 0x1;							//DSP写操作


static const int32			VIRTUAL_DSPA_COMADDR = 0x0400;				 //地址为short地址
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
功能：		获取本地网络设备的信息列表
devsList:	设备列表字符串指针，需要在调用该函数时分配空间，并由调用者负责释放
btyeSize：	设备列表空间的byte大小
返回：		信息列表字符串。含描述和名称（用$区分），不同设备信息用"|"区分。
如果返回值表示空间不够，需要重新申请空间并再次调用
*******************************************************************************************/
int16 CRnNetInterface::NC_GetDevs(int8* devsList, int32& btyeSize)
{
  pcap_if_t*	alldevs;																//wpcap设备列表对象指针
  pcap_if_t*	devs;
  int8		errbuf[PCAP_ERRBUF_SIZE];												//参数

  if (pcap_findalldevs(&alldevs, errbuf) == -1)										//获取设备信息列表
    return Net_Rt_Not_Get_Net_DevList;
  if (NULL == alldevs)																//没有找到设备，返回
    return Net_Rt_Not_Find_Net_Device;

  devs = alldevs;																		//返回的alldevs是一个链表，信息都存储在pcap_if_t结构中
  string sztemp;
  while (devs)																		//对信息列表的描述和名称（GUID）进行封装
  {
    sztemp += devs->description;
    sztemp += " $ ";
    sztemp += devs->name;
    sztemp += " | ";
    devs = devs->next;
  }

  pcap_freealldevs(alldevs);															//释放链表

  int32 iSize = sztemp.size();														//比较分配的空间是否能够满足要求
  if (btyeSize < iSize)
  {
    return Net_Rt_Not_Enough_Space;													//分配的外部空间不够，需要重新分配
  }

  if (iSize > 0)																		//假如找到设备列表信息
  {
    if (NULL != devsList)
    {
      memset(devsList, 0, iSize);
      memcpy_s(devsList, (iSize + 1)*sizeof(int8), sztemp.c_str(), (iSize + 1)*sizeof(int8));
    }
    else
    {
      return Net_Rt_Pointer_Invalid;												//输入指针无效
    }
  }
  else
  {
    return Net_Rt_Net_DevListLenth_Err;												//设备信息列表长度错误
  }

  return Rt_Success;
}
/*******************************************************************************************
功能：		获取适配器的描述
list:		适配器的描述信息
nameList：	适配器的名称信息
返回：		0--成功
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

  //获取转换所需要的空间
  int size = MultiByteToWideChar(CP_ACP, 0, devslist, -1, NULL, 0);
  wchar_t *idevslist = new wchar_t[size];
  wmemset(idevslist, 0, size);
  //将多字节转化为宽字符
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
    iEnd = ilist.find((L" | "), iStart);							//从istart位置开始找字符串
    tmp = ilist.substr(iStart, iEnd - iStart);						//提取子字符串
    iMid = tmp.find((L" $ "), 0);									//从子字符串中开始位置找字符串

    sDesp = tmp.substr(0, iMid);									//从子字符串中找到描述
    tmp1 = tmp.substr(iMid + 3, tmp.length() - iMid - 3);			//从子字符串中找到名称
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
功能：		打开设备
输入：		adapter: 设备的名称（GUID）
port:		保留参数
返回：		0成功，其他参看错误列表。
*******************************************************************************************/
int16 CRnNetInterface::NC_Open(const int8* adapter, int32 port)
{
  if ((NULL != p_Net) && (NULL != p_Net->adhandle)) //如果设备已经打开，直接返回
    return Rt_Success;

  p_Net = new CRnNet(adapter, port);
  if ((NULL != p_Net) && (NULL != p_Net->adhandle))
  {
    return Rt_Success;
  }
  return Net_Rt_Open_Device_Err;
}
/*******************************************************************************************
功能：关闭设备
输入：   无
返回：   无
*******************************************************************************************/
int16 CRnNetInterface::NC_Close()
{
  pcap_breakloop(p_Net->adhandle);
  Sleep(200);
  //如果没有收到响应，那么就关闭接收loop。延迟，等待线程关闭，然后再关闭该端口
  while (isInCallBack != -2)
  {
    //这里增加了一个标志的作用是，在回调函数中的时候还有数据在进行解析,等待线程退出。
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
功能：		从扫描到的设备中找到有响应的设备并打开该设备，找到一个有响应的设备后就退出
输入：		无
返回：		0成功，其他参看错误列表。
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
    //获取转换所需的目标缓存大小
    int32 dBufSize = WideCharToMultiByte(CP_OEMCP, 0, adpNameList[i].c_str(), -1, NULL, 0, NULL, FALSE);
    //分配目标缓存
    int8* dBuf = new int8[dBufSize];
    memset(dBuf, 0, dBufSize);
    //转换
    int32 nRet = WideCharToMultiByte(CP_OEMCP, 0, adpNameList[i].c_str(), -1, dBuf, dBufSize, NULL, FALSE);

    if (Rt_Success == NC_Open(dBuf, 65536))  //打开设备
    {
      //最早的方式是想通过一连接以后，FPGA会主动发送报文上来，先延时，通过解析该报文类型和checkesum等，如果是对的报文就置位标志位，
      //在这里判断标志位来判断是否连接正常，但是现在由于该报文太短，PC收不到，后面固件会补齐最小长度64byte，
      //Sleep(200);
      //if (p_Net->checkComSts())
      //{
      //	delete dBuf;
      //	dBuf = NULL;
      //	//找到设备，并连接成功
      //	return Rt_Success;
      //}
      //因为等环网数据包太短，小于64个byte，所以开始的时候无法接收到等环网发上来的数据，后面再更改固件进行补充。
      //现在解决方案是PC主动发一个包到FPGA，读取连续的FPGA从0x0000开始的64个byte地址的数据，因为地址是short类型的，所以就读取连续32个short地址即可。
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
            //找到设备，并连接成功
            return Rt_Success;
          }
        }
      }
      pcap_breakloop(p_Net->adhandle);
      Sleep(200);
      //如果没有收到响应，那么就关闭接收loop。延迟，等待线程关闭，然后再关闭该端口
      while(isInCallBack != -2)
      {
        //这里增加了一个标志的作用是，在回调函数中的时候还有数据在进行解析,等待线程退出。
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
功能：关闭扫描到的有响应的设备
输入：   无
返回：   无
*******************************************************************************************/
int16 CRnNetInterface::RnNetCom_Close()
{
  NC_Close();
  return Rt_Success;
}

/*******************************************************************************************
功能：FPGA数据通信交互函数
输入：
mode:			1:写  0：读
addr:			fpga地址
pData:			输入数据
num:			输入数据长度
输出：pData:    输出数据
num:			输出数据长度
返回：0成功，其他参看错误列表。
*******************************************************************************************/
int16 CRnNetInterface::RnNetCom_FPGA_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id)
{
  int16 ret = Rt_Success;
  if ((NULL != p_Net) && (NULL != p_Net->adhandle))									//网络是否连接
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

    p_Net->fillPacket(des_id, des_ch, pData, num, cmd, needReq, IsReq, addr, com_mode);								//填充数据包报头
    if (0 == p_Net->tx_packet())																					//发送请求
    {
      if (needReq == RN_NEED_REQ)
      {
        //等待1s如果有信号，说明得到了返回值
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
功能：dsp数据通信交互函数
输入：
mode:			1:写  0：读
addr:			fpga地址
pData:			输入数据
num:			输入数据长度
输出：pData:    输出数据
num:			输出数据长度
返回：0成功，其他参看错误列表。
*******************************************************************************************/
int16 CRnNetInterface::RnNetCom_DSP_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id)
{
  int16 ret = Rt_Success;
  if ((NULL != p_Net) && (NULL != p_Net->adhandle))									//网络是否连接
  {
    int32 iret = -1;

    if (num > (RN_MAX_PACKET_LEN >> 1))
      return RN_Net_Rt_Send_Data_TooLong;

    //根据地址判断，兼容之前的PCdebug的地址，虽然这个地址现在没有用处
    int16 des_ch;
    if ((Uint16)addr == (Uint16)VIRTUAL_DSPA_COMADDR)
    {
      des_ch = RN_DSP_CH_ID;
    }
    else if ((Uint16)addr == (((Uint16)VIRTUAL_DSPB_COMADDR)&0xffff))
    {
      des_ch = RN_PCI_CH_ID;
    }

    //根据模式设置cmd和是否需要响应
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
    //根据命令不同，延迟等待时间不同
    switch (order & 0x0fff)
    {
      //如有长时间的指令就在此增加
    case 38:
    case 39:
      delay_time = 120000;
      break;
    default:
      break;
    }

    p_Net->fillPacket(des_id, des_ch, pData, num, cmd, needReq, IsReq, addr, com_mode);								//填充数据包报头
    if (0 == p_Net->tx_packet())																				//发送请求
    {
      if (needReq == RN_NEED_REQ)
      {
        //等待1s如果有信号，说明得到了返回值
        if (WaitForSingleObject(Com_Rcv_Event, delay_time) == WAIT_OBJECT_0)
        {
          //如果是dsp响应的命令从第三个开始取，因为之前的是命令轴号等在解析部分已经验证过，这样做和之前就是兼容的。
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



