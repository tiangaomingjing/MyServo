//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Firmware download 		 									//
//	file				:	FirmwareDl.cpp												//
//	Description			:	use for Firmware download									//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <fstream>
#include <string>
using namespace std;
#include "ST_GTSD_Cmd.h"
#include "FirmwareDl.h"

CFirmwareDL* g_firmwareDl = NULL;
extern CAbsCom* g_AbsCom;

CFirmwareDL::CFirmwareDL()
{
	m_byte_write = 0;
}

CFirmwareDL::~CFirmwareDL()
{

}

int16 CFirmwareDL::ResetVar()
{
	m_byte_write = 0;
	return 0;
}

//设置命令更新启动位
int16 CFirmwareDL::SetRemoteUpdataStartbit(int16 com_type, int16 stationId)
{
	int16 data		= 1; //bit0置1
	int16 com_addr;
	int16 base_addr;	
	int16 comAddr;

	if (com_type == GTSD_COM_TYPE_NET)
	{
		com_addr = REMOTE_FPGA_CTL;
		base_addr = FPGA_DSPA_BASEADDR;
		comAddr = base_addr + (com_addr);
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		com_addr = RN_REMOTE_FPGA_CTL;
		base_addr = FPGA_RN_RMT_START_OFST;
		comAddr = base_addr + (com_addr);
	}

	int16 Data		= data;
	int16 comNum	= 1;
	int16 rtn		= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &Data, comNum, stationId);

	return rtn;
	
}
int16 CFirmwareDL::CheckRemoteUpdataState(int16 com_type, int16 stationId,int32 delaytimes /*= DELAY_TIMES*/)
{
	//检查命令是否完成
	int16 iRet;
	int16 data;	
	int16 com_addr;
	int16 base_addr;
	int16 comAddr;

	if (com_type == GTSD_COM_TYPE_NET)
	{
		com_addr	= REMOTE_FPGA_CTL;
		base_addr	= FPGA_DSPA_BASEADDR;
		comAddr		= base_addr + (com_addr);
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		com_addr	= RN_REMOTE_FPGA_CTL;
		base_addr	= FPGA_RN_RMT_START_OFST;
		comAddr		= base_addr + (com_addr);
	}
	int16 comNum	= 1;
	bool bSuccess	= false;
	
	while (delaytimes--)
	{
		iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &data, comNum, stationId);////查询完成标志
		if (iRet == 0)
		{
			if ((data & 0x0004)) //bit2 finish
			{
				bSuccess = true;
				break;
			}
		}
	}

	if (!bSuccess)
	{
		return -2;
	}

	if (0 == iRet)
	{
		if (data & 0x0002) // bit1 error
		{
			return -3;
		}
	}
	return iRet;
}
int16 CFirmwareDL::SetRemoteUpdataReadRequest(int16 com_type,Uint32 flash_addr, Uint16 iLength, int16 stationId)
{
	//首先写入读命令
	int16 iRet;
	Uint8 data[30]			= { 0 };

	//命令
	data[0]					= CMD_READ;

	int16 addrLo			= LOWORD(flash_addr);
	Uint8 addrLoLo			= LOBYTE(addrLo);
	Uint8 addrLoHi			= HIBYTE(addrLo);
	int16 addrHi			= HIWORD(flash_addr);
	Uint8 addrHiLo			= LOBYTE(addrHi);

	//取24bit地址
	data[1]					= addrLoLo;
	data[2]					= addrLoHi;
	data[3]					= addrHiLo;

	Uint8 lengthLo			= LOBYTE(iLength * 2);
	Uint8 lengthHi			= HIBYTE(iLength * 2);

	//长度
	data[4]					= lengthLo;
	data[5]					= lengthHi;



	int16 com_addr;
	int16 base_addr;
	int16 comAddr;

	if (com_type == GTSD_COM_TYPE_NET)
	{
		com_addr = REMOTE_FPGA_DATA_START;
		base_addr = FPGA_DSPA_BASEADDR;
		comAddr = base_addr + (com_addr);
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		com_addr = RN_REMOTE_FPGA_DATA_START;
		base_addr = FPGA_RN_RMT_START_OFST;
		comAddr = base_addr + (com_addr);
	}
	int16 comNum			= 3;

	iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(data), comNum, stationId); //写入读取命令
	if(iRet!=0)
	{
		return -1;
	}

	//设置命令更新启动位
	iRet = SetRemoteUpdataStartbit(com_type, stationId);
	if(iRet!=0)
	{
		return -2;
	}
	//检查命令是否完成
	iRet = CheckRemoteUpdataState(com_type, stationId);
	if(iRet!=0)
	{
		return -3;
	}
	return iRet;
}
int16 CFirmwareDL::ProtectOff(int16 com_type, int16 stationId)
{
	int16 iRet;
	Uint8 data[20]			= {0};
	int16 com_addr;
	int16 base_addr;
	int16 comAddr;

	if (com_type == GTSD_COM_TYPE_NET)
	{
		com_addr = REMOTE_FPGA_DATA_START;
		base_addr = FPGA_DSPA_BASEADDR;
		comAddr = base_addr + (com_addr);
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		com_addr = RN_REMOTE_FPGA_DATA_START;
		base_addr = FPGA_RN_RMT_START_OFST;
		comAddr = base_addr + (com_addr);
	}
	int16 comNum			= 3;
	data[0]					= CMD_PRET_OFF;					//命令
	data[4]					= 1;							//lenth写1

	iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(data), comNum, stationId);//写入protect off 命令
	if(iRet!=0)
	{
		return -1;
	}

	iRet = SetRemoteUpdataStartbit(com_type, stationId);
	if(iRet!=0)
	{
		return -2;
	}

	iRet = CheckRemoteUpdataState(com_type, stationId,10000);
	if(iRet!=0)
	{
		return -3;
	}
	return iRet;
}
int16 CFirmwareDL::ProtectOn(int16 com_type, int16 stationId)
{
	int16 iRet;
	Uint8 data[20]		= { 0 };
	int16 com_addr;
	int16 base_addr;
	int16 comAddr;

	if (com_type == GTSD_COM_TYPE_NET)
	{
		com_addr = REMOTE_FPGA_DATA_START;
		base_addr = FPGA_DSPA_BASEADDR;
		comAddr = base_addr + (com_addr);
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		com_addr = RN_REMOTE_FPGA_DATA_START;
		base_addr = FPGA_RN_RMT_START_OFST;
		comAddr = base_addr + (com_addr);
	}
	int16 comNum		= 3;
	data[0]				= CMD_PRET_ON;					//命令
	data[4]				= 1;							//lenth写1

	iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(data), comNum, stationId);//写入protect on 命令
	if(iRet!=0)
	{
		return -1;
	}

	iRet = SetRemoteUpdataStartbit(com_type, stationId);
	if(iRet!=0)
	{
		return -2;
	}

	iRet = CheckRemoteUpdataState(com_type, stationId);
	if(iRet!=0)
	{
		return -3;
	}
	return iRet;
}
int16 CFirmwareDL::EraseData(int16 com_type, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress,int16 stationId)
{
	
	int16 iRet;
	Uint8 data[20]		= { 0 };
	int16 com_addr;
	int16 base_addr;
	int16 comAddr;

	if (com_type == GTSD_COM_TYPE_NET)
	{
		com_addr = REMOTE_FPGA_DATA_START;
		base_addr = FPGA_DSPA_BASEADDR;
		comAddr = base_addr + (com_addr);
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		com_addr = RN_REMOTE_FPGA_DATA_START;
		base_addr = FPGA_RN_RMT_START_OFST;
		comAddr = base_addr + (com_addr);
	}
	int16 comNum		= 3;
	data[0]				= CMD_ERASE;					//命令
	data[4]				= 1;							//lenth写1

	iRet				= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(data), comNum, stationId); //写入erase 命令
	if(iRet!=0)
	{
		return -1;
	}

	iRet = SetRemoteUpdataStartbit(com_type, stationId);
	if(iRet!=0)
	{
		return -2;
	}

  int32 num = 1000000;
  //!progress高16位置1，用来给界面提示当前正处于擦除状态
  int16 highSet;
  highSet=(int16)(1<<15);
  for (int32 i = 0; i < num; i++)
  {
    iRet = CheckRemoteUpdataState(com_type, stationId, 100);
//    Sleep(0);
    if (iRet==0)
      break;
    if(i%10==0)
    {
      progress++;
      progress|=highSet;
      if ((progress&(~highSet))>100)
        progress = highSet;
      //std::cout << "erase num=" << i;
      (*tpfUpdataProgressPt)(ptrv, &progress);
    }
  }
  //iRet = CheckRemoteUpdataState(com_type, stationId,100000000);
	if(iRet!=0)
	{
		return -3;
	}
	return iRet;
}
int16 CFirmwareDL::GetFpgaFlashData(int16 com_type,Uint32 flash_addr, int16 *Getbuf, Uint16 iLength, int16 stationId)
{
	int16 iRet;

	//设置读请求
	iRet = SetRemoteUpdataReadRequest(com_type,flash_addr, iLength, stationId);
	if (iRet != 0)
	{
		return -1;
	}

	int16 com_addr;
	int16 base_addr;
	int16 comAddr;

	if (com_type == GTSD_COM_TYPE_NET)
	{
		com_addr = REMOTE_FPGA_DATA_START;
		base_addr = FPGA_DSPA_BASEADDR;
		comAddr = base_addr + (com_addr)+3; //地址不同？
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		com_addr = RN_REMOTE_FPGA_DATA_START;
		base_addr = FPGA_RN_RMT_START_OFST;
		comAddr = base_addr + (com_addr)+6;
	}
	int16 comNum		= 1;

	for (int16 i = 0; i < iLength; ++i)
	{
		//读数据
		iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, (int16*)(&Getbuf[i * 2]), comNum, stationId);
		if (0 != iRet)
		{
			return -1;
		}
		comAddr++;
	}
	return 0;
}
int16 CFirmwareDL::SendFpgaFlashData(int16 com_type,Uint32 flash_addr, int16 *Sendbuf, Uint16 iLength, int16 stationId)
{
	int16 iRet;
	Uint8 data[300]		= {0};

	//命令
	data[0]				= CMD_WRITE;

	int16 addrLo		= LOWORD(flash_addr);
	Uint8 addrLoLo		= LOBYTE(addrLo);
	Uint8 addrLoHi		= HIBYTE(addrLo);
	int16 addrHi		= HIWORD(flash_addr);
	Uint8 addrHiLo		= LOBYTE(addrHi);

	//取24bit地址
	data[1]				= addrLoLo;
	data[2]				= addrLoHi;
	data[3]				= addrHiLo;

	Uint8 lengthLo		= LOBYTE(iLength * 2);
	Uint8 lengthHi		= HIBYTE(iLength * 2);

	//长度
	data[4]				= lengthLo;
	data[5]				= lengthHi;

	//将数据放到buffer中从6开始
	memcpy_s(&data[6], iLength*sizeof(Uint16),Sendbuf, iLength*sizeof(Uint16));

	int16 com_addr;
	int16 base_addr;
	int16 comAddr;

	if (com_type == GTSD_COM_TYPE_NET)
	{
		com_addr = REMOTE_FPGA_DATA_START;
		base_addr = FPGA_DSPA_BASEADDR;
		comAddr = base_addr + (com_addr);
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		com_addr = RN_REMOTE_FPGA_DATA_START;
		base_addr = FPGA_RN_RMT_START_OFST;
		comAddr = base_addr + (com_addr);
	}
	int16 comNum		= 1;

	//for (int16 i = 0; i < iLength + 3;++i)
	//{
	//	//向里面压数据
	//	iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(&data[i*2]), comNum, stationId);
	//	if (0 != iRet)
	//	{
	//		return -1;
	//	}
	//	if (com_type == GTSD_COM_TYPE_NET)
	//	{
	//		//short地址
	//		comAddr++;
	//	}
	//	else if (com_type == GTSD_COM_TYPE_RNNET)
	//	{
	//		//byte地址
	//		comAddr += 2;
	//	}

	//}

	//提高效率，一次写200byte+6byte包头
	//向里面压数据
	iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(&data[0]), (iLength + 3), stationId);
	if (0 != iRet)
	{
		return -1;
	}
	

	iRet = SetRemoteUpdataStartbit(com_type, stationId);
	if(iRet!=0)
	{
		return -2;
	}

  iRet = CheckRemoteUpdataState(com_type, stationId,1000);
	if(iRet!=0)
	{
		return -3;
	}

	return 0;
}
int32 CFirmwareDL::CheckFFNumber(short* buffer,int lenth)
{
	//查找连续的0xff
	int32 ffnum = 0;
	for(int32 i = 0;i<lenth;i++)
	{
		if ((Uint16)(buffer[i]) != 0xffff)
			break;

		ffnum++;
	}
	return ffnum;
}
int16 CFirmwareDL::WriteFPGAFileToFlash(int16 com_type, string pFileName, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 stationId)
{
    void* ptr = ptrv;
	int iRet;
	iRet = ProtectOff(com_type, stationId);//关闭写保护
	if (iRet != 0)
	{
		return 1;
	}

  iRet = EraseData(com_type,tpfUpdataProgressPt,ptrv,progress, stationId); //清除fpga flash
	if (iRet != 0)
	{
		ProtectOn(com_type, stationId);
		return 2;
	}

	//百分比进度
    progress = 10;
	(*tpfUpdataProgressPt)(ptr, &progress);

	//打开文件
	fstream file;
	file.open(pFileName.c_str(), ios::in | ios::out | ios::binary);
	if (!file)
	{
		ProtectOn(com_type, stationId);
		return 4; //文件打开错误
	}

	//首先将数据读出来
	int16 buffer[BUF_LEN]		= {0};
	int16 real_read_lenth		= 0;
	int16 finish_flag			= 0;//是否读取完成标志
	Uint32 flash_addr			= 0;
	Uint32 sum_byte_lenth		= 0;
    Uint32 index                = 0;
    Uint32 times                = 0;//需要读取的次数
	Uint32 times_bk				= 0; 
    while(!finish_flag)
    {
        //读取BUF_LEN个数据，返回真正读取的数据，该条件的前提是后面的ffff足够多
        file.read((int8*)&buffer,sizeof(Uint16)*BUF_LEN);

        sum_byte_lenth += 2 * BUF_LEN;

        //判断其中的数据是否有连续的FF_LEN个0xffff
        int32 number = CheckFFNumber(buffer, BUF_LEN);

        //如果全是0xff那么就说明已经读取到最后的无用区域了
        if(number == BUF_LEN)
        {
            finish_flag = 1;
            m_byte_write = sum_byte_lenth;
        }
        else
        {
            finish_flag = 0;
        }

        index++;
        if (index > 1400)
        {
          //百分比
          if(index%20==0)
          {
            progress = 13;
            (*tpfUpdataProgressPt)(ptr, &progress);
          }
        }
    }
	index		= 0;
    //get the times
    times		= m_byte_write/(2 * BUF_LEN);
	times_bk	= times;
	file.close();

	//百分比
    progress = 15;
    (*tpfUpdataProgressPt)(ptr, &progress);

	//打开文件
	file.open(pFileName.c_str(), ios::in | ios::out | ios::binary);
	if (!file)
	{
		ProtectOn(com_type, stationId);
		return 4; //文件打开错误
	}
  int retSend = 0;
  int retValue = 0;
	while (times!=0)
	{
		//读取BUF_LEN个数据，返回真正读取的数据，该条件的前提是后面的ffff足够多
		file.read((int8*)&buffer,sizeof(Uint16)*BUF_LEN);

		//放在外面为了读取到全是ffff的时候还是写入一次
		for(int i = 0;i<2;i++)
		{
			//将数据写入,一次写100个short
      retSend=SendFpgaFlashData(com_type,flash_addr, &buffer[i*(BUF_LEN >> 1)], BUF_LEN / 2, stationId);//the param len must less than 127.
      if(retSend!=0)
        retSend=SendFpgaFlashData(com_type,flash_addr, &buffer[i*(BUF_LEN >> 1)], BUF_LEN / 2, stationId);//the param len must less than 127.
      if(retSend!=0)
      {
        retValue=-1;
        times=1;
        break;
      }
			//地址增加
			flash_addr += (2*(BUF_LEN>>1));
		}

    index++;
    if(index%10==0)
    {
      progress = 15 + (int16)((((double)index) / times_bk) * 85);
      if (progress >= 100)
      {
        progress = 100;
      }
      (*tpfUpdataProgressPt)(ptr, &progress);
    }
        
		times--;
	}
	file.close();

//	iRet = ProtectOn(com_type, stationId);//open写保护
//	if (iRet != 0)
//	{
//		return 6;
//	}
  return retValue;
}

int16 CFirmwareDL::ReadFPGADataToFile(int16 com_type, string FileName, Uint32 Bytelenth, int16 stationId)
{
	if (Bytelenth == 0)
	{
		return -1;
	}
	Uint32 flash_addr = 0;
	int16* buffer = new int16[Bytelenth >> 1];
	int16* buffer_old = new int16[Bytelenth]; //因为写下去的每个16bit数据占了32bit
	
	//每次读取100个short
	for (Uint32 i = 0; i < (Bytelenth / (BUF_LEN));++i)
	{
		GetFpgaFlashData(com_type, flash_addr, &buffer_old[i*(BUF_LEN >> 1)], (BUF_LEN >> 1), stationId);//the param len must less than 127.
		//地址增加
		flash_addr += (2 * (BUF_LEN >> 1));
	}

	for (Uint32 i = 0; i < (Bytelenth >> 1);++i)
	{
		buffer[i] = buffer_old[i * 2];
	}
	//打开文件
	fstream file;
	file.open(FileName.c_str(), ios::in | ios::out | ios::trunc | ios::binary);
	file.write((int8*)buffer, Bytelenth);
	file.close();

	delete buffer;
	delete buffer_old;
	return 0;
}
