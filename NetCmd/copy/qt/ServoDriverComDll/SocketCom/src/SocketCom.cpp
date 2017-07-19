//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	socket Communicaiton		 								//
//	file				:	GoogolSocket.cpp												//
//	Description			:	use for pc using socket com									//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/4	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

//winsock��һ�ֱ�׼API����Ҫ������������ͨ�ţ��������������߶��Ӧ�ó�����߽�����ͬһ������
//����������໥ͨ��,����һ�ֽӿڣ�����Э�顣

//winsock1��winsock2�����汾��ͨ��WSA���������������汾�ĺ�����winsock2����WSAǰ׺��
//�м�������ĺ���WSAStartup,WSACleanup,WSARecvEx ,WSAGetLastError�����Winsock1.1�ĺ���

//Windows CE .NET 4.1 and later supports Winsock 2.2, which provides easier access to multiple transport protocols.
//Winsock 2.2, with its functions exported from Ws2.dll, is not limited to TCP / IP protocol stacks 
//as is the case for Winsock 1.1 ���������Ѿ�ʹ��wince6.0������Ҫ����winsock1.1��������

//The development of Winsock 1.1 compatible applications has been deprecated since Windows 2000 was released.
//All applications should now use use the include Winsock2.h directive in Winsock application source files.

//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//WinSock2.h contains most of the Winsock functions, structures, and definitions
//ʹ��winsock2�淶
//The Ws2tcpip.h header file contains definitions introduced in the WinSock 2 Protocol - Specific Annex document for TCP / IP
//that includes newer functions and structures used to retrieve IP addresses 
//���粻ʹ����Щ�������Բ�����Ws2tcpip.hͷ�ļ�
//The Mswsock.h header file contains definitions for Microsoft-specific extensions to the Windows Sockets 2
//���粻ʹ����Щ�������Բ�����Mswsock.hͷ�ļ�
//The Winsock2.h header file internally includes core elements from the Windows.h header file, 
//so there is not usually an #include line for the Windows.h header file in Winsock applications.
//If an #include line is needed for the Windows.h header file, this should be preceded with the
//#define WIN32_LEAN_AND_MEAN macro.For historical reasons, the Windows.h header defaults to 
//including the Winsock.h header file for Windows Sockets 1.1.The declarations in the Winsock.h 
//header file will conflict with the declarations in the Winsock2.h header file required by 
//Windows Sockets 2. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included 
//by the Windows.h header.

#include <WinSock2.h>														
//#include <WS2tcpip.h>
//#include <MSWSock.h>

#include "SocketCom.h"
#include "RingNetCom.h"
 
//////////////////////////////////////////////////////////////////////////
//The ws2_32.lib file is for Winsock 2, while wsock32.lib is for the obsolete, older version.
#ifndef _WIN32_WCE
	// codes goes here for compilation of code that is 
	// not for Windows CE
	#pragma comment(lib,"ws2_32.lib")
#else
	#if (_WIN32_WCE < 600)
		// Code for versions earlier than CE 6.0 goes here
		#pragma comment(lib,"Ws2.lib")
	#else
		// Code for versions of CE 6.0 and later versions goes here
		#pragma comment(lib,"Ws2.lib")
	#endif
#endif

//////////////////////////////////////////////////////////////////////////
CSocketCom::CSocketCom()
{
	initSocket							= NULL;													//��ʼ��socket
	m_SocketRcvThreadHandle				= NULL;													//socket�߳�handler

	m_tSocketInfo.strIPAddress			= "";
	m_socketClientHandle				= INVALID_SOCKET;
	m_socketSrvHandle					= INVALID_SOCKET;
	m_tSocketInfo.uiSocketType			= SOCKET_TYPE_NONE;
	m_tSocketInfo.uiThListen			= 0;													//������־��ʼ��Ϊ0
	
	//set select function timeout
	m_tvTimeoutSend.tv_sec				= 0;															
	m_tvTimeoutSend.tv_usec				= 10;													//10us
	m_tvTimeoutRec.tv_sec				= 0;
	m_tvTimeoutRec.tv_usec				= 10;													//10us
}

CSocketCom::~CSocketCom()
{

}

int16 CSocketCom::SocketOpen(string strIpAddress, Uint16 uiPort /*= SOCKET_COM_PORT*/)
{
	//socket��ʼ��
	initSocket = new CInitSock;

	//�ͻ�����Ҫͨ��TCPͨ�ŵ�ʱ�򣬱���ָ����������IP��ַ�ͷ���˿ںţ�
	//��������������ͻ��������ʱ��Ҳ����ָ��һ��IP��ַ�Ͷ˿ں�
	//Windows CE.NET 4.1 and later provides a dual protocol stack to support 32 - bit IPv4 addresses and 128 - bit IPv6 addresses
	//this struct used for IPV4, include IP Address and port message
	SOCKADDR_IN addrServer;

	//����ip��ַ�����������ǿͻ��˻��Ƿ�����
	if (strIpAddress.length() > 4)//client
	{ 
		//change the third param from 0 to IPPROTO_TCP 
		m_socketClientHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
		if (INVALID_SOCKET == m_socketClientHandle)
		{
			printf("socket error = %d\n", WSAGetLastError());
			WSACleanup();
			return SOCKET_ERROR;
		}

		// IP address
		inet_pton(AF_INET, strIpAddress.c_str(), &(addrServer.sin_addr.S_un.S_addr)); //inet_addr(strIpAddress.c_str());
		//this must be set AF_INET to tell the winsock that we are using IPv4 address
		addrServer.sin_family = AF_INET;
		//host order to internet order, to change the port byte order to internet order 
		addrServer.sin_port = htons(uiPort);

		//client part ��you can use connect to connect to server part
		if (SOCKET_ERROR == connect(m_socketClientHandle, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)))
		{	
			printf("socket error = %d\n", WSAGetLastError());
			WSACleanup();
			return SOCKET_ERROR;
		}

		//set flag
		m_tSocketInfo.uiConnectFlag = TRUE;
		m_tSocketInfo.uiSocketType	= SOCKET_TYPE_CLIENT;
	}
	else
	{
		//server part
		//�׽����Ǵ����ṩ����ľ��
		//��һ��������Э��ĵ�ַ��
		//�ڶ����������׽�������
		//������������protocol
		// protocol:If a value of 0 is specified, the caller does not wish to specify a protocol and 
		//the service provider will choose the protocol to use.
		//change the third param from 0 to IPPROTO_TCP,���廯
		m_socketSrvHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
		if (m_socketSrvHandle == INVALID_SOCKET)
		{
			printf("socket error = %d\n", WSAGetLastError());
			WSACleanup();
			return SOCKET_ERROR;
		}
		
		//set the address_in struct
		addrServer.sin_addr.S_un.S_addr		= htonl(INADDR_ANY);	//��ָ���ͻ��˵�IP������������������ֽ����Ϊ�����ֽ��򣬲��������׽��ְ󶨵�
																	//���п��õĽӿڣ��Ա㽫���������⣨��ȷ���ӿ��ϵĿͻ������Ӷ����Ա������׽��ֽ���
		addrServer.sin_family				= AF_INET;				//Э���ַ��	
		addrServer.sin_port					= htons(uiPort);		//��Ҫ����˿ںţ���ʾ�������ڸö˿��ṩ����		

		//bind ����˱���Ҫ�󶨶˿ڣ����Բ�ָ��ip��ַ
		if (SOCKET_ERROR == bind(m_socketSrvHandle, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)))
		{
			printf("socket error = %d\n", WSAGetLastError());
			WSACleanup();
			return SOCKET_ERROR;
		}

		//listen �����ͻ���,�ڶ���������ͬʱ���Ŀͻ�������ʱ������������ܹ��洢���ٸ����󣬶�Ļᶪ��
		if (SOCKET_ERROR == listen(m_socketSrvHandle, SOCKET_MAX_CONNECT_QUEEN_NUM))
		{
			printf("socket error = %d\n", WSAGetLastError());
			WSACleanup();
			return SOCKET_ERROR;
		}
		//set flag
		m_tSocketInfo.uiConnectFlag = FALSE;
		m_tSocketInfo.uiSocketType	= SOCKET_TYPE_SERVER;
	}
	//��дsocket��Ϣ
	m_tSocketInfo.strIPAddress	= strIpAddress; //��������ip�������룬Ϊ�ռ���
	m_tSocketInfo.uiComPort		= uiPort;

	//create thread and the thread is suspended
	if (NULL == m_SocketRcvThreadHandle)
	{
		//�����߳�
		m_SocketRcvThreadHandle = CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)socketThread,
			this,
			CREATE_SUSPENDED,
			&m_dwThreadID);

		//�������ȼ�
		SetThreadPriority(m_SocketRcvThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);

		if (NULL == m_SocketRcvThreadHandle)
		{
			printf("thread creat error \n");
			return SOCKET_RT_THREAD_ERR;
		}
	}
	//set listen flag,���ü�����־���������ȴ��ͻ��ˣ��ͻ�����Ȼ���ü���������Ϊ�˴���һ��ͬ�����߳���ʹ�øñ�־
	m_tSocketInfo.uiThListen = 1;

	//resume the thread
	ResumeThread(m_SocketRcvThreadHandle);

	return SOCKET_RT_SUCCESS;

}

//�ر�socket��closesocket���ɣ����������ܴ�������Ӱ�죨����ε������йأ����ܵ������ݵĶ�ʧ��
//Ӧ���ڵ�����֮ǰ������shutdown���ݵ���ֹ���ӡ���Ȼ�����������������ӵ�Э�鶼֧�ִ��ݹرգ�
int16 CSocketCom::SocketClose(void)
{
	////stop the thread
	//if (NULL != m_SocketRcvThreadHandle)
	//{
	//	TerminateThread(m_SocketRcvThreadHandle, 0);
	//	m_SocketRcvThreadHandle = NULL;
	//}

	m_tSocketInfo.uiThListen = 0;
	Sleep(300);

	AS_close(m_socketClientHandle);
	AS_close(m_socketSrvHandle);

	//set the socket INVALID_SOCKET
	m_socketClientHandle	= INVALID_SOCKET;
	m_socketSrvHandle		= INVALID_SOCKET;

	delete initSocket;
	return 0;
}

int16 CSocketCom::SocketSend()
{
	Uint32 ulByteNum = 0;
	int16 rtn;

	//send data package to target
	rtn = AS_send(m_socketClientHandle, (char *)(m_sts->ulByteNum), (m_sts->ulByteNum+4), 0);
	if (SOCKET_ERROR == rtn)
	{
		printf("socket error = %d\n", WSAGetLastError());
		return SOCKET_ERROR;
	}

	return 0;
}

void CSocketCom::socketThread(LPVOID lpParam)
{
	CSocketCom* pSocket = (CSocketCom*)lpParam;
	pSocket->socketThreadFunc();
}

int16 CSocketCom::socketThreadFunc()
{
	int16 rtn;
	//server listen,�ͻ��˹��øñ�־
	while (m_tSocketInfo.uiThListen)
	{	
		//if i am a server,i will wait for client at first.��������û�пͻ�������֮ǰ��־Ϊ0
		if(0 == m_tSocketInfo.uiConnectFlag)
		{
			//the client address will be in the struct SOCKADDR_IN
			SOCKADDR_IN addrclient; 
			int32 sockeLen = sizeof(SOCKADDR);

			//accept the client connect,accept�����µ��׽��֣�����׽��־��Ƕ�Ӧ�ÿͻ��ˣ�ԭ���Ǹ��������׽��ֿ��Լ�������
			//����������޶�Ϊ1��1
			m_socketClientHandle = accept(m_socketSrvHandle,(SOCKADDR*)&addrclient,(int*)&sockeLen);
			if (INVALID_SOCKET == m_socketSrvHandle)
			{
				printf("socket error = %d\n", WSAGetLastError());
				return SOCKET_ERROR;
			}
			m_tSocketInfo.uiConnectFlag = TRUE;
		}

		//�Ȼ�ȡ�������ݵĸ���
		Uint32 lenth = 0;
		rtn = AS_recv(m_socketClientHandle, (char *)&lenth, 4, 0);
		if (SOCKET_ERROR == rtn)
		{
			printf("socket error = %d\n", WSAGetLastError());
			return SOCKET_ERROR;
		}

		//���ݸ�������������
		rtn = AS_recv(m_socketClientHandle, (char *)(&m_rt_sts->usdata), lenth, 0);
		if (SOCKET_ERROR == rtn)
		{
			printf("socket error = %d\n", WSAGetLastError());
			return SOCKET_ERROR;
		}
		m_rt_sts->ulByteNum = lenth;

		//������ת��Ϊ�Ȼ�����ʽ���н��
		DecodeSocketPacket(pRx, m_rt_sts);
		decoderPacket((int16*)(&(parsePacket[0])), (int16)m_rt_sts->ulByteNum);
	}
	return 0;
}
//���й�ϵ���շ����ݵĻ�������������char���ͣ����������Ƕ����ƻ����ַ��ͣ����޹ؽ�Ҫ
//////////////////////////////////////////////////////////////////////////
//�ڲ�����
int16 CSocketCom::AS_send(SOCKET s, const char *buf, int len, int flags)
{
	int rtn;

	//clear the writefd set
	FD_ZERO(&m_fdsSendFds);

	//add s to writefd set
	FD_SET(s, &m_fdsSendFds);

	//select 
	rtn = select(0, NULL, &m_fdsSendFds, NULL, &m_tvTimeoutSend);

	if (SOCKET_ERROR != rtn)
	{
		//can i send command ?
		if (FD_ISSET(s, &m_fdsSendFds))
		{
			rtn = send(s, buf, len, flags);
		}
	}

	return rtn;
}
//////////////////////////////////////////////////////////////////////////
int16 CSocketCom::AS_recv(SOCKET s, char *buf, int len, int flags)
{
	int rtn;
	
	//clear the readfd set
	FD_ZERO(&m_fdsRecFds);

	//add s to the readfd set
	FD_SET(s,&m_fdsRecFds);

	//select
	rtn = select(0,&m_fdsRecFds,NULL,NULL,&m_tvTimeoutRec);
	
	if (SOCKET_ERROR != rtn)
	{	
		//any data come ?
		if(FD_ISSET(s,&m_fdsRecFds)) 
		{
			rtn = recv(s,buf,len,flags);			
		}
	}

	return rtn;
}
//////////////////////////////////////////////////////////////////////////
int16 CSocketCom::AS_close(SOCKET s)
{
	char buff[1024];
	int i;
	int rtn;

	if (INVALID_SOCKET == s)
	{
		return SOCKET_RT_SUCCESS;
	}
	//ʹ��SD_SEND��ʾ�������ڵ��÷��ͺ���
	//����TCP�׽�����˵�������������ݷ��ͳ�֮�󣬲����ڽ��ն�ȷ�Ϻ�����һ��FIN��
	shutdown(s, SD_SEND);
	for (i = 0; i < 25; i++)
	{
		rtn = AS_recv(s, buff, 1024, 0);
		if ((rtn < 1024) || (SOCKET_ERROR == rtn))
		{
			break;
		}
	}
	
	//shutdown(s, SD_BOTH);

	closesocket(s);

	return SOCKET_RT_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
