//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Communicaiton TX RX DEFINE		 							//
//	file				:	Net.cpp														//
//	Description			:	use for pc and fpga communicaiton							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Net.h"
#include "BaseReturn_def.h"

//////////////////////////////////////////////////////////////////////////

CNet::CNet(const int8* adapter, int32 port)
{
	adhandle = NULL;
	if (init_net(adapter, port))
	{
		return;
	}
	m_txStatus = 0;
}

CNet::~CNet(void)
{
	if (adhandle)
		pcap_close(adhandle);
	adhandle = NULL;
}

int32 CNet::init_net(const int8* adapter, int32 port)
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
									-1,			// read timeout
									errbuf			// error buffer
									)) == NULL)
	{
		//释放设备
		pcap_freealldevs(alldevs);
		return Net_Rt_Open_Device_Err;
	}
	
	pcap_freealldevs(alldevs);

	get_local_mac(m_adapterName.c_str());

	int8 filter[128] = { 0 };
	sprintf_s(filter, sizeof(filter), "ether dst %02lx:%02lx:%02lx:%02lx:%02lx:%02lx", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	//compile the filter
	bpf_u_int32			NetMask;
	struct bpf_program	fcode;
	NetMask = 0xffffff;
	if (pcap_compile(adhandle, &fcode, filter, 1, NetMask) < 0)
	{
		pcap_close(adhandle);
		return Net_Rt_Complie_Err;
	}
	if (pcap_setfilter(adhandle, &fcode) < 0)
	{
		pcap_close(adhandle);
		return Net_Rt_SetFilter_Err;
	}

	return Rt_Success;
}

int32 CNet::tx_packet()
{
	m_txStatus = 0;

	int32 len		= tx_length;
	Uint8* buffer	= new Uint8[len];
	memcpy_s((void*)buffer, sizeof(Uint8)*len,(void*)pTx, sizeof(Uint8)*len);

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

int32 CNet::wait_response()
{
	pcap_pkthdr *header		= NULL;
	int32 nret				= -1;
	const Uint8 *pTempRx	= NULL;
	int32 iCount			= 0;
	int32 iSuccessNum		= 0;
	bool bSuccess			= false;
	do
	{
		nret = pcap_next_ex((pcap_t *)adhandle, &header, &pTempRx);
		if (nret == 0)
		{
			iCount++;
		}
		else if (nret == 1)
		{
			//QueryPerformanceCounter(&g_Time3);
			if (NULL != header)
			{
				if (header->caplen > MIN_PACKET_LEN)
				{
					bSuccess = true;
					break;
				}
			}

			iSuccessNum++;
		}

	} while (iCount < 1000);

	if (bSuccess)
	{
		rx_length = header->caplen;
		memcpy_s(pRx, header->caplen * sizeof(Uint8),pTempRx, header->caplen * sizeof(Uint8));
	}
	else
	{
		nret = Net_Rt_Receive_Err;
	}

	return nret;
}