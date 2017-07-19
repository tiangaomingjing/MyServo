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

//���������������λ
int16 CFirmwareDL::SetRemoteUpdataStartbit(int16 com_type)
{
	int16 data		= 1; //bit0��1
	int16 com_addr	= REMOTE_FPGA_CTL;
	int16 base_addr = FPGA_DSPA_BASEADDR;	
	int16 comAddr	= base_addr + (com_addr);
	int16 Data		= data;
	int16 comNum	= 1;
	int16 rtn		= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &Data, comNum);

	return rtn;
	
}
int16 CFirmwareDL::CheckRemoteUpdataState(int16 com_type,int32 delaytimes /*= DELAY_TIMES*/)
{
	//��������Ƿ����
	int16 iRet;
	int16 data;	
	int16 com_addr	= REMOTE_FPGA_CTL;
	int16 base_addr = FPGA_DSPA_BASEADDR;
	int16 comAddr	= base_addr + (com_addr);
	int16 comNum	= 1;
	bool bSuccess	= false;
	
	while (delaytimes--)
	{
		iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &data, comNum);////��ѯ��ɱ�־
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
int16  CFirmwareDL::SetRemoteUpdataReadRequest(int16 com_type,Uint32 flash_addr, Uint16 iLength)
{
	//����д�������
	int16 iRet;
	Uint8 data[30]			= { 0 };

	//����
	data[0]					= CMD_READ;

	int16 addrLo			= LOWORD(flash_addr);
	Uint8 addrLoLo			= LOBYTE(addrLo);
	Uint8 addrLoHi			= HIBYTE(addrLo);
	int16 addrHi			= HIWORD(flash_addr);
	Uint8 addrHiLo			= LOBYTE(addrHi);

	//ȡ24bit��ַ
	data[1]					= addrLoLo;
	data[2]					= addrLoHi;
	data[3]					= addrHiLo;

	Uint8 lengthLo			= LOBYTE(iLength * 2);
	Uint8 lengthHi			= HIBYTE(iLength * 2);

	//����
	data[4]					= lengthLo;
	data[5]					= lengthHi;



	int16 com_addr			= REMOTE_FPGA_DATA_START;
	int16 base_addr			= FPGA_DSPA_BASEADDR;
	int16 comAddr			= base_addr + (com_addr);
	int16 comNum			= 3;

	iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(data), comNum); //д���ȡ����
	if(iRet!=0)
	{
		return -1;
	}

	//���������������λ
	iRet = SetRemoteUpdataStartbit(com_type);
	if(iRet!=0)
	{
		return -2;
	}
	//��������Ƿ����
	iRet = CheckRemoteUpdataState(com_type);
	if(iRet!=0)
	{
		return -3;
	}
	return iRet;
}
int16  CFirmwareDL::ProtectOff(int16 com_type)
{
	int16 iRet;
	Uint8 data[20]			= {0};
	int16 com_addr			= REMOTE_FPGA_DATA_START;
	int16 base_addr			= FPGA_DSPA_BASEADDR;
	int16 comAddr			= base_addr + (com_addr);
	int16 comNum			= 3;
	data[0]					= CMD_PRET_OFF;					//����
	data[4]					= 1;							//lenthд1

	iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(data), comNum);//д��protect off ����
	if(iRet!=0)
	{
		return -1;
	}

	iRet = SetRemoteUpdataStartbit(com_type);
	if(iRet!=0)
	{
		return -2;
	}

	iRet = CheckRemoteUpdataState(com_type,10000);
	if(iRet!=0)
	{
		return -3;
	}
	return iRet;
}
int16  CFirmwareDL::ProtectOn(int16 com_type)
{
	int16 iRet;
	Uint8 data[20]		= { 0 };
	int16 com_addr		= REMOTE_FPGA_DATA_START;
	int16 base_addr		= FPGA_DSPA_BASEADDR;
	int16 comAddr		= base_addr + (com_addr);
	int16 comNum		= 3;
	data[0]				= CMD_PRET_ON;					//����
	data[4]				= 1;							//lenthд1

	iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(data), comNum);//д��protect on ����
	if(iRet!=0)
	{
		return -1;
	}

	iRet = SetRemoteUpdataStartbit(com_type);
	if(iRet!=0)
	{
		return -2;
	}

	iRet = CheckRemoteUpdataState(com_type);
	if(iRet!=0)
	{
		return -3;
	}
	return iRet;
}
int16  CFirmwareDL::EraseData(int16 com_type)
{
	
	int16 iRet;
	Uint8 data[20]		= { 0 };
	int16 com_addr		= REMOTE_FPGA_DATA_START;
	int16 base_addr		= FPGA_DSPA_BASEADDR;
	int16 comAddr		= base_addr + (com_addr);
	int16 comNum		= 3;
	data[0]				= CMD_ERASE;					//����
	data[4]				= 1;							//lenthд1

	iRet				= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(data), comNum); //д��erase ����
	if(iRet!=0)
	{
		return -1;
	}

	iRet = SetRemoteUpdataStartbit(com_type);
	if(iRet!=0)
	{
		return -2;
	}

	iRet = CheckRemoteUpdataState(com_type,100000000);
	if(iRet!=0)
	{
		return -3;
	}
	return iRet;
}
int16  CFirmwareDL::GetFpgaFlashData(int16 com_type,Uint32 flash_addr, int16 *Getbuf, Uint16 iLength)
{
	int16 iRet;

	//���ö�����
	iRet = SetRemoteUpdataReadRequest(com_type,flash_addr, iLength);
	if (iRet != 0)
	{
		return -1;
	}

	int16 com_addr		= REMOTE_FPGA_DATA_START;
	int16 base_addr		= FPGA_DSPA_BASEADDR;
	int16 comAddr		= base_addr + (com_addr)+3;
	int16 comNum		= 1;

	for (int16 i = 0; i < iLength; ++i)
	{
		//������
		iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, (int16*)(&Getbuf[i * 2]), comNum);
		if (0 != iRet)
		{
			return -1;
		}
		comAddr++;
	}
	return 0;
}
int16  CFirmwareDL::SendFpgaFlashData(int16 com_type,Uint32 flash_addr, int16 *Sendbuf, Uint16 iLength)
{
	int16 iRet;
	Uint8 data[300]		= {0};

	//����
	data[0]				= CMD_WRITE;

	int16 addrLo		= LOWORD(flash_addr);
	Uint8 addrLoLo		= LOBYTE(addrLo);
	Uint8 addrLoHi		= HIBYTE(addrLo);
	int16 addrHi		= HIWORD(flash_addr);
	Uint8 addrHiLo		= LOBYTE(addrHi);

	//ȡ24bit��ַ
	data[1]				= addrLoLo;
	data[2]				= addrLoHi;
	data[3]				= addrHiLo;

	Uint8 lengthLo		= LOBYTE(iLength * 2);
	Uint8 lengthHi		= HIBYTE(iLength * 2);

	//����
	data[4]				= lengthLo;
	data[5]				= lengthHi;

	//�����ݷŵ�buffer�д�6��ʼ
	memcpy_s(&data[6], iLength*sizeof(Uint16),Sendbuf, iLength*sizeof(Uint16));

	int16 com_addr		= REMOTE_FPGA_DATA_START;
	int16 base_addr		= FPGA_DSPA_BASEADDR;
	int16 comAddr		= base_addr + (com_addr);
	int16 comNum		= 1;

	for (int16 i = 0; i < iLength + 3;++i)
	{
		//������ѹ����
		iRet = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, (int16*)(&data[i*2]), comNum);
		if (0 != iRet)
		{
			return -1;
		}
		comAddr++;
	}
	

	iRet = SetRemoteUpdataStartbit(com_type);
	if(iRet!=0)
	{
		return -2;
	}

	iRet = CheckRemoteUpdataState(com_type);
	if(iRet!=0)
	{
		return -3;
	}

	return 0;
}
int32 CFirmwareDL::CheckFFNumber(short* buffer,int lenth)
{
	//����������0xff
	int32 ffnum = 0;
	for(int32 i = 0;i<lenth;i++)
	{
		if ((Uint16)(buffer[i]) != 0xffff)
			break;

		ffnum++;
	}
	return ffnum;
}
int16 CFirmwareDL::WriteFPGAFileToFlash(int16 com_type, string pFileName, void(*tpfUpdataProgressPt)(void*,int16*),void* ptrv,int16& progress)
{
    void* ptr = ptrv;
	int iRet;
	iRet = ProtectOff(com_type);//�ر�д����
	if (iRet != 0)
	{
		return 1;
	}

	iRet = EraseData(com_type); //���fpga flash
	if (iRet != 0)
	{
		ProtectOn(com_type);
		return 2;
	}

	//�ٷֱȽ���
    progress = 10;
	(*tpfUpdataProgressPt)(ptr, &progress);

	//���ļ�
	fstream file;
	file.open(pFileName.c_str(), ios::in | ios::out | ios::binary);
	if (!file)
	{
		ProtectOn(com_type);
		return 4; //�ļ��򿪴���
	}

	//���Ƚ����ݶ�����
	int16 buffer[BUF_LEN]		= {0};
	int16 real_read_lenth		= 0;
	int16 finish_flag			= 0;//�Ƿ��ȡ��ɱ�־
	Uint32 flash_addr			= 0;
	Uint32 sum_byte_lenth		= 0;
    Uint32 index                = 0;
    Uint32 times                = 0;//��Ҫ��ȡ�Ĵ���
	Uint32 times_bk				= 0; 
    while(!finish_flag)
    {
        //��ȡBUF_LEN�����ݣ�����������ȡ�����ݣ���������ǰ���Ǻ����ffff�㹻��
        file.read((int8*)&buffer,sizeof(Uint16)*BUF_LEN);

        sum_byte_lenth += 2 * BUF_LEN;

        //�ж����е������Ƿ���������FF_LEN��0xffff
        int32 number = CheckFFNumber(buffer, BUF_LEN);

        //���ȫ��0xff��ô��˵���Ѿ���ȡ����������������
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
			//�ٷֱ�
			progress = 13;
			(*tpfUpdataProgressPt)(ptr, &progress);
		}
    }
	index		= 0;
    //get the times
    times		= m_byte_write/(2 * BUF_LEN);
	times_bk	= times;
	file.close();

	//�ٷֱ�
    progress = 15;
    (*tpfUpdataProgressPt)(ptr, &progress);

	//���ļ�
	file.open(pFileName.c_str(), ios::in | ios::out | ios::binary);
	if (!file)
	{
		ProtectOn(com_type);
		return 4; //�ļ��򿪴���
	}
	while (times!=0)
	{
		//��ȡBUF_LEN�����ݣ�����������ȡ�����ݣ���������ǰ���Ǻ����ffff�㹻��
		file.read((int8*)&buffer,sizeof(Uint16)*BUF_LEN);

		//��������Ϊ�˶�ȡ��ȫ��ffff��ʱ����д��һ��
		for(int i = 0;i<2;i++)
		{
			//������д��,һ��д100��short
			SendFpgaFlashData(com_type,flash_addr, &buffer[i*(BUF_LEN >> 1)], BUF_LEN / 2);//the param len must less than 127.
			//��ַ����
			flash_addr += (2*(BUF_LEN>>1));
		}
        index++;
   
		progress = 15 + (int16)((((double)index) / times_bk) * 85);
		if (progress >= 100)
		{
			progress = 100;
		}
        (*tpfUpdataProgressPt)(ptr, &progress);
        
		times--;
	}
	file.close();

	iRet = ProtectOn(com_type);//openд����
	if (iRet != 0)
	{
		return 6;
	}
	return 0;
}

int16 CFirmwareDL::ReadFPGADataToFile(int16 com_type, string FileName, Uint32 Bytelenth)
{
	if (Bytelenth == 0)
	{
		return -1;
	}
	Uint32 flash_addr = 0;
	int16* buffer = new int16[Bytelenth >> 1];
	int16* buffer_old = new int16[Bytelenth]; //��Ϊд��ȥ��ÿ��16bit����ռ��32bit
	
	//ÿ�ζ�ȡ100��short
	for (Uint32 i = 0; i < (Bytelenth / (BUF_LEN));++i)
	{
		GetFpgaFlashData(com_type, flash_addr, &buffer_old[i*(BUF_LEN >> 1)], (BUF_LEN >> 1));//the param len must less than 127.
		//��ַ����
		flash_addr += (2 * (BUF_LEN >> 1));
	}

	for (Uint32 i = 0; i < (Bytelenth >> 1);++i)
	{
		buffer[i] = buffer_old[i * 2];
	}
	//���ļ�
	fstream file;
	file.open(FileName.c_str(), ios::in | ios::out | ios::trunc | ios::binary);
	file.write((int8*)buffer, Bytelenth);
	file.close();

	delete buffer;
	delete buffer_old;
	return 0;
}
