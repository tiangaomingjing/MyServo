//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	socket packet 		 										//
//	file				:	SocketPacket.cpp											//
//	Description			:	use for change socket packet and rn packet 					//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/22	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SocketPacket.h"

//////////////////////////////////////////////////////////////////////////
CSocketPacket::CSocketPacket()
{
	m_sts		= new TStatus;
	m_rt_sts	= new TRemoteStatus;

	//���buffer
	int16 i = 0;
	for (i = 0; i < SOCKET_DATASECTION_LENTH; ++i)
	{
		m_sts->usdata[i]	= 0;
		m_rt_sts->usdata[i]	= 0;
	}
	m_sts->ulByteNum		= 0;
	m_rt_sts->ulByteNum		= 0;

}
//////////////////////////////////////////////////////////////////////////

CSocketPacket::~CSocketPacket()
{
	if (NULL != m_sts)
		delete m_sts;
	if (NULL != m_rt_sts)
		delete m_rt_sts;
}
//////////////////////////////////////////////////////////////////////////
//���Ͷ�
int16 CSocketPacket::FillSocketPacket(RN_PACKET_FORMAT* pRnTx, TStatus* sts)
{
	int16 ByteDataLenth;

	//��ȡ���ݳ��ȣ������ݳ���
	ByteDataLenth = ((pRnTx->header.bit.num_low & 0x00ff) | ((pRnTx->header.bit.num_bit8 << 8) & 0xff00));

	//���峤���Ƕ�Ӱ�ͷ14���ֽ�
	sts->ulByteNum = (ByteDataLenth + RN_PACKET_HEADER_LENTH);

	memcpy_s(&(sts->usdata[0]), sizeof(Uint8)*(sts->ulByteNum), &(pRnTx->ttl), sizeof(Uint8)*(sts->ulByteNum));

	return 0;
}
//////////////////////////////////////////////////////////////////////////
//���ն�
int16 CSocketPacket::DecodeSocketPacket(RN_PACKET_FORMAT* pRnRx, TRemoteStatus* rt_sts)
{
	//���ݺ�6����ͷ,��2��ȥ����һ��վ�ŵ����ݣ�ֻ����ʣ�µġ�
	memcpy_s(&(pRnRx->ttl), sizeof(Uint8)*(rt_sts->ulByteNum), &(rt_sts->usdata[0]), sizeof(Uint8)*(rt_sts->ulByteNum));
	
	return 0;
}

