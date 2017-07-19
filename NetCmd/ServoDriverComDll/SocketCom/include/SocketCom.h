//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	socket Communicaiton		 								//
//	file				:	GoogolSocket.h												//
//	Description			:	use for pc using socket com									//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2017/1/4	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

//ip internet protocol ��һ��������Э�飬������ȷ�����ݴ���ĳɹ�
//�����߼�Э��TCP(Transmission control protocol�������Э��)��UDP(User datagram protocol �û����ݱ�Э��)
//ͨ��IP�����������Ӻ������ӵ�����ͨ��
//winsock�����ֻ�����ͨ�ż������������ӵ�ͨ�ź������ӵ�ͨ��
//����������ͨ��TCP/IPЭ����ɵģ�TCP�ṩ�����������֮��ɿ���������ݴ��䣬ʹ��TCP��ʱ�򣬼����֮��Ὠ��һ�����������

#ifndef	__GTSD_SOCKET_COM__
#define __GTSD_SOCKET_COM__

#include <WinSock2.h>
#include "SocketPacket.h"

//////////////////////////////////////////////////////////////////////////
//ÿ��winsockӦ�ö�������غ��ʵ�winsock dll �汾�����û�м��ؾͻ᷵��SOCKET_ERROR
//�������ʵ��winsock��ļ���

class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		// ��ʼ��WS2_32.dll   
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer, majorVer);
		int err;
		//The WSAStartup function must be the first Windows Sockets function called by an application or DLL.
		//wVersionRequested :The highest version of Windows Sockets specification that the caller can use. 
		//The high-order byte specifies the minor version number; 
		//the low-order byte specifies the major version number.
		//lpWSAData :A pointer to the WSADATA data structure that is to receive details of the Windows Sockets implementation.
		
		//wHighVersion:The highest version of the Windows Sockets specification that the Ws2_32.dll can support
		err = WSAStartup(sockVersion, &wsaData);
		if (err != 0)
		{
			/* Tell the user that we could not find a usable */
			/* Winsock DLL.                                  */
			//this part can not use WSAGetLastError
			printf("WSAStartup failed with error: %d\n", err);
			exit(0);
		}
		/* Confirm that the WinSock DLL supports 2.2.*/
		/* Note that if the DLL supports versions greater    */
		/* than 2.2 in addition to 2.2, it will still return */
		/* 2.2 in wVersion since that is the version we      */
		/* requested.                                        */

		if (LOBYTE(wsaData.wVersion) != majorVer || HIBYTE(wsaData.wVersion) != minorVer)
		{
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			printf("Could not find a usable version of Winsock.dll\n");
			WSACleanup();
			exit(0);
		}
		else
		{
			printf("The Winsock %d.%d dll was found okay\n", majorVer, minorVer);
		}
	}
	~CInitSock()
	{
		WSACleanup();
	}
};
/////////////////////////////////////////////////////////////////////////////////////
#define SOCKET_MAX_CONNECT_QUEEN_NUM	  5				//���Ӷ����пɻ����������
#define SOCKET_COM_PORT					  5349			//communication port 1024 ~ 49151����ͨ�û�ע��Ķ˿ں�

/////////////////////////////////////////////////////////////////////////////////////
//socket type
#define SOCKET_TYPE_NONE		(0)
#define SOCKET_TYPE_SERVER		(1)
#define SOCKET_TYPE_CLIENT		(2)

/////////////////////////////////////////////////////////////////////////////////////
//error return table
//#define SOCKET_ERROR  (-1)	has define in Winsock2.h
#define SOCKET_RT_SUCCESS		(0)	
#define SOCKET_RT_THREAD_ERR	(1)

//////////////////////////////////////////////////////////////////////////
//socket info define
struct TSocketInfo
{
	Uint16 uiComPort;																	//communication port
	Uint16 uiSocketType;																//socket type,0--none,1--server,other--client
	Uint16 uiConnectFlag;																//0--not connected,other--connected
	Uint16 uiThListen;																	//0--no listen thread is active,1---yes 
	string strIPAddress;																//server IP or client IP(if i am server)
};
//////////////////////////////////////////////////////////////////////////
class CSocketCom : public CSocketPacket
{
public:
	CSocketCom();
	~CSocketCom();
public:
	int16 SocketOpen(string strIpAddress, Uint16 uiPort = SOCKET_COM_PORT);
	int16 SocketClose(void);
	int16 SocketSend();

private:
	static void socketThread(LPVOID lpParam);
	int16 AS_close(SOCKET s);
	int16 AS_recv(SOCKET s, char FAR* buf, int len, int flags);
	int16 AS_send(SOCKET s, const char FAR * buf, int len, int flags);

private:
	//socket
	CInitSock* initSocket;																//socket init
	TSocketInfo m_tSocketInfo;															//socket info struct define
	SOCKET m_socketClientHandle;														//define socket client object
	SOCKET m_socketSrvHandle;															//define socket server object
	
	//thread
	HANDLE m_SocketRcvThreadHandle;														//thread handle
	Uint32 m_dwThreadID;																//thread id	
	int16  socketThreadFunc();															//socket thread function

	//async select function 
	struct fd_set m_fdsSendFds;
	struct fd_set m_fdsRecFds;
	struct timeval m_tvTimeoutSend;
	struct timeval m_tvTimeoutRec;
};

#endif // _GOOGOLTECH_DRIVER_SOCKET_
