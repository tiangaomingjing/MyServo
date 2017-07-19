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

/* packet handler ����ԭ�� */
void packet_handler(Uint8 *param, const struct pcap_pkthdr *header, const Uint8 *pkt_data);

class CRnNet : public CRnPacket
{
public:
	CRnNet(const int8* adapter, int32 port);
	~CRnNet(void);

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

	//��������,���ûص������ķ�ʽ���б������ͽ���ص�����
	//void rx_packet();
};
unsigned __stdcall RnReceiveThread(void *para);
extern int16 parsePacket[512];
extern int16 parsePacketCmd[512];
extern int16 isInCallBack;
extern int16 waveDataLenth;
#endif