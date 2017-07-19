//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Firmware download 		 									//
//	file				:	FirmwareDl.h												//
//	Description			:	use for Firmware download									//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GTSD_FIRMWARE_DL_
#define	_GTSD_FIRMWARE_DL_

#include "Basetype_def.h"

class CFirmwareDL
{
public:
	CFirmwareDL();
	~CFirmwareDL();

public:	
	static const int32		BUF_LEN			= 200;
	static const int32		FF_LEN			= 200;
	static const int16		DELAY_TIMES		= 100;

	static const int32		CMD_READ		= 0;
	static const int32		CMD_WRITE		= 1;
	static const int32		CMD_PRET_ON		= 2;
	static const int32		CMD_PRET_OFF	= 3;
	static const int32		CMD_ERASE		= 4;

	static const Uint16		REMOTE_FPGA_CTL			= 0x37F;
	static const Uint16		REMOTE_FPGA_DATA_START	= 0x300;
	static const Uint16		REMOTE_FPGA_DATA_END	= 0x37C;

	Uint32 m_byte_write;

	int16 ResetVar();
	int16 SetRemoteUpdataStartbit(int16 com_type);
	int16 CheckRemoteUpdataState(int16 com_type, int32 delaytimes = DELAY_TIMES);
	int16 SetRemoteUpdataReadRequest(int16 com_type, Uint32 flash_addr, Uint16 iLength);
	int16 ProtectOff(int16 com_type );
	int16 ProtectOn(int16 com_type);
	int16 EraseData(int16 com_type );
	int16 GetFpgaFlashData(int16 com_type, Uint32 flash_addr, int16 *Getbuf, Uint16 iLength);
	int16 SendFpgaFlashData(int16 com_type, Uint32 flash_addr, int16 *Sendbuf, Uint16 iLength);
	int32 CheckFFNumber(short* buffer, int lenth);

    int16 WriteFPGAFileToFlash(int16 com_type, string pFileName, void(*tpfUpdataProgressPt)(void*,int16*),void* ptrv,int16& progress);
	int16 ReadFPGADataToFile(int16 com_type, string  FileName, Uint32 Bytelenth);
};
extern CFirmwareDL* g_firmwareDl;
#endif
