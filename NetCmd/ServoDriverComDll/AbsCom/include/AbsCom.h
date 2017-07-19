﻿//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Abstract Communicaiton layer Define		 							//
//	file				:	AbsCom.h													//
//	Description			:	use for pc and fpga communicaiton							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef		__GTSD_ABSCOM_DLL__
#define		__GTSD_ABSCOM_DLL__

#include "NetCommunication.h"
#include "Rninterface.h"
#include "SocketInterface.h"

/***********************************************************************/
/* 定义返回值类型                                                       */
/************************************************************************/
typedef enum com_rt_type
{
	GTSD_COM_SUCCESS,								//函数返回成功
	GTSD_COM_OPEN_ERR,								//打开设备失败
	GTSD_COM_CLOSE_ERR,								//关闭设备失败
	GTSD_COM_FIRMWARE_ERR,							//固件通信错误
	GTSD_COM_PROCESSOR_ERR							//处理器通信错误
}COM_RT_TYPE;

/************************************************************************/
/* 定义操作模式                                                         */
/************************************************************************/

typedef enum opt_type
{
	GTSD_COM_MODE_READ,			//读操作
	GTSD_COM_MODE_WRITE			//写操作
}OPT_TYPE;


class CAbsCom
{
public:
	CAbsCom(void);
	~CAbsCom(void);

public:

	CNetCom*			m_pNetCom;
	CRnNetInterface*	m_pRnNetCom;
	CSocketInterface*   m_pSocketCom;

	int16	com_flag;
	/*******************************************************************************************
	功能：		打开通信设备
	输入：
  comType:	通信设备的类型，0：裸机网口 ：1:RN_NET 2：usb转串口 3:虚拟设备
	输出：		无
	返回：		0成功，其他参看错误列表。
	*******************************************************************************************/
	int16 GTSD_Com_Open(void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 comType);
	/*******************************************************************************************
	功能：		关闭通信设备
	输入：
	comType:	通信设备的类型，0：裸机网口 1：TCP/IP协议网口 2：usb转串口 3:虚拟设备
	返回：		0成功，其他参看错误列表。
	*******************************************************************************************/
	int16 GTSD_Com_Close(int16 comType);
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
	int16 GTSD_Com_Firmware_handler(int16 comType, int16 comMode, int16 comAddr, int16* pData, int16 comNum, int16 stationId);
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
	int16 GTSD_Com_Processor_handler(int16 comType, int16 comMode, int16 comAddr, int16* pData, int16 comNum, int16 stationId);

};

#endif
