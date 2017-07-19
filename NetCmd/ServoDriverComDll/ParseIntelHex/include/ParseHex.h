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
Intel Hex文件解析器

Hex文件的格式如下：

RecordMark	RecordLength	LoadOffset	RecordType	Data	Checksum

在Intel Hex文件中，RecordMark规定为“:”

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
	int16	WriteFlash(int16 axis, vector<INTEL_HEX_FRAME>* frame, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type, int16 stationId);
	int16	ReadFlash(int16 axis, vector<INTEL_HEX_FRAME>* frame, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type, int16 stationId);
	int16	WriteHexFile(string filename, vector<INTEL_HEX_FRAME>* frame);
	int16	ParseHex(string filename);					//解析Hex文件
	int16	ParseRecord(int8 ch);					//解析每一条记录
	Uint16	GetRecordLength();					//获取记录长度
	int8	GetRecordMark();						//获取记录标识
	Uint32	GetLoadOffset();						//获取内存装载偏移
	Uint16	GetRecordType();						//获取记录类型
	int16*	GetData();							//获取数据
	Uint16	GetChecksum();						//获取校验和
	int16	ResetVar();							//复位变量
	vector<INTEL_HEX_FRAME> m_hex_frame_write;		//存储解析出来的帧
	vector<INTEL_HEX_FRAME> m_hex_frame_read;	//存储读取出来的帧

private:
	int8 m_cBuffer[MAX_BUFFER_SIZE];			//存储待解析的记录
	int8 m_cRecordMark;							//记录标识
	int8 *m_nRecordLength;						//记录长度
	int8 *m_pLoadOffset;						//装载偏移
	int8 *m_pRecordType;						//记录类型
	int8 *m_pData;								//数据字段
	int16 *m_pDataValid;							//有效数据
	int8 *m_pChecksum;							//校验和
	bool m_bRecvStatus;							//接收状态标识
};
extern Hex* g_hex;
#endif
