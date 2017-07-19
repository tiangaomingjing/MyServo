//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	RN Communicaiton TX RX DEFINE		 						//
//	file				:	RingNetCom.h												//
//	Description			:	use for pc using winpcap communicaiton with RN				//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/3	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GTSD_RN_NET__
#define	__GTSD_RN_NET__

#include "pcap/pcap.h"
#include "RingNetPacket.h"

/* packet handler 函数原型 */
void packet_handler(Uint8 *param, const struct pcap_pkthdr *header, const Uint8 *pkt_data);

class CRnNet : public CRnPacket
{
public:
	CRnNet(const int8* adapter, int32 port);
	~CRnNet(void);

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

	//接收数据,采用回调函数的方式，有报文来就进入回调函数
	//void rx_packet();
};
unsigned __stdcall RnReceiveThread(void *para);
extern int16 parsePacket[512];
extern int16 parsePacketCmd[512];
extern int16 isInCallBack;
extern int16 waveDataLenth;
#endif