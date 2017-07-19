//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	socket Communicaiton interface		 						//
//	file				:	socketinterface.cpp												//
//	Description			:	use for pc using socket com									//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/5	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocketInterface.h"
#include "BaseReturn_def.h"
#include "RingNetCom.h"

extern HANDLE Com_Rcv_Event;

static const int32			FPGA_MODE_RD = 0x0;							//FPGA读操作
static const int32			FPGA_MODE_WR = 0x1;							//FPGA写操作	

static const int32			DSP_MODE_RD = 0x0;							//DSP读操作
static const int32			DSP_MODE_WR = 0x1;							//DSP写操作	


static const int32			VIRTUAL_DSPA_COMADDR = 0x0400;				 //地址为short地址
static const int32			VIRTUAL_DSPB_COMADDR = 0x8400;


//---------------------------------------------------------------

CSocketInterface::CSocketInterface()
{
	m_pSocket		= NULL;
}

CSocketInterface::~CSocketInterface()
{
}

int16 CSocketInterface::SocketCom_Open(string ipAddr /*= "192.168.1.2"*/)
{
	//PC是客户端
	int16 rtn = Rt_Success;
	
	if ((NULL != m_pSocket)) //如果设备已经打开，直接返回
		return rtn;

	m_pSocket = new CSocketCom();
	if ((NULL == m_pSocket))
	{
		return SOCKET_Rt_CreateObj_Err;
	}

	rtn = m_pSocket->SocketOpen(ipAddr, 6000);
	
	if (rtn)
	{
		printf(("连接到服务器%s错误，请查看服务器是否打开！"),ipAddr);
		rtn = SOCKET_Rt_Open_Device_Err;
	}

	//上面只是验证可以和cpu进行通信，并不能说明FPGA dsp固件是否正常
	int16 mode		= FPGA_MODE_RD;
	int16 addr		= 0x0000;
	int16 des_id	= 0xff; //广播发送
	int16 data[64]	= { 0 };
	int16 num		= 32;
	for (int16 i = 0; i < 1; ++i)
	{
		if (Rt_Success == SocketCom_FPGA_ComHandler(mode, addr, data, num, des_id))
		{
			if (data[0] != 0)
			{
				//固件测试通信连接成功
				rtn = Rt_Success;
			}
			else
			{
				rtn = SOCKET_Rt_Com_Err;
			}
		}
	}
	return rtn;
}

int16 CSocketInterface::SocketCom_Close()
{
	m_pSocket->SocketClose();

	if (NULL != m_pSocket)
	{
		delete m_pSocket;
		m_pSocket = NULL;
	}
	return Rt_Success;
}

int16 CSocketInterface::SocketCom_FPGA_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id)
{
	int16 ret = Rt_Success;
	if ((NULL != m_pSocket))									//网络是否连接
	{
		int32 iret = -1;

		if (num > (RN_MAX_PACKET_LEN >> 1))
			return RN_Net_Rt_Send_Data_TooLong;

		int16 des_ch = RN_FPGA_CH_ID;
		int16 cmd;
		int16 needReq;
		int16 com_mode = RN_MODE_MAIL;
		int16 IsReq = RN_ISNOT_REQ;
		if (FPGA_MODE_RD == mode)
		{
			cmd = RN_ARD;
			needReq = RN_NEED_REQ;
		}
		else if (FPGA_MODE_WR == mode)
		{
			cmd = RN_AWR;
			needReq = RN_NOTNEED_REQ;
		}
		else
		{
			cmd = RN_ARD;
			needReq = RN_NEED_REQ;
		}

		m_pSocket->fillPacket(des_id, des_ch, pData, num, cmd, needReq, IsReq, addr, com_mode);							//填充数据包报头
		m_pSocket->FillSocketPacket(m_pSocket->pTx, m_pSocket->m_sts);													//转化格式为socket包格式
		if (0 == m_pSocket->SocketSend())																				//发送请求
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
					ret = SOCKET_Rt_Timeout;
				}
			}
			else
			{
				ret = Rt_Success;
			}	
		}
		else
		{
			ret = SOCKET_Rt_Send_Err;
		}
	}

	return ret;
}

int16 CSocketInterface::SocketCom_DSP_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id)
{
	int16 ret = Rt_Success;
	if (NULL != m_pSocket)									//网络是否连接
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
		else if ((Uint16)addr == (((Uint16)VIRTUAL_DSPB_COMADDR) & 0xffff))
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

		m_pSocket->fillPacket(des_id, des_ch, pData, num, cmd, needReq, IsReq, addr, com_mode);								//填充数据包报头
		if (0 == m_pSocket->SocketSend())																				//发送请求
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
					ret = SOCKET_Rt_Timeout;
				}
			}
			else
			{
				ret = Rt_Success;
			}
		}
		else
		{
			ret = SOCKET_Rt_Send_Err;
		}
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////