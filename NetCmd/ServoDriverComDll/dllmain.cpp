// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

HANDLE GTSD_Event;                 //Event for MultiThread


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:		
		GTSD_Event = CreateEvent(NULL, FALSE, TRUE, NULL);     //Create an Auto-Reset Event
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		CloseHandle(GTSD_Event);									//Close the Event Handle
		break;
	}
	return TRUE;
}

