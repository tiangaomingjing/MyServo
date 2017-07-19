//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Communicaiton Packet Define		 							//
//	file				:	Packet.cpp													//
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
#include "Packet.h"
#include "BaseReturn_def.h"

CPacket::CPacket(void)
{
	pTx			= new PACKET_FORMAT;
	pRx			= new PACKET_FORMAT;
	bHaveMac	= false;
	init_pack();
}

CPacket::~CPacket(void)
{
	if (NULL != pTx)
		delete pTx;
	if (NULL != pRx)
		delete pRx;
}

void CPacket::init_pack()
{
	//	0xff,0xff,0xff,0xff,0xff,0xff,//dmac 6

	for (int i = 0; i < MAC_ADDR_BYTE_LENTH;i++)
	{
		pTx->dmac[i]	= 0xff;
	}
	//	0x3c,0x97,0x0e,0x30,0xe6,0x1e,//smac 6 12
	pTx->smac[0]		= 0x28;
	pTx->smac[1]		= 0xd2;
	pTx->smac[2]		= 0x44;
	pTx->smac[3]		= 0xdc;
	pTx->smac[4]		= 0x2a;
	pTx->smac[5]		= 0xf6;

	pTx->type0			= 0x88;
	pTx->type1			= 0xa4;
	pTx->lengthbit7_0	= 0x12;
	pTx->lengthbit9_8   = 0x10; 
	
	m_tx_index			= 0xff;
	m_rx_index			= m_tx_index;

	tx_length			= PACKET_HEADER_LENTH;
	rx_length			= 0;
}

Uint8 CPacket::get_tx_index()
{
	return m_tx_index;
}

bool CPacket::decoder_return_flag(int16 mode)
{
	if (rx_length < MIN_PACKET_LEN)
		return false;

	if ((pRx->type0 != pTx->type0) || (pRx->type1 != pTx->type1))
		return false;

	if (mode == PACKET_MODE_READ_FPGA)  //FPGA��ģʽ
	{
		int16 data = 0;
		memcpy_s(&data, sizeof(int16), &(pRx->dataSection[0]), sizeof(int16));
		return (((data & RETURN_REQUEST_MASK) != 0) ? true : false);//�ж�bit1λ�Ƿ�Ϊ1�����Ϊ1����ô˵��ָ���Ѿ����أ����Զ�ȡ����ֵ�����Ϊ0����ô˵��ָ��û�з��ء�
	}
	else
	{
		return true;
	}
}

int16 CPacket::decoder_dsp_packet(Uint8 index, int16* pData, int16 &dma_num)
{
	if (rx_length < MIN_PACKET_LEN)
		return Net_Rt_Receive_DLenth_short;

	if ((pRx->type0 != pTx->type0) || (pRx->type1 != pTx->type1))
		return Net_Rt_Receive_Unknow_Data;

	if (pRx->index != index)
		return Net_Rt_Index_Unmatch;

	//��ȡ������
	int16 data	= 0;
	memcpy_s(&data, sizeof(int16), &(pRx->lengthbit7_0), sizeof(int16));
	int16 lenth = (LENTH_MASK & data);

	//��ȡ���ݳ���
	memcpy_s(&data, sizeof(int16), &pRx->dma_numbit7_0, sizeof(int16));
	dma_num		= (DMA_NUMBER_MASK & data);

	//��ѯ������Ϣ�Ƿ���ȷ
	switch (pRx->dataSection[4])
	{
	case NET_COM_EXECUTE_FAIL:
		return Net_Rt_Execute_Fail;

	case NET_COM_EXECUTE_SUCCESS:
		break;

	case NET_COM_PARAMETER_INVALID:
		return Net_Rt_Parameter_invalid;

	case NET_COM_INSTRUCTION_INVALID:
		return Net_Rt_Instruction_invalid;

	default:
		return Net_Rt_Other_Error;
	}

	memcpy_s(pData, dma_num * sizeof(int16), &(pRx->dataSection[6]), dma_num * sizeof(int16));
	return Rt_Success;

}

int16 CPacket::decoder_fpga_packet(Uint8 index, int16* pData, int16 &dma_num)
{
	if (rx_length < MIN_PACKET_LEN)
		return Net_Rt_Receive_DLenth_short;

	if (pRx->type0 != pTx->type0 || pRx->type1 != pTx->type1)
		return Net_Rt_Receive_Unknow_Data;

	if (pRx->index != index)
		return Net_Rt_Index_Unmatch;

	//��ȡ������
	int16 data	= 0;
	memcpy_s(&data, sizeof(int16), &(pRx->lengthbit7_0), sizeof(int16));
	int16 lenth = (LENTH_MASK & data);
	
	//��ȡ���ݳ���
	memcpy_s(&data, sizeof(int16), &pRx->dma_numbit7_0, sizeof(int16));
	dma_num		= (DMA_NUMBER_MASK & data);

	memcpy_s(pData, dma_num * sizeof(short), &(pRx->dataSection[0]), dma_num * sizeof(short));
	return Rt_Success;
}

static int32 getLocalMac(Uint8 *mac, const int8* adapterName) //��ȡ����MACַ 
{
	//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();

	//�õ��ṹ���С,����GetAdaptersInfo����
	Uint32 stSize = sizeof(IP_ADAPTER_INFO);

	//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
	int32 nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);

	//��¼��������
	int32 netCardNum = 0;

	//��¼ÿ�������ϵ�IP��ַ����
	int32 IPnumPerNetCard = 0;

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//����������ص���ERROR_BUFFER_OVERFLOW
		//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
		//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
		//�ͷ�ԭ�����ڴ�ռ�
		delete pIpAdapterInfo;

		//���������ڴ�ռ������洢����������Ϣ
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];

		//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
	}

	int32 iCount = 0;
	while (pIpAdapterInfo)//����ÿһ������
	{
		//  pIpAdapterInfo->Address MACַ
		if (strstr(adapterName, pIpAdapterInfo->AdapterName) != NULL)
		{
			for (int32 i = 0; i < (int32)pIpAdapterInfo->AddressLength; i++)
			{
				mac[iCount++] = pIpAdapterInfo->Address[i];
			}
		}

		pIpAdapterInfo = pIpAdapterInfo->Next;
	}
	
	return iCount;
}

void CPacket::get_local_mac(const int8* adapterName)
{
	if (!bHaveMac)
	{
		if (getLocalMac(mac, adapterName) > 0)
		{
			int32 i;
			for (i = 0; i < MAC_ADDR_BYTE_LENTH; ++i)	//Source mac 
			{
				pTx->smac[i] = (mac[i]);
			}
			bHaveMac = true;
		}
	}
}

void CPacket::fillPacket(int16 dma_addr, int16* pData, int16 dma_num, int16 FPGAmode)
{
	int32 i,j;
	tx_length				= PACKET_HEADER_LENTH;
	int16 length			= dma_num * 2 + LENTH_INCLUDE;
	int16 iAddZero			= MIN_SEND_PKT_LEN - LENTH_BEFORE - length;
	if (iAddZero > 0)
		length = MIN_SEND_PKT_LEN - LENTH_BEFORE;

	length					|= LENTH_MASK_ZERO;
	length					&= LENTH_MASK_ONE;
	pTx->lengthbit7_0		= (length & 0xff);
	pTx->lengthbit9_8		= ((length>>8) & 0xff);
	
	m_tx_index++;		
	pTx->index				= m_tx_index;

	int16 dmanum			= dma_num;

	if (FPGAmode == PACKET_MODE_READ_FPGA)//��ģʽ
		dmanum				&= DMA_NUMBER_MASK;
	else
		dmanum				|= DMA_NUMBER_MASK_WRITE;

	pTx->dma_numbit7_0		= (dmanum & 0xff);
	pTx->dma_numbit8		= ((dmanum >> 8) & 0xff);

	pTx->dmaAddrbit7_0		= (dma_addr & 0xff);
	pTx->dmaAddrbit15_8		= ((dma_addr >> 8) & 0xff);

	for (i = 0,j=0; i < dma_num;i++)
	{
		pTx->dataSection[j]		= ((*(pData + i)) & 0xff);
		pTx->dataSection[j + 1] = (((*(pData + i)) >> 8) & 0xff);
		j						= j + 2;
		tx_length				+= 2;
	}

	for (i = 0; i < iAddZero; ++i)
	{
		pTx->dataSection[i+j]	= 0x0;
		tx_length++;
	}
}