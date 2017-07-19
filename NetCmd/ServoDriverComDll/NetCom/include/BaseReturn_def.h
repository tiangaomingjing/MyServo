/////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	base return value define		 									//
//	file				:	BaseReturn_def.h												//
//	Description			:	use for Improving the portability of program				//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef		__GTSD_BASE_RETURN_DEFINE__
#define		__GTSD_BASE_RETURN_DEFINE__

#include "Basetype_def.h"



const int32			Rt_Success					=	0x0000;					//通用返回成功

//-------------------------------------------------------------------------------------------------
//pcdebug 协议使用 类似ethcat协议的自定义调试协议

const int32			Net_Rt_Execute_Fail			=	0x0001;					//dsp命令返回执行错误
const int32			Net_Rt_Execute_Success		=	0x0002;					//dsp命令返回执行成功
const int32			Net_Rt_Parameter_invalid	=   0x0003;					//dsp命令返回参数无效
const int32			Net_Rt_Instruction_invalid	=	0x0004;					//dsp命令返回指令无效
const int32			Net_Rt_Other_Error			=	0x0005;					//dsp命令返回指令无效

const int32 		Net_Rt_Index_Unmatch		=	0x0006;					//索引值匹配失败
const int32 		Net_Rt_Receive_Unknow_Data	=	0x0007;					//未知返回数据
const int32 		Net_Rt_Receive_DLenth_short =	0x0008;					//返回数据长度过短

const int32 		Net_Rt_Not_Find_Net_Device	=	0x0009;					//没有找到网络设备
const int32 		Net_Rt_Not_Get_Net_DevList	=	0x000A;					//找网络设备列表失败
const int32 		Net_Rt_Adapter_Not_Match	=	0x000B;					//设备列表中的名称和设备不匹配
const int32 		Net_Rt_Open_Device_Err		=	0x000C;                 //打开网络设备错误
const int32 		Net_Rt_Complie_Err			=	0x000d;					//编译错误
const int32 		Net_Rt_SetFilter_Err		=	0x000e;					//设置滤波器错误

const int32 		Net_Rt_Send_Err				=	0x000f;                 //命令发送失败
const int32 		Net_Rt_Receive_Err			=	0x0010;				    //接收命令失败

const int32			Net_Rt_Not_Enough_Space		=	0x0011;					//输入参数分配空间不够
const int32 		Net_Rt_Net_DevListLenth_Err =	0x0012;					//网络设备列表长度错误
const int32 		Net_Rt_Pointer_Invalid		=	0x0013;				    //输入指针无效
const int32 		Net_Rt_MBTOWC_Err			=	0x0014;					//多字节转换为宽字符错误
const int32 		Net_Rt_Not_Find_ScanDevice	=	0x0015;					//没有从扫描到的设备列表中找到有响应的设备
const int32 		Net_Rt_Send_Data_TooLong	=	0x0016;					//发送数据长度超过1023（bytes）
const int32 		Net_Rt_Timeout				=	0x0017;					//等待超时
const int32 		Net_Rt_Mac_Err				=	0x0018;					//MAC地址错误
const int32 		Net_Rt_Flag_Err				=	0x0019;					//读写完成标志位未置
const int32 		Net_Rt_CreateObj_Err		=	0x001A;					//创建对象错误

const int32 		Net_Rt_Lock_Err				=	0x001B;					//LOCK ERR
const int32 		Net_Rt_param_Err			=	0x001c;					//param ERR

const int32			Net_Rt_SerialPort_Err		=	0x001d;					//serial port ERR   


//--------------------------------------------------------------------------------------------

//等环网返回定义

const int32 		RN_Rt_Receive_DLenth_short	= 0x0008;					//返回数据长度过短
const int32 		RN_Net_Rt_Send_Data_TooLong	= 0x0016;					//发送数据长度过长
const int32 		RN_Net_Rt_Send_Err			= 0x000f;					//命令发送失败
const int32 		RN_Net_Rt_Timeout			= 0x0017;					//等待超时
const int32 		RN_Net_Rt_CreateObj_Err		= 0x001A;					//创建对象错误
const int32 		RN_Net_Rt_Complie_Err		= 0x000d;					//编译错误
const int32 		RN_Net_Rt_SetFilter_Err		= 0x000e;					//设置滤波器错误


//socket返回定义

const int32 		SOCKET_Rt_CreateObj_Err		= 0x001A;					//创建对象错误
const int32 		SOCKET_Rt_Open_Device_Err	= 0x000C;					//打开网络设备错误
const int32 		SOCKET_Rt_Com_Err			= 0x001F;					//socket通信出错
const int32 		SOCKET_Rt_Timeout			= 0x0017;					//等待超时
const int32 		SOCKET_Rt_Send_Err			= 0x000f;					//命令发送失败

#endif
