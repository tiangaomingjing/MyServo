//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	socket Communicaiton interface		 						//
//	file				:	SocketInterface.h											//
//	Description			:	use for pc using socket com									//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/22	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef		__GTSD_SOCKET_INTERFACE__
#define		__GTSD_SOCKET_INTERFACE__

//////////////////////////////////////////////////////////////////////////
#include "SocketCom.h"
#include "RingNetCom.h"

//////////////////////////////////////////////////////////////////////////
class CSocketInterface
{
public:
	CSocketInterface();
	~CSocketInterface();
	//-------------------------------------------------------------------------------------
	CSocketCom*		m_pSocket;																					//���������

	int16 SocketCom_Open(string ipAddr = "192.168.1.2");														//��socket�ӿ�
	
	int16 SocketCom_Close();																					//�ر�socket�ӿ�

	int16 SocketCom_FPGA_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id);				//sockets����FPGA

	int16 SocketCom_DSP_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id);				//socket����dsp
};
#endif