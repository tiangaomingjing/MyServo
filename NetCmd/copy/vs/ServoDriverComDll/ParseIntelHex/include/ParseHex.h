//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	parse intel hex file	 		 							//
//	file				:	ParseHex.h													//
//	Description			:	parse intel hex file										//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef __GTSD_PARSE_HEX__
#define __GTSD_PARSE_HEX__

/*
Intel Hex�ļ�������

Hex�ļ��ĸ�ʽ���£�

RecordMark	RecordLength	LoadOffset	RecordType	Data	Checksum

��Intel Hex�ļ��У�RecordMark�涨Ϊ��:��

*/
#include "Basetype_def.h"
#include <vector>
using namespace std;
#include "ServoDriverComDll.h"

class Hex
{
public:	
	Hex(int8 mark);
	~Hex();

	static const int32		  MAX_BUFFER_SIZE			= 43;
	Uint32	m_baseAddr;
	Uint64  dataLenth;
	int16	CompareFlash(vector<INTEL_HEX_FRAME>* frame_w, vector<INTEL_HEX_FRAME>* frame_r);
	int16	WriteFlash(int16 axis, vector<INTEL_HEX_FRAME>* frame, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type);
	int16	ReadFlash(int16 axis, vector<INTEL_HEX_FRAME>* frame, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type);
	int16	WriteHexFile(string filename, vector<INTEL_HEX_FRAME>* frame);
	int16	ParseHex(string filename);					//����Hex�ļ�
	int16	ParseRecord(int8 ch);					//����ÿһ����¼
	Uint16	GetRecordLength();					//��ȡ��¼����
	int8	GetRecordMark();						//��ȡ��¼��ʶ
	Uint32	GetLoadOffset();						//��ȡ�ڴ�װ��ƫ��
	Uint16	GetRecordType();						//��ȡ��¼����
	int16*	GetData();							//��ȡ����
	Uint16	GetChecksum();						//��ȡУ���
	int16	ResetVar();							//��λ����
	vector<INTEL_HEX_FRAME> m_hex_frame_write;		//�洢����������֡
	vector<INTEL_HEX_FRAME> m_hex_frame_read;	//�洢��ȡ������֡

private:
	int8 m_cBuffer[MAX_BUFFER_SIZE];			//�洢�������ļ�¼
	int8 m_cRecordMark;							//��¼��ʶ
	int8 *m_nRecordLength;						//��¼����
	int8 *m_pLoadOffset;						//װ��ƫ��
	int8 *m_pRecordType;						//��¼����
	int8 *m_pData;								//�����ֶ�
	int16 *m_pDataValid;							//��Ч����
	int8 *m_pChecksum;							//У���
	bool m_bRecvStatus;							//����״̬��ʶ
};
extern Hex* g_hex;
#endif
