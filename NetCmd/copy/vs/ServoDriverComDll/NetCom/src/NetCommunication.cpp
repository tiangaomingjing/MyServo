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
const int32			FPGA_MODE_RD		= 0x0;							//FPGA读操作
const int32			FPGA_MODE_WR		= 0x1;							//FPGA写操作	
	
const int32			MAX_DMA_NUM			= 509;							//命令发送最大长度(类型：short）

//地址为dsp与fpga之间通信的fpga地址，不同的dsp对应不同的fpga地址。
const int32			NET_DSPA_COMADDR	= 0x0400;						//地址为short地址
const int32			NET_DSPB_COMADDR	= 0x8400;

CNetCom::CNetCom(void)
{
	p_Net = NULL;
}

CNetCom::~CNetCom(void)
{

}

/*******************************************************************************************
功能：		获取本地网络设备的信息列表
devsList:	设备列表字符串指针，需要在调用该函数时分配空间，并由调用者负责释放
btyeSize：	设备列表空间的byte大小
返回：		信息列表字符串。含描述和名称（用$区分），不同设备信息用"|"区分。
			如果返回值表示空间不够，需要重新申请空间并再次调用
*******************************************************************************************/
int16 CNetCom::NC_GetDevs(int8* devsList, int32& btyeSize)
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
			memcpy_s(devsList, (iSize + 1)*sizeof(int8),sztemp.c_str(), (iSize + 1)*sizeof(int8));
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
int16 CNetCom::NC_Open(const int8* adapter, int32 port)
{
	if (NULL != p_Net && NULL != p_Net->adhandle) //如果设备已经打开，直接返回
		return Rt_Success;

	p_Net = new CNet(adapter, port);
	if (NULL != p_Net && NULL != p_Net->adhandle)
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
功能：		从扫描到的设备中找到有响应的设备并打开该设备，找到一个有响应的设备后就退出
输入：		无
返回：		0成功，其他参看错误列表。
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
		//获取转换所需的目标缓存大小  
		int32 dBufSize = WideCharToMultiByte(CP_OEMCP, 0, adpNameList[i].c_str(), -1, NULL, 0, NULL, FALSE);
		//分配目标缓存  
		int8* dBuf = new int8[dBufSize];
		memset(dBuf, 0, dBufSize);
		//转换  
		int32 nRet = WideCharToMultiByte(CP_OEMCP, 0, adpNameList[i].c_str(), -1, dBuf, dBufSize, NULL, FALSE);
		//复位成功次数
		success_times = 0;
		if (Rt_Success == NC_Open(dBuf, 65536))  //打开设备
		{
			int16 dma_address		= NET_DSPA_COMADDR;		//FPGA 双口ram地址
			int16 fulldataAry		= 0;
			int16 sLen				= 1;

			//测试10次，如果成功次数大于5次认为设备是通的
			for (counter = 0; counter < 10;counter++)
			{
				if (Rt_Success == NetCom_FPGA_ComHandler(FPGA_MODE_RD, dma_address, &fulldataAry, sLen)) //发送命令以确定此设备是否可接收响应
				{
					success_times++;
				}
			}
			if (success_times > 5)
			{
				delete dBuf;
				dBuf = NULL;
				//找到设备，并连接成功
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
功能：关闭扫描到的有响应的设备
输入：   无
返回：   无
*******************************************************************************************/
int16 CNetCom::NetCom_Close()
{
	NC_Close();
	return Rt_Success;
}
/*******************************************************************************************
功能：	读写完成标志位处理函数
输入：
FpgaMode:	请求类型, FPGA_MODE_RD表示读 FPGA_MODE_WR表示写
cmd:		命令ID和轴号，bit[0- 11]表示命令ID，bit[12 - 15]表示轴号
返回：		true表示成功
*******************************************************************************************/
bool CNetCom::ReturnFlaghandler(int16 FpgaMode, int16 cmd, int16 addr)
{
	int16	axisNum		= (cmd & 0xf000) >> 12;												//计算轴号
	int16	dma_addr	= (int16)(FPGA_QUERY_ADDR0);										//根据轴号来区分需要查询的FPGA地址是属于哪个DSP，每个DSP对应两个轴
	if (addr == ((int16)NET_DSPB_COMADDR))
	{
		dma_addr		= (int16)(FPGA_QUERY_ADDR1);
	}
	int16	dma_num		= 0;																//读时dma_num为0
	if (FpgaMode == FPGA_MODE_WR)
	{
		dma_num			= 1;																//写时dma_num为1	
	}
		
	int16	dat			= 0;
	p_Net->fillPacket(dma_addr, &dat, dma_num, FpgaMode);									//填充数据包报头
	if (0 == p_Net->tx_packet())															//发送请求
	{
		int32 iret = -1;
		iret = p_Net->wait_response();														//等待响应
		if (iret == 1)
		{
			return p_Net->decoder_return_flag(FpgaMode);
		}
	}
	return false;
}

/*******************************************************************************************
功能：FPGA数据通信交互函数
输入：
mode:			1:写  0：读
dma_addr:		dma地址
pData:			输入数据
dma_num:		输入数据长度
输出：pData:    输出数据
dma_num:		输出数据长度
返回：0成功，其他参看错误列表。
*******************************************************************************************/
int16 CNetCom::NetCom_FPGA_ComHandler(int16 mode, int16 dma_addr, int16* pData, int16 dma_num)
{
	int16 ret = Rt_Success;
	if ((NULL != p_Net) && (NULL != p_Net->adhandle))										//网络是否连接
	{
		if (p_Net->bHaveMac)
		{
			int32 iret = -1;

			if (dma_num > MAX_DMA_NUM)
				return Net_Rt_Send_Data_TooLong;

			p_Net->fillPacket(dma_addr, pData, dma_num, mode);								//填充数据包报头
			if (0 == p_Net->tx_packet())													//发送请求
			{
				iret = p_Net->wait_response();												//等待响应
				if (iret == 1)
				{
					ret = p_Net->decoder_fpga_packet(p_Net->get_tx_index(), pData, dma_num); //对返回数据解包
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
功能：dsp数据通信交互函数
输入：
mode:			1:写  0：读 mode没有具体作用，备用
dma_addr:		dma地址
pData:          输入数据
dma_num:		输入数据长度
输出：pData:    输出数据
dma_num:		输出数据长度
返回：			0成功，其他参看错误列表。
*******************************************************************************************/
int16 CNetCom::NetCom_DSP_ComHandler(int16 mode, int16 dma_addr, int16* pData, int16 dma_num)
{
	int16 ret = Rt_Success;
	if ((NULL != p_Net) && (NULL != p_Net->adhandle))										//网络是否连接
	{
		if (p_Net->bHaveMac)
		{
			int32 iret = -1;

			if (dma_num > MAX_DMA_NUM)
				return Net_Rt_Send_Data_TooLong;

			int16 cmd			= *pData;
			bool  bFinishFlag	= false;
			p_Net->fillPacket(dma_addr, pData, dma_num, FPGA_MODE_WR);					    //填充DSP命令,首先都是写一个命令

			int32 icount = 0; //查询读完成标志位重复次数
			int32 lt_time = 100;
			//////////////////////////////////////////////////////////////////////////
			//根据命令不同，延迟等待时间不同
			int16 lt_cmd = cmd;
			switch (cmd &0x0fff)
			{
				//如有长时间的指令就在此增加
			case 38:
			case 39:
				lt_time = 1000000;
				break;
			default:
				break;
			}			 
			//////////////////////////////////////////////////////////////////////////

			if (0 == p_Net->tx_packet())  //发送请求
			{
				iret = p_Net->wait_response();  //等待响应
				if (iret == 1)
				{
					while ((!bFinishFlag) && (icount < lt_time))
					{
						bFinishFlag = ReturnFlaghandler(FPGA_MODE_RD, cmd, dma_addr);					//查询命令完成标志位
						if (bFinishFlag)
						{
							p_Net->fillPacket(dma_addr, pData, dma_num, FPGA_MODE_RD);		//填充接收数据命令的数据包报头
							if (0 == p_Net->tx_packet()) //发送请求
							{
								iret = p_Net->wait_response();  //等待响应
								if (iret == 1)
								{
									ret = p_Net->decoder_dsp_packet(p_Net->get_tx_index(), pData, dma_num);	  //对返回数据解包
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
			//清除返回标志
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



