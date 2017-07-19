//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	RN Communicaiton TX RX DEFINE		 						//
//	file				:	RingNetCom.cpp												//
//	Description			:	use for pc using winpcap communicaiton with RN				//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/3	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <process.h>
#include "RingNetCom.h"
#include "BaseReturn_def.h"

//////////////////////////////////////////////////////////////////////////
int16 isInCallBack = 0;
int16 parsePacket[512] = { 0 };
int16 parsePacketCmd[512] = { 0 };
int16 waveDataLenth = 0;
//////////////////////////////////////////////////////////////////////////

CRnNet::CRnNet(const int8* adapter, int32 port)
{
	adhandle	= NULL;
	m_txStatus	= 0;
	if (init_net(adapter, port))
	{
		return;
	}
}

CRnNet::~CRnNet(void)
{
	if (adhandle)
		pcap_close(adhandle);
	adhandle = NULL;
}

int32 CRnNet::init_net(const int8* adapter, int32 port)
{
	pcap_if_t*	alldevs;
	pcap_if_t*	devs;
	char		errbuf[PCAP_ERRBUF_SIZE];

	//获取设备列表
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
		return Net_Rt_Not_Get_Net_DevList;

	if (NULL == alldevs)
		return Net_Rt_Not_Find_Net_Device;

	devs = alldevs;
	string strAdapterName;
	while (devs)
	{
		//判断字符串2是不是1的子串
		if (strstr(devs->name, adapter) != NULL)
		{
			m_adapterName = devs->name;
			break;
		}

		devs = devs->next;
	}
	if (NULL == devs)
	{
		pcap_freealldevs(alldevs);
		return Net_Rt_Adapter_Not_Match;
	}

	if ((adhandle = pcap_open_live(devs->name,		// name of the device
		65536,			// portion of the packet to capture. // 65536 grants that the whole packet will be captured on all the MACs.					
		1,				// promiscuous mode (nonzero means promiscuous)
		-1,				// read timeout
		errbuf			// error buffer
		)) == NULL)
	{
		//释放设备
		pcap_freealldevs(alldevs);
		return Net_Rt_Open_Device_Err;
	}

	pcap_freealldevs(alldevs);


	int8 filter[128] = { 0 };
	sprintf_s(filter, sizeof(filter), "ether src %02lx:%02lx:%02lx:%02lx:%02lx:%02lx", 0x55, 0x55, 0x55, 0x55, 0x55, 0x55);

	//compile the filter
	bpf_u_int32			NetMask;
	struct bpf_program	fcode;
	NetMask = 0xffffff;
	if (pcap_compile(adhandle, &fcode, filter, 1, NetMask) < 0)
	{
		pcap_close(adhandle);
		return RN_Net_Rt_Complie_Err;
	}
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		pcap_close(adhandle);
		return RN_Net_Rt_SetFilter_Err;
	}



	//开始接收,启动一个线程
	HANDLE receiveThread;
	receiveThread = (HANDLE)_beginthreadex(NULL, 0, &RnReceiveThread, this, 0, NULL);
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS/*HIGH_PRIORITY_CLASS*/);
	int32 processLevel = GetPriorityClass(GetCurrentProcess());
	SetThreadPriority(receiveThread, THREAD_PRIORITY_TIME_CRITICAL);
	int32 threadLevel = GetThreadPriority(receiveThread);
	

	return Rt_Success;
}
//////////////////////////////////////////////////////////////////////////
int32 CRnNet::tx_packet()
{
	m_txStatus = 0;

	int32 len = tx_length;
	Uint8* buffer = new Uint8[len];
	//因为之前没有mac，会收到别的地方发来的数据，不是某个站发来的，为了过滤，增加了mac，之前是从ptx头开始复制，现在从ttl开始复制，
	//mac不需要我来填写，
	memcpy_s((void*)buffer, sizeof(Uint8)*len, (void*)(&(pTx->ttl)), sizeof(Uint8)*len);

	//QueryPerformanceCounter(&g_Time1);
	if (pcap_sendpacket(adhandle, buffer, len) != 0)
	{
		m_txStatus = -1;
		delete[]buffer;
		return Net_Rt_Send_Err;
	}
	//QueryPerformanceCounter(&g_Time2);
	m_txStatus = 0;
	delete[]buffer;
	return Rt_Success;
}
////////////////////////////////////////////////////////////////////////////
//void CRnNet::rx_packet()
//{
//	
//}

unsigned __stdcall RnReceiveThread(void *para)
{
	/* 开始捕获 */
	CRnNet* prm = (CRnNet*)para;
	int16 rtn;
	rtn = pcap_loop(prm->adhandle, -1, packet_handler, (Uint8*)prm);
	if (-1 == rtn)
	{
		printf("---- pcap_loop error\n");
	}
	else if (-2 == rtn)
	{
		printf("---- pcap_loop break!\n");
	}
	isInCallBack = rtn;

	return 0;
}
//////////////////////////////////////////////////////////////////////////
/* 每次捕获到数据包时，libpcap都会自动调用这个回调函数 */
void packet_handler(Uint8 *param, const struct pcap_pkthdr *header, const Uint8 *pkt_data)
{
	
	CRnNet* net		= NULL;
	net				= (CRnNet*)param;
	if (NULL != header)
	{
		if (header->caplen > RN_MIN_PACKET_LEN)
		{	
			net->rx_length  = header->caplen;
			memcpy_s(net->pRx, header->caplen * sizeof(Uint8), pkt_data, header->caplen * sizeof(Uint8));
			net->decoderPacket((int16*)(&(parsePacket[0])), (int16)header->caplen);
			//isInCallBack = 0;
		}
		else
		{
			//isInCallBack = 0;
			return;
		}
	}
	
}