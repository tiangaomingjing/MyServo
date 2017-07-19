//////////////////////////////////////////////////////////////////////////////////////////
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

/************************************************************************/
/* ���巵��ֵ����                                                       */
/************************************************************************/
typedef enum com_rt_type
{
	GTSD_COM_SUCCESS,								//�������سɹ�
	GTSD_COM_OPEN_ERR,								//���豸ʧ��
	GTSD_COM_CLOSE_ERR,								//�ر��豸ʧ��
	GTSD_COM_FIRMWARE_ERR,							//�̼�ͨ�Ŵ���
	GTSD_COM_PROCESSOR_ERR							//������ͨ�Ŵ���
}COM_RT_TYPE;

/************************************************************************/
/* �������ģʽ                                                         */
/************************************************************************/

typedef enum opt_type
{
	GTSD_COM_MODE_READ,			//������
	GTSD_COM_MODE_WRITE			//д����
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
	���ܣ�		��ͨ���豸
	���룺
	comType:	ͨ���豸�����ͣ�0��������� 1��TCP/IPЭ������ 2��usbת���� 3:�����豸
	�����		��
	���أ�		0�ɹ��������ο������б���
	*******************************************************************************************/
	int16 GTSD_Com_Open(int16 comType);
	/*******************************************************************************************
	���ܣ�		�ر�ͨ���豸
	���룺
	comType:	ͨ���豸�����ͣ�0��������� 1��TCP/IPЭ������ 2��usbת���� 3:�����豸
	���أ�		0�ɹ��������ο������б���
	*******************************************************************************************/
	int16 GTSD_Com_Close(int16 comType);
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
	���أ�		0�ɹ��������ο������б���
	*******************************************************************************************/
	int16 GTSD_Com_Firmware_handler(int16 comType, int16 comMode, int16 comAddr, int16* pData, int16 comNum, int16 stationId);
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
	���أ�		0�ɹ��������ο������б���
	*******************************************************************************************/
	int16 GTSD_Com_Processor_handler(int16 comType, int16 comMode, int16 comAddr, int16* pData, int16 comNum, int16 stationId);

};

#endif