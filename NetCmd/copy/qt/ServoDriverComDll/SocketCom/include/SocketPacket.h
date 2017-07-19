//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	socket packet 		 										//
//	file				:	SocketPacket.h												//
//	Description			:	use for change socket packet and rn packet					//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/22	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef		__GTSD_SOCKET_PACKET__
#define		__GTSD_SOCKET_PACKET__

#include "Basetype_def.h"
#include "RingNetPacket.h"

#define 	SOCKET_DATASECTION_LENTH   500								//socket���ݽṹ�����ݶεĳ��ȣ�short���ȣ�

//////////////////////////////////////////////////////////////////////////
//�����������ṹ��
//-------------------------------------------------------------------------------
//����
typedef struct Tmy
{
	Uint32			ulByteNum;							//length of data buffer,unit:Byte
	int16			usdata[SOCKET_DATASECTION_LENTH];   //��������
}TStatus;
//--------------------------------------------------------------------
//����
typedef struct TRemote
{
	Uint32			ulByteNum;							//lenght of data buffer,unit:Byte
	int16           usdata[SOCKET_DATASECTION_LENTH];   //��������
}TRemoteStatus;


//////////////////////////////////////////////////////////////////////////
//�̳еȻ����İ���ʽ������ʱ�����ݰ��յȻ�����ʽ�����Ȼ����дsocket����������
//���ص�������socket����������Ҳ�ǰ��յȻ����ĸ�ʽ���н��

class CSocketPacket : public CRnPacket
{
public:
	CSocketPacket();
	~CSocketPacket();

	TStatus*					m_sts;								//����״̬��������
	TRemoteStatus*				m_rt_sts;							//Զ��״̬��������

	int16 FillSocketPacket(RN_PACKET_FORMAT* pRnTx,TStatus* sts);
	int16 DecodeSocketPacket(RN_PACKET_FORMAT* pRnRx, TRemoteStatus* rt_sts);
};
//////////////////////////////////////////////////////////////////////////
#endif