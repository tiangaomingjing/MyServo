#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;
#include "ParseHex.h"

Hex* g_hex = NULL;

Hex::Hex(int8 mark)
{
	m_baseAddr				= 0;
	m_cRecordMark			= mark;
	m_cBuffer[0]			= '\0';
	m_nRecordLength			= NULL;
	m_pLoadOffset			= NULL;
	m_pRecordType			= NULL;
	m_pData					= NULL;
	m_pDataValid			= NULL;
	m_pChecksum				= NULL;
	m_bRecvStatus			= false;
	dataLenth				= 0;
}

Hex::~Hex()
{
	
}

//获取记录标识
int8 Hex::GetRecordMark()
{
	return this->m_cRecordMark;
}
//获取每条记录的长度
Uint16 Hex::GetRecordLength()
{
	if (strlen(m_cBuffer) >= 2)
	{
		m_nRecordLength		= new int8[3];
		m_nRecordLength[0]	= m_cBuffer[0];
		m_nRecordLength[1]	= m_cBuffer[1];
		m_nRecordLength[2]	= '\0';
		int8 *p = NULL;
		return (Uint16)strtol(m_nRecordLength, &p, 16);  //将字符串转化为整形
	}
	else
	{
		return 0;
	}
}
//获取装载偏移
Uint32 Hex::GetLoadOffset()
{
	if (strlen(m_cBuffer) == (GetRecordLength() + 5) * 2) //去掉mark以后，两个字符占一个字节
	{
		m_pLoadOffset = new int8[5];
		for (int i = 0; i < 4; ++i)
		{
			m_pLoadOffset[i] = m_cBuffer[i + 2];
		}
		m_pLoadOffset[4] = '\0';

		int8 *p = NULL;
		return strtol(m_pLoadOffset, &p, 16);  //将字符串转化为整形
	}
	else
	{
		return  0;
	}
}
//获取记录类型
Uint16 Hex::GetRecordType()
{
	if (strlen(m_cBuffer) == (GetRecordLength() + 5) * 2)
	{
		m_pRecordType		= new int8[3];
		m_pRecordType[0]	= m_cBuffer[6];
		m_pRecordType[1]	= m_cBuffer[7];
		m_pRecordType[2]	= '\0';
		int8 *p = NULL;
		return (Uint16)strtol(m_pRecordType, &p, 16);  //将字符串转化为整形
	}
	else
	{
		return 0;
	}
	
}
//获取数据
int16* Hex::GetData()
{
	if (strlen(m_cBuffer) == (GetRecordLength() + 5) * 2)
	{
		int32 len			= GetRecordLength();
		m_pData				= new int8[len * 2 + 1];
		m_pDataValid		= new int16[len];
		int i;
		for (i = 0; i < len * 2; ++i)
		{
			m_pData[i]		= m_cBuffer[i + 8];
			if ((i!=0)&&(i%2 == 0))
			{
				m_pData[i]					= '\0';
				int8 *p = NULL;
				m_pDataValid[((i>>1)-1)]	= (int16)strtol(&m_pData[i - 2], &p, 16);  //将字符串转化为整形
				//恢复之前的数据
				m_pData[i]					= m_cBuffer[i + 8];
			}	
		}
		if (i == len * 2)
		{
			m_pData[len * 2]				= '\0';
			int8 *p = NULL;
			m_pDataValid[((i >> 1) - 1)]	= (int16)strtol(&m_pData[i - 2], &p, 16);  //将字符串转化为整形
		}	
	}
	return m_pDataValid;
}
//获取校验和
Uint16 Hex::GetChecksum()
{
	int32 len = GetRecordLength();
	if (strlen(m_cBuffer) == (len + 5) * 2)
	{
		m_pChecksum			= new int8[3];
		m_pChecksum[0]		= m_cBuffer[(len + 5) * 2 - 2];
		m_pChecksum[1]		= m_cBuffer[(len + 5) * 2 - 1];
		m_pChecksum[2]		= '\0';
		int8 *p = NULL;
		return (Uint16)strtol(m_pChecksum, &p, 16);  //将字符串转化为整形
	}
	else
	{
		return 0;
	}
	
}

int16 Hex::ResetVar()
{
	m_hex_frame_read.clear();
	m_hex_frame_write.clear();
	dataLenth = 0;
	return 0;
}

//解析Hex文件中的每一条记录
int16 Hex::ParseRecord(int8 ch)
{
	INTEL_HEX_FRAME tmp;
	int32 buf_len				= strlen(m_cBuffer);
	if ((m_bRecvStatus == false) &&(ch!=0x0A) && (ch != 0x0D))
	{
		if (GetRecordMark() != ch)
		{
			return 1;
		}
		else
		{
			m_bRecvStatus = true;
			m_cBuffer[0] = '\0';
			return 0;
		}
	}
	
	if ((buf_len == (GetRecordLength() + 5) * 2 - 1))
	{
		//接收最后一个字符
		m_cBuffer[buf_len]		= ch;
		m_cBuffer[buf_len + 1]	= '\0';
		//检验接收的数据
		int8 temp[3];
		int8 *p					= NULL;
		int32 checksum		= 0;
		for (Uint16 i = 0; i < strlen(m_cBuffer); i += 2)
		{
			temp[0]				= m_cBuffer[i];
			temp[1]				= m_cBuffer[i + 1];
			temp[2]				= '\0';
			checksum			+= strtol(temp, &p, 16);
			temp[0]				= '\0';
		}
		checksum				&= 0x00ff;//取计算结果的低8位
		if (checksum == 0)//checksum为0说明接收的数据无误
		{
			
			tmp.lenth			= GetRecordLength();
			tmp.addr			= GetLoadOffset() + (m_baseAddr);
			tmp.type			= GetRecordType();
			memcpy_s(&(tmp.data[0]), tmp.lenth*sizeof(int16), GetData(), tmp.lenth*sizeof(int16));
			tmp.checksum		= GetChecksum();

			if (tmp.type == 4) //线性扩展地址
			{
				m_baseAddr = (((tmp.data[0] << 8) | (tmp.data[1]))<<16);
			}
			else if (tmp.type == 5)//起始线性地址
			{
				m_baseAddr = (((tmp.data[0] << 24) | (tmp.data[1]<<16)|(tmp.data[2]<<8)|(tmp.data[3])));
			}
			else if (tmp.type == 1)//停止
			{

			}
			else if (tmp.type == 0)//数据
			{
				m_hex_frame_write.push_back(tmp);
				dataLenth += tmp.lenth;
			}
			else
			{
			}	
		}
		else//否则接收数据有误
		{
			return 1;
		}
		m_cBuffer[0] = '\0';
		m_bRecvStatus = false;

		delete m_nRecordLength;						//记录长度
		delete m_pLoadOffset;						//装载偏移
		delete m_pRecordType;						//记录类型
		delete m_pData;								//数据字段
		if (tmp.type != 1){ delete m_pDataValid; }//有效数据
		delete m_pChecksum;							//校验和

		m_nRecordLength		= NULL;
		m_pLoadOffset		= NULL;
		m_pRecordType		= NULL;
		m_pData				= NULL;
		m_pDataValid		= NULL;
		m_pChecksum			= NULL;
		m_bRecvStatus		= false;
	}
	else if (m_bRecvStatus)
	{
		m_cBuffer[buf_len]			= ch;
		m_cBuffer[buf_len + 1]		= '\0';
	}
	return 0;
}

int16 Hex::CompareFlash(vector<INTEL_HEX_FRAME>* frame_w, vector<INTEL_HEX_FRAME>* frame_r)
{
	vector<INTEL_HEX_FRAME>::iterator it_w = frame_w->begin();
	vector<INTEL_HEX_FRAME>::iterator it_r = frame_r->begin();
	bool flag = false;
	while ((frame_w->end() != it_w) && (frame_r->end() != it_r))
	{
		if ((*it_w).lenth != (*it_r).lenth)
		{
			flag = true;
			break;
		}

		if ((*it_w).addr != (*it_r).addr)
		{
			flag = true;
			break;
		}

		for (int16 i = 0; i < (*it_w).lenth;++i)
		{
			if ((*it_w).data[i]!=(*it_r).data[i])
			{
				flag = true;
				break;
			}
			
		}
		it_w++;
		it_r++;

	}

	if (flag == false)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

int16 Hex::WriteFlash(int16 axis, vector<INTEL_HEX_FRAME>* frame, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type)
{
	int16 progress;
	int16 index = 0;
	int32 vector_size = frame->size();
	for (vector<INTEL_HEX_FRAME>::iterator it = frame->begin(); frame->end() != it; it++)
	{
		if (GTSD_CMD_FlashWrite(axis, &(*it), com_type) != 0)
		{
			return -1;
		}
		progress = 30 + int16(((double)index)/vector_size * 20.0);
		(*tpfUpdataProgressPt)(ptrv, &progress);
		index++;
	}
	return 0;
}

int16 Hex::ReadFlash(int16 axis, vector<INTEL_HEX_FRAME>* frame, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16 com_type)
{
	int16 progress;
	int16 index = 0;
	int32 vector_size = frame->size();

	for (vector<INTEL_HEX_FRAME>::iterator it = frame->begin(); frame->end() != it; it++)
	{
		INTEL_HEX_FRAME tmp;
		if (GTSD_CMD_FlashRead(axis, &(*it), &tmp, com_type) != 0)
		{
			return -1;
		}
		m_hex_frame_read.push_back(tmp);	
		progress = 50 + int16(((double)index) / vector_size * 40.0);
		(*tpfUpdataProgressPt)(ptrv, &progress);
		index++;
	}
	return 0;
}

int16 Hex::WriteHexFile(string filename, vector<INTEL_HEX_FRAME>* frame)
{
	int16 index;
	index = filename.find_last_of('.', filename.length());
	string outFile = filename.substr(0, index);
	outFile = outFile + "_parse.txt";

	fstream file_out;
	file_out.open(outFile.c_str(), ios::in | ios::out | ios::trunc);

	for (vector<INTEL_HEX_FRAME>::iterator it = frame->begin(); frame->end() != it; it++)
	{
		file_out << setiosflags(ios::left) << setw(10) << "addr:" << setw(8) << hex << (*it).addr << endl;
		file_out << setiosflags(ios::left) << setw(10) << "lenth:" << setiosflags(ios::right) << setw(2) << setfill('0') << hex << (*it).lenth << endl;
		file_out << setfill(' ');
		file_out << resetiosflags(ios::right);//重置标记
		file_out << setiosflags(ios::left) << setw(10) << "data:";
		for (int16 i = 0; i < (*it).lenth; ++i)
		{
			file_out << resetiosflags(ios::left);//重置标记
			file_out << setiosflags(ios::right) << setw(2) << setfill('0') << hex << (*it).data[i];
		}
		file_out << endl;

		file_out << resetiosflags(ios::right);//重置标记
		file_out << setfill(' ') << endl;
	}
	file_out.close();
	return 0;
}

//解析Hex文件
int16 Hex::ParseHex(string filename)
{
	fstream file;
	file.open(filename.c_str());
	if (!file)
	{
		return -1;
	}
	char tmp;
	int16 flag = 0;
	while(!file.eof())
	{
		file.get(tmp);
		if (ParseRecord(tmp)!=0)
		{
			flag = 1;
			break;
		}
	}
	file.close();
	if (flag != 0)
	{
		return 1;
	}

	WriteHexFile(filename, &m_hex_frame_write);
	
	return 0;
}

