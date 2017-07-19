//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Communicaiton TX RX DEFINE		 							//
//	file				:	Net.h														//
//	Description			:	use for pc and fpga communicaiton							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GTSD_NET__
#define	__GTSD_NET__

#include "pcap/pcap.h"
#include "Packet.h"


class CNet : public CPacket
{
public:
	CNet(const int8* adapter, int32 port);
	~CNet(void);

public:
	pcap_t*		adhandle;							//wpcap对象指针
	int32		m_txStatus;							//发送状态
	string		m_adapterName;						//网络适配器名称

public:
	/*******************************************************************************************
	功能：初始化wpcap库，并且连接服务端
	输入：
	adapter：   网络适配器名称
	port:		保留参数
	*******************************************************************************************/
	int32 init_net(const int8* adapter, int32 port);

	//发送数据
	int32 tx_packet();
	
	//接收数据
	int32 wait_response();
};
#endif