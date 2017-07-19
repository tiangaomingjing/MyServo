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

#define 	SOCKET_DATASECTION_LENTH   500								//socket数据结构中数据段的长度（short长度）

//////////////////////////////////////////////////////////////////////////
//定义数据区结构体
//-------------------------------------------------------------------------------
//发送
typedef struct Tmy
{
	Uint32			ulByteNum;							//length of data buffer,unit:Byte
	int16			usdata[SOCKET_DATASECTION_LENTH];   //定义数据
}TStatus;
//--------------------------------------------------------------------
//接收
typedef struct TRemote
{
	Uint32			ulByteNum;							//lenght of data buffer,unit:Byte
	int16           usdata[SOCKET_DATASECTION_LENTH];   //定义数据
}TRemoteStatus;


//////////////////////////////////////////////////////////////////////////
//继承等环网的包格式，发送时将数据按照等环网格式组包，然后填写socket的数据区，
//返回的数据是socket的数据区，也是按照等环网的格式进行解包

class CSocketPacket : public CRnPacket
{
public:
	CSocketPacket();
	~CSocketPacket();

	TStatus*					m_sts;								//本地状态，发送用
	TRemoteStatus*				m_rt_sts;							//远程状态，接收用

	int16 FillSocketPacket(RN_PACKET_FORMAT* pRnTx,TStatus* sts);
	int16 DecodeSocketPacket(RN_PACKET_FORMAT* pRnRx, TRemoteStatus* rt_sts);
};
//////////////////////////////////////////////////////////////////////////
#endif