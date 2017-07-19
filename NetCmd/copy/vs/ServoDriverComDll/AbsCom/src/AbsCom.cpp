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
功能：		打开通信设备
输入：
comType:	通信设备的类型，0：裸机网口 1：TCP/IP协议网口 2：usb转串口 3:虚拟设备
输出：		无
返回：		0成功，其他参看错误列表。
*******************************************************************************************/
int16 CAbsCom::GTSD_Com_Open(int16 comType)
{
	if (NULL != m_pNetCom) //如果设备已经打开，直接返回
		return Rt_Success;

	//定义对象
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
功能：		关闭通信设备
输入：
comType:	通信设备的类型，0：裸机网口 1：TCP/IP协议网口 2：usb转串口 3:虚拟设备
返回：		0成功，其他参看错误列表。
*******************************************************************************************/
int16  CAbsCom::GTSD_Com_Close(int16 comType)
{
	//假如已经关闭了就直接退出
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
功能：		固件(fpga/cpld)数据通信交互函数
输入：
comType:	通信设备的类型，0：裸机网口 1：TCP/IP协议网口 2：usb转串口 3:虚拟设备
comMode:	1:写  0：读
comAddr:	通信地址
pData:		输入数据
comNum:		输入数据长度
输出：
pData:		输出数据
comNum:		输出数据长度
返回：		0成功，其他参看错误列表。
*******************************************************************************************/
int16  CAbsCom::GTSD_Com_Firmware_handler(int16 comType, int16 comMode, int16 comAddr, int16* pData, int16 comNum)
{
	//假如已经关闭了就直接退出
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
功能：		处理器（DSP/ARM）数据通信交互函数
输入：
comType:	通信设备的类型，0：裸机网口 1：TCP/IP协议网口 2：usb转串口 3:虚拟设备
comMode:	1:写  0：读
comAddr:	通信地址
pData:		输入数据
comNum:		输入数据长度
输出：
pData:		输出数据
comNum:		输出数据长度
返回：		0成功，其他参看错误列表。
*******************************************************************************************/
int16  CAbsCom::GTSD_Com_Processor_handler(int16 comType, int16 comMode, int16 comAddr, int16* pData, int16 comNum)
{
	//假如已经关闭了就直接退出
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

