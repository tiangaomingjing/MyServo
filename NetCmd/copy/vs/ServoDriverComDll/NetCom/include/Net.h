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
	pcap_t*		adhandle;							//wpcap����ָ��
	int32		m_txStatus;							//����״̬
	string		m_adapterName;						//��������������

public:
	/*******************************************************************************************
	���ܣ���ʼ��wpcap�⣬�������ӷ����
	���룺
	adapter��   ��������������
	port:		��������
	*******************************************************************************************/
	int32 init_net(const int8* adapter, int32 port);

	//��������
	int32 tx_packet();
	
	//��������
	int32 wait_response();
};
#endif