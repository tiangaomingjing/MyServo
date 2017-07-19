//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	socket Communicaiton interface		 						//
//	file				:	socketinterface.cpp												//
//	Description			:	use for pc using socket com									//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/5	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocketInterface.h"
#include "BaseReturn_def.h"
#include "RingNetCom.h"

extern HANDLE Com_Rcv_Event;

static const int32			FPGA_MODE_RD = 0x0;							//FPGA������
static const int32			FPGA_MODE_WR = 0x1;							//FPGAд����	

static const int32			DSP_MODE_RD = 0x0;							//DSP������
static const int32			DSP_MODE_WR = 0x1;							//DSPд����	


static const int32			VIRTUAL_DSPA_COMADDR = 0x0400;				 //��ַΪshort��ַ
static const int32			VIRTUAL_DSPB_COMADDR = 0x8400;


//---------------------------------------------------------------

CSocketInterface::CSocketInterface()
{
	m_pSocket		= NULL;
}

CSocketInterface::~CSocketInterface()
{
}

int16 CSocketInterface::SocketCom_Open(string ipAddr /*= "192.168.1.2"*/)
{
	//PC�ǿͻ���
	int16 rtn = Rt_Success;
	
	if ((NULL != m_pSocket)) //����豸�Ѿ��򿪣�ֱ�ӷ���
		return rtn;

	m_pSocket = new CSocketCom();
	if ((NULL == m_pSocket))
	{
		return SOCKET_Rt_CreateObj_Err;
	}

	rtn = m_pSocket->SocketOpen(ipAddr, 6000);
	
	if (rtn)
	{
		printf(("���ӵ�������%s������鿴�������Ƿ�򿪣�"),ipAddr);
		rtn = SOCKET_Rt_Open_Device_Err;
	}

	//����ֻ����֤���Ժ�cpu����ͨ�ţ�������˵��FPGA dsp�̼��Ƿ�����
	int16 mode		= FPGA_MODE_RD;
	int16 addr		= 0x0000;
	int16 des_id	= 0xff; //�㲥����
	int16 data[64]	= { 0 };
	int16 num		= 32;
	for (int16 i = 0; i < 1; ++i)
	{
		if (Rt_Success == SocketCom_FPGA_ComHandler(mode, addr, data, num, des_id))
		{
			if (data[0] != 0)
			{
				//�̼�����ͨ�����ӳɹ�
				rtn = Rt_Success;
			}
			else
			{
				rtn = SOCKET_Rt_Com_Err;
			}
		}
	}
	return rtn;
}

int16 CSocketInterface::SocketCom_Close()
{
	m_pSocket->SocketClose();

	if (NULL != m_pSocket)
	{
		delete m_pSocket;
		m_pSocket = NULL;
	}
	return Rt_Success;
}

int16 CSocketInterface::SocketCom_FPGA_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id)
{
	int16 ret = Rt_Success;
	if ((NULL != m_pSocket))									//�����Ƿ�����
	{
		int32 iret = -1;

		if (num > (RN_MAX_PACKET_LEN >> 1))
			return RN_Net_Rt_Send_Data_TooLong;

		int16 des_ch = RN_FPGA_CH_ID;
		int16 cmd;
		int16 needReq;
		int16 com_mode = RN_MODE_MAIL;
		int16 IsReq = RN_ISNOT_REQ;
		if (FPGA_MODE_RD == mode)
		{
			cmd = RN_ARD;
			needReq = RN_NEED_REQ;
		}
		else if (FPGA_MODE_WR == mode)
		{
			cmd = RN_AWR;
			needReq = RN_NOTNEED_REQ;
		}
		else
		{
			cmd = RN_ARD;
			needReq = RN_NEED_REQ;
		}

		m_pSocket->fillPacket(des_id, des_ch, pData, num, cmd, needReq, IsReq, addr, com_mode);							//������ݰ���ͷ
		m_pSocket->FillSocketPacket(m_pSocket->pTx, m_pSocket->m_sts);													//ת����ʽΪsocket����ʽ
		if (0 == m_pSocket->SocketSend())																				//��������
		{
			if (needReq == RN_NEED_REQ)
			{
				//�ȴ�1s������źţ�˵���õ��˷���ֵ
				if (WaitForSingleObject(Com_Rcv_Event, 1000)== WAIT_OBJECT_0)
				{
					memcpy_s(pData, num*sizeof(int16), parsePacket, num*sizeof(int16));
				}
				else
				{
					ret = SOCKET_Rt_Timeout;
				}
			}
			else
			{
				ret = Rt_Success;
			}	
		}
		else
		{
			ret = SOCKET_Rt_Send_Err;
		}
	}

	return ret;
}

int16 CSocketInterface::SocketCom_DSP_ComHandler(int16 mode, int16 addr, int16* pData, int16 num, int16 des_id)
{
	int16 ret = Rt_Success;
	if (NULL != m_pSocket)									//�����Ƿ�����
	{
		int32 iret = -1;

		if (num > (RN_MAX_PACKET_LEN >> 1))
			return RN_Net_Rt_Send_Data_TooLong;

		//���ݵ�ַ�жϣ�����֮ǰ��PCdebug�ĵ�ַ����Ȼ�����ַ����û���ô�
		int16 des_ch;
		if ((Uint16)addr == (Uint16)VIRTUAL_DSPA_COMADDR)
		{
			des_ch = RN_DSP_CH_ID;
		}
		else if ((Uint16)addr == (((Uint16)VIRTUAL_DSPB_COMADDR) & 0xffff))
		{
			des_ch = RN_PCI_CH_ID;
		}

		//����ģʽ����cmd���Ƿ���Ҫ��Ӧ
		int16 cmd;
		int16 needReq;
		int16 com_mode = RN_MODE_MAIL;
		int16 IsReq = RN_ISNOT_REQ;
		if (DSP_MODE_RD == mode)
		{
			cmd = RN_ARD;
			needReq = RN_NEED_REQ;
		}
		else if (DSP_MODE_WR == mode)
		{
			cmd = RN_AWR;
			needReq = RN_NEED_REQ;
		}
		else
		{
			cmd = RN_ARD;
			needReq = RN_NEED_REQ;
		}

		int16 order = *pData;
		int32 delay_time = 1000;
		//////////////////////////////////////////////////////////////////////////
		//�������ͬ���ӳٵȴ�ʱ�䲻ͬ
		switch (order & 0x0fff)
		{
			//���г�ʱ���ָ����ڴ�����
		case 38:
		case 39:
			delay_time = 120000;
			break;
		default:
			break;
		}

		m_pSocket->fillPacket(des_id, des_ch, pData, num, cmd, needReq, IsReq, addr, com_mode);								//������ݰ���ͷ
		if (0 == m_pSocket->SocketSend())																				//��������
		{
			if (needReq == RN_NEED_REQ)
			{
				//�ȴ�1s������źţ�˵���õ��˷���ֵ
				if (WaitForSingleObject(Com_Rcv_Event, delay_time) == WAIT_OBJECT_0)
				{
					//�����dsp��Ӧ������ӵ�������ʼȡ����Ϊ֮ǰ����������ŵ��ڽ��������Ѿ���֤������������֮ǰ���Ǽ��ݵġ�
					memcpy_s(pData, num*sizeof(int16), &parsePacket[3], num*sizeof(int16));
				}
				else
				{
					ret = SOCKET_Rt_Timeout;
				}
			}
			else
			{
				ret = Rt_Success;
			}
		}
		else
		{
			ret = SOCKET_Rt_Send_Err;
		}
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////