//////////////////////////////////////////////////////////////////////////////////////////
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

const int32			Rt_Success					=	0x0000;					//ͨ�÷��سɹ�

const int32			Net_Rt_Execute_Fail			=	0x0001;					//dsp�����ִ�д���
const int32			Net_Rt_Execute_Success		=	0x0002;					//dsp�����ִ�гɹ�
const int32			Net_Rt_Parameter_invalid	=   0x0003;					//dsp����ز�����Ч
const int32			Net_Rt_Instruction_invalid	=	0x0004;					//dsp�����ָ����Ч
const int32			Net_Rt_Other_Error			=	0x0005;					//dsp�����ָ����Ч

const int32 		Net_Rt_Index_Unmatch		=	0x0006;					//����ֵƥ��ʧ��
const int32 		Net_Rt_Receive_Unknow_Data	=	0x0007;					//δ֪��������
const int32 		Net_Rt_Receive_DLenth_short =	0x0008;					//�������ݳ��ȹ���

const int32 		Net_Rt_Not_Find_Net_Device	=	0x0009;					//û���ҵ������豸
const int32 		Net_Rt_Not_Get_Net_DevList	=	0x000A;					//�������豸�б�ʧ��
const int32 		Net_Rt_Adapter_Not_Match	=	0x000B;					//�豸�б��е����ƺ��豸��ƥ��
const int32 		Net_Rt_Open_Device_Err		=	0x000C;                 //�������豸����
const int32 		Net_Rt_Complie_Err			=	0x000d;					//�������
const int32 		Net_Rt_SetFilter_Err		=	0x000e;					//�����˲�������

const int32 		Net_Rt_Send_Err				=	0x000f;                 //�����ʧ��
const int32 		Net_Rt_Receive_Err			=	0x0010;				    //��������ʧ��

const int32			Net_Rt_Not_Enough_Space		=	0x0011;					//�����������ռ䲻��
const int32 		Net_Rt_Net_DevListLenth_Err =	0x0012;					//�����豸�б��ȴ���
const int32 		Net_Rt_Pointer_Invalid		=	0x0013;				    //����ָ����Ч
const int32 		Net_Rt_MBTOWC_Err			=	0x0014;					//���ֽ�ת��Ϊ���ַ�����
const int32 		Net_Rt_Not_Find_ScanDevice	=	0x0015;					//û�д�ɨ�赽���豸�б����ҵ�����Ӧ���豸
const int32 		Net_Rt_Send_Data_TooLong	=	0x0016;					//�������ݳ��ȳ���1023��bytes��
const int32 		Net_Rt_Timeout				=	0x0017;					//�ȴ���ʱ
const int32 		Net_Rt_Mac_Err				=	0x0018;					//MAC��ַ����
const int32 		Net_Rt_Flag_Err				=	0x0019;					//��д��ɱ�־λδ��
const int32 		Net_Rt_CreateObj_Err		=	0x001A;					//�����������

const int32 		Net_Rt_Lock_Err				=	0x001B;					//LOCK ERR
const int32 		Net_Rt_param_Err			=	0x001c;					//param ERR

#endif