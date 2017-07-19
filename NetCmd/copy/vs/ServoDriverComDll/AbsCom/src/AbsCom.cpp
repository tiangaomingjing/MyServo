//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Abstract Communicaiton layer Define		 							//
//	file				:	AbsCom.CPP													//
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
#include "BaseReturn_def.h"
#include "ServoDriverComDll.h"
#include "AbsCom.h"

CAbsCom::CAbsCom(void)
{
	m_pNetCom = NULL;
	com_flag = 0;
}

CAbsCom::~CAbsCom(void)
{

}
/*******************************************************************************************
���ܣ�		��ͨ���豸
���룺
comType:	ͨ���豸�����ͣ�0��������� 1��TCP/IPЭ������ 2��usbת���� 3:�����豸
�����		��
���أ�		0�ɹ��������ο������б�
*******************************************************************************************/
int16 CAbsCom::GTSD_Com_Open(int16 comType)
{
	if (NULL != m_pNetCom) //����豸�Ѿ��򿪣�ֱ�ӷ���
		return Rt_Success;

	//�������
	m_pNetCom = new CNetCom;
	if (m_pNetCom == NULL)
	{
		return Net_Rt_CreateObj_Err;
	}
	
	int16 rtn;
	switch (comType)
	{
	case GTSD_COM_TYPE_NET:
		if (GTSD_COM_SUCCESS == m_pNetCom->NetCom_Open())
		{
			com_flag = 1;
			rtn = GTSD_COM_SUCCESS;
		}
		else
		{
			com_flag = 0;
			rtn = GTSD_COM_OPEN_ERR;
		}
		break;
	case GTSD_COM_TYPE_TCPIP:
		break;
	case GTSD_COM_TYPE_USB2UART:
		break;
	case GTSD_COM_TYPE_VIRTUAL:
		break;
	default:
		break;
	}
	return rtn;
}
/*******************************************************************************************
���ܣ�		�ر�ͨ���豸
���룺
comType:	ͨ���豸�����ͣ�0��������� 1��TCP/IPЭ������ 2��usbת���� 3:�����豸
���أ�		0�ɹ��������ο������б�
*******************************************************************************************/
int16  CAbsCom::GTSD_Com_Close(int16 comType)
{
	//�����Ѿ��ر��˾�ֱ���˳�
	if (m_pNetCom == NULL)
	{
		return Rt_Success;
	}

	int16 rtn;
	switch (comType)
	{
	case GTSD_COM_TYPE_NET:
		if (GTSD_COM_SUCCESS == m_pNetCom->NetCom_Close())
		{
			com_flag = 0;
			rtn = GTSD_COM_SUCCESS;
		}
		else
		{
			com_flag = 1;
			rtn = GTSD_COM_CLOSE_ERR;
		}
		break;
	case GTSD_COM_TYPE_TCPIP:
		break;
	case GTSD_COM_TYPE_USB2UART:
		break;
	case GTSD_COM_TYPE_VIRTUAL:
		break;
	default:
		break;
	}
	if (NULL != m_pNetCom)
	{
		delete m_pNetCom;
		m_pNetCom = NULL;
	}
	return rtn;
}

/*******************************************************************************************
���ܣ�		�̼�(fpga/cpld)����ͨ�Ž�������
���룺
comType:	ͨ���豸�����ͣ�0��������� 1��TCP/IPЭ������ 2��usbת���� 3:�����豸
comMode:	1:д  0����
comAddr:	ͨ�ŵ�ַ
pData:		��������
comNum:		�������ݳ���
�����
pData:		�������
comNum:		������ݳ���
���أ�		0�ɹ��������ο������б�
*******************************************************************************************/
int16  CAbsCom::GTSD_Com_Firmware_handler(int16 comType, int16 comMode, int16 comAddr, int16* pData, int16 comNum)
{
	//�����Ѿ��ر��˾�ֱ���˳�
	if (m_pNetCom == NULL)
	{
		return Rt_Success;
	}
	int16 rtn;
	switch (comType)
	{
	case GTSD_COM_TYPE_NET:
		rtn = m_pNetCom->NetCom_FPGA_ComHandler(comMode, comAddr, pData, comNum);
		break;
	case GTSD_COM_TYPE_TCPIP:
		break;
	case GTSD_COM_TYPE_USB2UART:
		break;
	case GTSD_COM_TYPE_VIRTUAL:
		break;
	default:
		break;
	}
	return rtn;
}
/*******************************************************************************************
���ܣ�		��������DSP/ARM������ͨ�Ž�������
���룺
comType:	ͨ���豸�����ͣ�0��������� 1��TCP/IPЭ������ 2��usbת���� 3:�����豸
comMode:	1:д  0����
comAddr:	ͨ�ŵ�ַ
pData:		��������
comNum:		�������ݳ���
�����
pData:		�������
comNum:		������ݳ���
���أ�		0�ɹ��������ο������б�
*******************************************************************************************/
int16  CAbsCom::GTSD_Com_Processor_handler(int16 comType, int16 comMode, int16 comAddr, int16* pData, int16 comNum)
{
	//�����Ѿ��ر��˾�ֱ���˳�
	if (m_pNetCom == NULL)
	{
		return Rt_Success;
	}
	int16 rtn;
	switch (comType)
	{
	case GTSD_COM_TYPE_NET:
		rtn = m_pNetCom->NetCom_DSP_ComHandler(comMode, comAddr, pData, comNum);
		break;
	case GTSD_COM_TYPE_TCPIP:
		break;
	case GTSD_COM_TYPE_USB2UART:
		break;
	case GTSD_COM_TYPE_VIRTUAL:
		break;
	default:
		break;
	}
	return rtn;
}

