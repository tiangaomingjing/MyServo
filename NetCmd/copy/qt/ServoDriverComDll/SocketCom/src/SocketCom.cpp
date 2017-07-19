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

//winsock是一种标准API，主要用于网络数据通信，它允许两个或者多个应用程序或者进程再同一机器上
//或者网络间相互通信,它是一种接口，不是协议。

//winsock1和winsock2两个版本，通过WSA可以区分这两个版本的函数，winsock2带有WSA前缀。
//有几个例外的函数WSAStartup,WSACleanup,WSARecvEx ,WSAGetLastError都输出Winsock1.1的函数

//Windows CE .NET 4.1 and later supports Winsock 2.2, which provides easier access to multiple transport protocols.
//Winsock 2.2, with its functions exported from Ws2.dll, is not limited to TCP / IP protocol stacks 
//as is the case for Winsock 1.1 现在我们已经使用wince6.0，不需要考虑winsock1.1的问题了

//The development of Winsock 1.1 compatible applications has been deprecated since Windows 2000 was released.
//All applications should now use use the include Winsock2.h directive in Winsock application source files.

//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
//WinSock2.h contains most of the Winsock functions, structures, and definitions
//使用winsock2规范
//The Ws2tcpip.h header file contains definitions introduced in the WinSock 2 Protocol - Specific Annex document for TCP / IP
//that includes newer functions and structures used to retrieve IP addresses 
//假如不使用这些函数可以不包含Ws2tcpip.h头文件
//The Mswsock.h header file contains definitions for Microsoft-specific extensions to the Windows Sockets 2
//假如不使用这些函数可以不包含Mswsock.h头文件
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
	initSocket							= NULL;													//初始化socket
	m_SocketRcvThreadHandle				= NULL;													//socket线程handler

	m_tSocketInfo.strIPAddress			= "";
	m_socketClientHandle				= INVALID_SOCKET;
	m_socketSrvHandle					= INVALID_SOCKET;
	m_tSocketInfo.uiSocketType			= SOCKET_TYPE_NONE;
	m_tSocketInfo.uiThListen			= 0;													//监听标志初始化为0
	
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
	//socket初始化
	initSocket = new CInitSock;

	//客户端需要通过TCP通信的时候，必须指定服务器的IP地址和服务端口号，
	//服务器打算监听客户端请求的时候，也必须指定一个IP地址和端口号
	//Windows CE.NET 4.1 and later provides a dual protocol stack to support 32 - bit IPv4 addresses and 128 - bit IPv6 addresses
	//this struct used for IPV4, include IP Address and port message
	SOCKADDR_IN addrServer;

	//根据ip地址长度来区分是客户端还是服务器
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

		//client part ，you can use connect to connect to server part
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
		//套接字是传输提供程序的句柄
		//第一个参数是协议的地址族
		//第二个参数是套接字类型
		//第三个参数是protocol
		// protocol:If a value of 0 is specified, the caller does not wish to specify a protocol and 
		//the service provider will choose the protocol to use.
		//change the third param from 0 to IPPROTO_TCP,具体化
		m_socketSrvHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
		if (m_socketSrvHandle == INVALID_SOCKET)
		{
			printf("socket error = %d\n", WSAGetLastError());
			WSACleanup();
			return SOCKET_ERROR;
		}
		
		//set the address_in struct
		addrServer.sin_addr.S_un.S_addr		= htonl(INADDR_ANY);	//不指定客户端的IP，可以随便连，主机字节序改为网络字节序，并且允许将套接字绑定到
																	//所有可用的接口，以便将来传到任意（正确）接口上的客户机连接都可以被监听套接字接受
		addrServer.sin_family				= AF_INET;				//协议地址族	
		addrServer.sin_port					= htons(uiPort);		//需要具体端口号，表示服务器在该端口提供服务		

		//bind 服务端必须要绑定端口，可以不指定ip地址
		if (SOCKET_ERROR == bind(m_socketSrvHandle, (SOCKADDR*)&addrServer, sizeof(SOCKADDR)))
		{
			printf("socket error = %d\n", WSAGetLastError());
			WSACleanup();
			return SOCKET_ERROR;
		}

		//listen 监听客户端,第二个参数是同时来的客户端请求时，队列中最多能够存储多少个请求，多的会丢弃
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
	//填写socket信息
	m_tSocketInfo.strIPAddress	= strIpAddress; //服务器的ip不需输入，为空即可
	m_tSocketInfo.uiComPort		= uiPort;

	//create thread and the thread is suspended
	if (NULL == m_SocketRcvThreadHandle)
	{
		//创建线程
		m_SocketRcvThreadHandle = CreateThread(NULL,
			0,
			(LPTHREAD_START_ROUTINE)socketThread,
			this,
			CREATE_SUSPENDED,
			&m_dwThreadID);

		//设置优先级
		SetThreadPriority(m_SocketRcvThreadHandle, THREAD_PRIORITY_TIME_CRITICAL);

		if (NULL == m_SocketRcvThreadHandle)
		{
			printf("thread creat error \n");
			return SOCKET_RT_THREAD_ERR;
		}
	}
	//set listen flag,设置监听标志，服务器等待客户端，客户端虽然不用监听，但是为了代码一致同样在线程中使用该标志
	m_tSocketInfo.uiThListen = 1;

	//resume the thread
	ResumeThread(m_SocketRcvThreadHandle);

	return SOCKET_RT_SUCCESS;

}

//关闭socket用closesocket即可，但是他可能带来负面影响（和如何调用它有关，可能导致数据的丢失）
//应该在调用它之前，利用shutdown从容的终止连接。当然并不是所有面向连接的协议都支持从容关闭，
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
	//server listen,客户端共用该标志
	while (m_tSocketInfo.uiThListen)
	{	
		//if i am a server,i will wait for client at first.服务器在没有客户端连接之前标志为0
		if(0 == m_tSocketInfo.uiConnectFlag)
		{
			//the client address will be in the struct SOCKADDR_IN
			SOCKADDR_IN addrclient; 
			int32 sockeLen = sizeof(SOCKADDR);

			//accept the client connect,accept返回新的套接字，这个套接字就是对应该客户端，原来那个监听的套接字可以继续监听
			//但是这里就限定为1对1
			m_socketClientHandle = accept(m_socketSrvHandle,(SOCKADDR*)&addrclient,(int*)&sockeLen);
			if (INVALID_SOCKET == m_socketSrvHandle)
			{
				printf("socket error = %d\n", WSAGetLastError());
				return SOCKET_ERROR;
			}
			m_tSocketInfo.uiConnectFlag = TRUE;
		}

		//先获取接收数据的个数
		Uint32 lenth = 0;
		rtn = AS_recv(m_socketClientHandle, (char *)&lenth, 4, 0);
		if (SOCKET_ERROR == rtn)
		{
			printf("socket error = %d\n", WSAGetLastError());
			return SOCKET_ERROR;
		}

		//根据个数，接收数据
		rtn = AS_recv(m_socketClientHandle, (char *)(&m_rt_sts->usdata), lenth, 0);
		if (SOCKET_ERROR == rtn)
		{
			printf("socket error = %d\n", WSAGetLastError());
			return SOCKET_ERROR;
		}
		m_rt_sts->ulByteNum = lenth;

		//将数据转化为等环网格式进行解包
		DecodeSocketPacket(pRx, m_rt_sts);
		decoderPacket((int16*)(&(parsePacket[0])), (int16)m_rt_sts->ulByteNum);
	}
	return 0;
}
//所有关系到收发数据的缓冲区都是属于char类型，不管数据是二进制还是字符型，都无关紧要
//////////////////////////////////////////////////////////////////////////
//内部函数
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
	//使用SD_SEND表示不允许在调用发送函数
	//对于TCP套接字来说，会在所有数据发送出之后，并且在接收端确认后，生成一个FIN包
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
