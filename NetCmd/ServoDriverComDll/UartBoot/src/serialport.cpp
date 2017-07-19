//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	serialport		 											//
//	file				:	serialport.cpp												//
//	Description			:	use for control the serial port								//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/11/19	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <memory>
using std::unique_ptr;
#include <assert.h>

#include "serialport.h"
#include "ihex.h"
#include "ST_GTSD_Cmd.h"
#include "BaseReturn_def.h"
#include "ServoDriverComDll.h"

extern CAbsCom* g_AbsCom;

image::~image()
{
	chunk *c = first_chunk;
	while (c) {
		chunk *next_c = c->next;
		delete c;
		c = next_c;
	}
}


//////////////////////////////////////////////////////////////////////////
SerialCtl* g_serial = NULL;

//////////////////////////////////////////////////////////////////////////

SerialCtl::SerialCtl() : status(FALSE) {}

SerialCtl::~SerialCtl()
{
	
}

bool SerialCtl::open(int16 axis, int32 baudRate, int16 com_type, int16 stationId)
{
	if (status == true) // if port is opened already do not open port again
		return false;

  //---------------------------------------------------------------------------
	//实现打开串口的功能，通过操作具体的FPGA地址
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16 Axis			= axis;												//轴号
	int16 dsp_number	= GTSD_DSP_A;
	//根据轴号计算是哪个dsp
	if (Axis > 1)
	{
		dsp_number = (int16)(GTSD_DSP_B);
	}
	//----------------------------------------------------------------------------
	int16 com_addr		= FPGA_UART_CONFIG_W;								//写配置地址
	int16 com_addr1		= FPGA_UART_BAUDRATE_WR;							//读写波特率地址
	int16 com_addr2		= FPGA_UART_STARTEND_W;								//写打开模块地址
	int16 com_addr3		= FPGA_UART_CONFIG_R;								//读取是否打开状态地址

	//根据dsp号选择FPGA基地址
	int16 base_addr;
	if (dsp_number == GTSD_DSP_A)
	{
		if (com_type == GTSD_COM_TYPE_NET)
		{
			base_addr = (int16)FPGA_DSPA_UART_PCDEBUG_BASEADDR;
		}
		else if (com_type == GTSD_COM_TYPE_RNNET)
		{
			base_addr = (int16)FPGA_DSPA_UART_RNNET_BASEADDR;
		}
		else
		{
			base_addr = (int16)FPGA_DSPA_UART_RNNET_BASEADDR;
		}
		
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		if (com_type == GTSD_COM_TYPE_NET)
		{
			base_addr = (int16)FPGA_DSPB_UART_PCDEBUG_BASEADDR;
		}
		else if (com_type == GTSD_COM_TYPE_RNNET)
		{
			base_addr = (int16)FPGA_DSPB_UART_RNNET_BASEADDR;
		}
		else
		{
			base_addr = (int16)FPGA_DSPB_UART_RNNET_BASEADDR;
		}
	}
	else
	{
		return Net_Rt_param_Err;
	}
	//计算实际的地址
	int16 comAddr;
	int16 comAddr1;
	int16 comAddr2;
	int16 comAddr3;
	int16 comNum;
	if (com_type == GTSD_COM_TYPE_NET)
	{
		//short地址
		//计算实际的地址
		comAddr = base_addr + (com_addr >> 1);
		comAddr1 = base_addr + (com_addr1 >> 1);
		comAddr2 = base_addr + (com_addr2 >> 1);
		comAddr3 = base_addr + (com_addr3 >> 1);
		comNum = 1;
	}
	else if (com_type == GTSD_COM_TYPE_RNNET)
	{
		//byte地址
		//计算实际的地址
		comAddr		= base_addr + (com_addr);
		comAddr1	= base_addr + (com_addr1);
		comAddr2	= base_addr + (com_addr2);
		comAddr3	= base_addr + (com_addr3);
		comNum =  1;
	}
	
	//----------------------------------------------------------------------------
	//打开模块
	int16 startValue	= (int16)0xFF00; //写该值打开该模块
	int16 rtn			= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr2, &startValue, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	//读取该值验证是否已经打开该模块
	int16 startVauleRtn;
	rtn					= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr3, &startVauleRtn, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	//如果bit15是1，说明已经打开，否则未打开成功
	if ((startVauleRtn &0x8000)==0)
	{
		return Net_Rt_param_Err;
	}
	//----------------------------------------------------------------------------
	//计算下发的波特率 (baudRate unit bps),转化为ns
	double BD		= (1.0 / baudRate)*1000.0*1000.0*1000.0;
	double clk		= (1.0 / FPGA_CLOCK)*1000.0*1000.0*1000.0;
	
	//download 的波特率是BD/clk;
	int16  dl_bd		= (int16)(BD / clk)-1;
	int16  dl_bds		= dl_bd;
	int16  dl_cfg		= (0x0700);

	//配置停止位，奇偶校验，传输位宽等
	rtn					= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &dl_cfg, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	//读取配置进行验证
	int16 dl_cfg_rtn;
	rtn					= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &dl_cfg_rtn, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	if ((dl_cfg_rtn & 0x3fff) != (0x0700))
	{
		return Net_Rt_param_Err;
	}
	//----------------------------------------------------------------------------
	//配置波特率
	rtn					= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr1, &dl_bd, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	int16 dl_bd_rtn;
	rtn					= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr1, &dl_bd_rtn, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	if (dl_bd_rtn != dl_bds)
	{
		return Net_Rt_param_Err;
	}

	status		= true;
	baudrate	= baudRate;
	return true;
}

bool SerialCtl::close(int16 axis, int16 com_type, int16 stationId)
{
	if (status == true)
	{
		//实现关闭串口的功能，通过操作具体的FPGA地址
		if (axis >= COM_AXIS_MAX)
		{
			return Net_Rt_param_Err;
		}
		int16 Axis			= axis;												//轴号
		int16 dsp_number	= GTSD_DSP_A;
		//根据轴号计算是哪个dsp
		if (Axis > 1)
		{
			dsp_number = (int16)(GTSD_DSP_B);
		}
		int16 com_addr		= FPGA_UART_STARTEND_W;								//关闭模块地址
		int16 com_addr1		= FPGA_UART_CONFIG_R;								//查看是否未使能
		//根据dsp号选择FPGA基地址
		int16 base_addr;
		if (dsp_number == GTSD_DSP_A)
		{
			if (com_type == GTSD_COM_TYPE_NET)
			{
				base_addr = (int16)FPGA_DSPA_UART_PCDEBUG_BASEADDR;
			}
			else if (com_type == GTSD_COM_TYPE_RNNET)
			{
				base_addr = (int16)FPGA_DSPA_UART_RNNET_BASEADDR;
			}
			else
			{
				base_addr = (int16)FPGA_DSPA_UART_RNNET_BASEADDR;
			}

		}
		else if (dsp_number == GTSD_DSP_B)
		{
			if (com_type == GTSD_COM_TYPE_NET)
			{
				base_addr = (int16)FPGA_DSPB_UART_PCDEBUG_BASEADDR;
			}
			else if (com_type == GTSD_COM_TYPE_RNNET)
			{
				base_addr = (int16)FPGA_DSPB_UART_RNNET_BASEADDR;
			}
			else
			{
				base_addr = (int16)FPGA_DSPB_UART_RNNET_BASEADDR;
			}
		}
		else
		{
			return Net_Rt_param_Err;
		}
		//计算实际的地址
		int16 comAddr;
		int16 comAddr1;
		int16 comNum;
		if (com_type == GTSD_COM_TYPE_NET)
		{
			//short地址
			//计算实际的地址
			comAddr = base_addr + (com_addr >> 1);
			comAddr1 = base_addr + (com_addr1 >> 1);
			comNum = 1;
		}
		else if (com_type == GTSD_COM_TYPE_RNNET)
		{
			//byte地址
			//计算实际的地址
			comAddr = base_addr + (com_addr);
			comAddr1 = base_addr + (com_addr1);
			comNum = 1;
		}

		//----------------------------------------------------------------------------
		//关闭模块
		int16 stopValue = 0x0000; //写该值关闭该模块
		int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &stopValue, comNum, stationId);
		if (rtn != GTSD_COM_SUCCESS)
		{
			return false;
		}
		//读取该值验证是否已经关闭该模块
		int16 stopValueRtn;
		rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr1, &stopValueRtn, comNum, stationId);
		if (rtn != GTSD_COM_SUCCESS)
		{
			return false;
		}

		if ((stopValueRtn & 0x8000) != 0)
		{
			return false;
		}
		status = false;                 // update status
		return true;
	}
	return true;
}

bool SerialCtl::read(int16 axis,Uint8 *buf, int32 length, int32 *length_read, int16 com_type, int16 stationId)
{
	//实现读串口的功能，通过操作具体的FPGA地址
	if (status == true)
	{
		if (axis >= COM_AXIS_MAX)
		{
			return Net_Rt_param_Err;
		}
		int16 Axis			= axis;												//轴号
		int16 dsp_number	= GTSD_DSP_A;
		int16 com_addr		= FPGA_UART_RX_STATE_R;								// RX FIFO STATE
		int16 com_addr1		= FPGA_UART_RECEIVE_FIFO_R;							// RX FIFO 

		//根据轴号计算是哪个dsp
		if (Axis > 1)
		{
			dsp_number = (int16)(GTSD_DSP_B);
		}
		//根据dsp号选择FPGA基地址
		int16 base_addr;
		if (dsp_number == GTSD_DSP_A)
		{
			if (com_type == GTSD_COM_TYPE_NET)
			{
				base_addr = (int16)FPGA_DSPA_UART_PCDEBUG_BASEADDR;
			}
			else if (com_type == GTSD_COM_TYPE_RNNET)
			{
				base_addr = (int16)FPGA_DSPA_UART_RNNET_BASEADDR;
			}
			else
			{
				base_addr = (int16)FPGA_DSPA_UART_RNNET_BASEADDR;
			}

		}
		else if (dsp_number == GTSD_DSP_B)
		{
			if (com_type == GTSD_COM_TYPE_NET)
			{
				base_addr = (int16)FPGA_DSPB_UART_PCDEBUG_BASEADDR;
			}
			else if (com_type == GTSD_COM_TYPE_RNNET)
			{
				base_addr = (int16)FPGA_DSPB_UART_RNNET_BASEADDR;
			}
			else
			{
				base_addr = (int16)FPGA_DSPB_UART_RNNET_BASEADDR;
			}
		}
		else
		{
			return Net_Rt_param_Err;
		}
		//计算实际的地址
		int16 comAddr;
		int16 comAddr1;
		int16 comNum;
		if (com_type == GTSD_COM_TYPE_NET)
		{
			//short地址
			//计算实际的地址
			comAddr = base_addr + (com_addr >> 1);
			comAddr1 = base_addr + (com_addr1 >> 1);
			comNum = 1;
		}
		else if (com_type == GTSD_COM_TYPE_RNNET)
		{
			//byte地址
			//计算实际的地址
			comAddr = base_addr + (com_addr);
			comAddr1 = base_addr + (com_addr1);
			comNum = 1;
		}
		//----------------------------------------------------------------------------
		//查询RX状态
		int16 stateValue = 0x0000;
		int16 rtn;
		//----------------------------------------------------------------------------
		do 
		{	
			//查询当前 rx fifo中有多少数据。
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &stateValue, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
			//查询fifo是否发生过接收错误
			if (stateValue & 0x4000)
			{
				return false;
			}
		} while ((stateValue&0x03ff)<length);
	
		//如果接收到足够的数据，那么就开始从fifo中读取数据
		int16 tmp;
		int32 i;
		for (i = 0; i < length; ++i)
		{
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr1, &tmp, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
			//16bit数据转化为8bit数据，16bit数据的高8bit无效。
			buf[i] = (Uint8)(tmp & 0x00ff);
		}
		if (i == length)
		{
			*length_read = length;
		}

		return true;
	}
	else
	{
		return false;
	}
	
}

bool SerialCtl::write(int16 axis,Uint8 *buf, int32 length, int32 *length_written, int16 com_type, int16 stationId)
{
	//实现写串口的功能，通过操作具体的FPGA地址
	if (status == true)
	{
		if (axis >= COM_AXIS_MAX)
		{
			return Net_Rt_param_Err;
		}
		int16 Axis			= axis;												//轴号
		int16 dsp_number	= GTSD_DSP_A;
		int16 com_addr		= FPGA_UART_RX_STATE_R;								//RTS信号读取，为0表示可以发送数据到uart，为1停止发送，该功能由FPGA处理，这里只是查询该标志
		int16 com_addr1		= FPGA_UART_TX_STATE_R;								// TX FIFO 状态
		int16 com_addr2		= FPGA_UART_SEND_FIFO_W;							// tx fifo
		//根据轴号计算是哪个dsp
		if (Axis > 1)
		{
			dsp_number = (int16)(GTSD_DSP_B);
		}
		//根据dsp号选择FPGA基地址
		int16 base_addr;
		if (dsp_number == GTSD_DSP_A)
		{
			if (com_type == GTSD_COM_TYPE_NET)
			{
				base_addr = (int16)FPGA_DSPA_UART_PCDEBUG_BASEADDR;
			}
			else if (com_type == GTSD_COM_TYPE_RNNET)
			{
				base_addr = (int16)FPGA_DSPA_UART_RNNET_BASEADDR;
			}
			else
			{
				base_addr = (int16)FPGA_DSPA_UART_RNNET_BASEADDR;
			}

		}
		else if (dsp_number == GTSD_DSP_B)
		{
			if (com_type == GTSD_COM_TYPE_NET)
			{
				base_addr = (int16)FPGA_DSPB_UART_PCDEBUG_BASEADDR;
			}
			else if (com_type == GTSD_COM_TYPE_RNNET)
			{
				base_addr = (int16)FPGA_DSPB_UART_RNNET_BASEADDR;
			}
			else
			{
				base_addr = (int16)FPGA_DSPB_UART_RNNET_BASEADDR;
			}
		}
		else
		{
			return Net_Rt_param_Err;
		}
		//计算实际的地址
		int16 comAddr;
		int16 comAddr1;
		int16 comAddr2;
		int16 comNum;
		if (com_type == GTSD_COM_TYPE_NET)
		{
			//short地址
			//计算实际的地址
			comAddr = base_addr + (com_addr >> 1);
			comAddr1 = base_addr + (com_addr1 >> 1);
			comAddr2 = base_addr + (com_addr2 >> 1);
			comNum = 1;
		}
		else if (com_type == GTSD_COM_TYPE_RNNET)
		{
			//byte地址
			//计算实际的地址
			comAddr = base_addr + (com_addr);
			comAddr1 = base_addr + (com_addr1);
			comAddr2 = base_addr + (com_addr2);
			comNum = 1;
		}
		//----------------------------------------------------------------------------
		//查询RTS状态
		int16 stateValue = 0x0000; 
		int16 counter = 0;
		int16 rtn;
		do 
		{
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &stateValue, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
			counter++;
		} while ((stateValue & 0x2000) && (counter<2000));//假如bit13是1表示芯片未准备好接收，那么先不要发送数据

		if (counter >=2000)
		{
			return false;
		}
		//----------------------------------------------------------------------------
		//查询当前发送fifo中有多少空间。
		do
		{
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr1, &stateValue, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
			//查询fifo是否发生过溢出
			if (stateValue & 0x1000)
			{
				return false;
			}
		} while ((1024 - (int16)(stateValue & 0x07ff))<length);

		//如果空间足够，那么就开始发送数据。写tx fifo
		int16 tmp;
		int32 i;
		for (i = 0; i < length;++i)
		{
			//8bit数据转化为16bit数据，16bit数据的高8bit无效。
			tmp = (int16)(buf[i] & 0x00ff);
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr2, &tmp, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
			//Sleep(10);
		}
		if (i == length)
		{
			*length_written = length;
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool SerialCtl::downloadBootStream(int16 axis, string& inputKey, int16 cmd, string& filename, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 com_type, int16 stationId)
{
	if (cmd == SerialCtl::AC_PROGRAM)
	{
		if (!send_ldr_img(axis, inputKey, filename, tpfUpdataProgressPt, ptrv, progress, com_type,stationId))
		{
			return false;
		}
		Sleep(1000); // Give the second stage time to get started.
		return true;
	}
	if (cmd == SerialCtl::AC_ERASE)
	{
		return send_erase_cmd(axis, tpfUpdataProgressPt, ptrv, progress, com_type,stationId);
	}

	return false;
}

// Send loader image file with given name, which can be either binary or Intel
// Hex, over the serial line configured in the dialog.
bool SerialCtl::send_ldr_img(int16 axis, string& inputKey, string& filename, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 com_type, int16 stationId)
{
	Uint8 key[16];
	//判断是否key存在
	if ((!inputKey.empty()) && (!parse_key(inputKey, key)))
		return false;

	unique_ptr<image> img(read_img(filename, 0, 0x80000000, 4));
	/*
	访问智能指针包含的裸指针则可以用 get() 函数。
	由于智能指针是一个对象，所以if (my_smart_object)永远为真，
	要判断智能指针的裸指针是否为空，需要这样判断：if (my_smart_object.get())。
	*/
	if (!img.get())
		return false;

	if (img->first_chunk != img->last_chunk) 
	{
		//Fragmented boot stream;
		return false;
	}

	//百分比进度
	progress = 20;
	(*tpfUpdataProgressPt)(ptrv, &progress);

	// Parse and verify boot stream.
	vector<Uint32> blocks;
	Uint32 addr = 0, last_addr = 0;
	Uint8 *data = &img->first_chunk->data[0];
	while (addr < img->len) 
	{
		// Fill blocks have a non-zero BYTE_COUNT size, but no actual payload.
		// They are marked with BFLAG_FILL, which is bit 8 of the block code.
		Uint32 payload;
		if (data[addr + 1] & 1)
		{
			payload = 0;
		}
		else 
		{
			payload = data[addr + 8] + (data[addr + 9] << 8) +
				(data[addr + 10] << 16) + (data[addr + 11] << 24);
		}

		// The payload size should be a multiple of 4.
		// (The Slater loader utility has the -a4 option to ensure that.)
		if (payload % 4 != 0)
		{
			//Unaligned payload size in boot stream;
			return false;
		}

		Uint32 size = 16 + payload;
		blocks.push_back(size);
		last_addr = addr;
		addr += size;
	}

	if (addr != img->len) 
	{
		//Invalid payload size in boot stream;
		return false;
	}

	// Check BFLAG_FINAL, which is bit 15 of the little-endian block code word.
	if (!(data[last_addr + 1] & 0x80))
	{
		//Last block of boot stream not marked as final;
		return false;
	}

	//Verified boot stream.;
	//百分比进度
	progress = 30;
	(*tpfUpdataProgressPt)(ptrv, &progress);

	if (!do_ldr_autobaud(axis,com_type, stationId))
		return false;

	Uint32 written;

	//// Send unlock command
	//if (!inputKey.empty()) 
	//{
	//	Uint8 cmd[20] = { 0x3F, 0x04, 0x6A, 0x08 };
	//	memcpy_s(cmd + 4,16, key, 16);
	//	if ((GTSD_CMD_WriteSerialPort(axis, cmd, 20, (int32*)&written, com_type) != 0) || written != 20)
	//	{
	//		//Write of unlock command failed;
	//		return false;
	//	}
	//	// Unlock command sent;
	//}
	//百分比进度
	progress = 35;
	(*tpfUpdataProgressPt)(ptrv, &progress);

	// Download blocks
	addr = 0;
	Uint32 chunk_size = baudrate / 50;  // About 0.2 seconds per chunk.

	if (chunk_size >= 1000)
	{
		chunk_size = 512;
	}
	Uint64 sumLenth = 0;
	for (size_t i = 0; i < blocks.size(); i++)
	{
		sumLenth += blocks[i];
	}

	for (size_t i = 0; i < blocks.size(); i++)
	{
		Uint32 size = blocks[i];

		Uint32 remaining = size;
		while (remaining)
		{
			Uint32 to_write = min(remaining, chunk_size);
			if ((GTSD_CMD_WriteSerialPort(axis, data + addr, to_write, (int32*)&written, com_type) != 0) || !written)
			{
				// Download failed;
				return false;
			}
			remaining -= written;
			addr += written;
			//Sleep(100);
			progress = (int16)(35 + (int32)(((((double)addr) / sumLenth)* 65.0)));
			if (progress >= 100)
			{
				progress = 100;
			}
			(*tpfUpdataProgressPt)(ptrv, &progress);
		}
	}

	// Download completed;
	return true;
}

/* Parse and store key in dialog into supplied array.
* Stored in big-endian word order, with little-endian byte order within each word.
* If the first byte (i.e. hex pair) in the hex string is considered position 0:
* 3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12.
*/
int16 SerialCtl::parse_key(string& inputKey, Uint8 key[16])
{
	if (inputKey.length() != 32) 
	{
		//长度错误
		return -1;
	}
	for (int16 i = 0; i < 32; i++) 
	{
		if (!isxdigit(inputKey[i]))
		{
			//格式错误，不是十六进制数据
			return false;
		}
	}
	// 为了按照如下规格排列3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12.
	// 使用异或
	for (int16 i = 0; i < 16; i++) 
	{
		int8 buf[] = { inputKey[2 * i], inputKey[2 * i + 1], 0 };
		key[i ^ 3] = (Uint8)strtol(buf, NULL, 16);
	}
	return true;
}

// Read image file in either Intel HEX or binary format. The 'desc' parameter
// is used to identify the image in diagnostics.
image* SerialCtl::read_img(string& filename, Uint32 range_start, Uint32 range_len, Uint32 alignment)
{
	FILE *file = NULL;
	fopen_s(&file,filename.c_str(), "rb");
	if (!file) 
	{
		return NULL;
	}

	image *img = new image(range_start, range_len, alignment);

	// Look at first byte of file to decide whether it's Intel HEX.
	if (fgetc(file) == ':') 
	{
		// Assume it's an Intel hex file.
		/*
		rewind，C 程序中的库函数，功能是将文件内部的指针重新指向一个流的开头。
		函数名: rewind()
		功 能: 将文件内部的位置指针重新指向一个流（数据流/文件）的开头
		注意：不是文件指针而是文件内部的位置指针，随着对文件的读写文件的位置指针（指向当前读写字节）向后移动。而文件指针是指向整个文件，如果不重新赋值文件指针不会改变。
		rewind函数作用等同于 (void)fseek(stream, 0L, SEEK_SET);
		*/
		rewind(file);
		int res = IHEX_ReadFile(file, hex_read_callback, img);
		if (res != IHEX_OK) {
			if (res == IHEX_ADDR_ERROR)
			{
				//Out-of-range address in Intel HEX
			}
			else
			{
				//Invalid Intel HEX format;
			}
				
			goto fail;
		}

		if (!img->len) {
			//Empty Intel HEX;
			goto fail;
		}

		//Read Intel HEX bytes ok;
	}
	else 
	{
		// Assume it's a binary file.
		int32 file_len;
		if (fseek(file, 0, SEEK_END) != 0 || (file_len = ftell(file)) < 0) 
		{
			//Failed to determine length of binary
			goto fail;
		}

		if (!file_len) 
		{
			// Empty binary;
			goto fail;
		}

		if ((Uint32)file_len > range_len)
		{
			//Binary bytes too large for target
			goto fail;
		}

		Uint32 len = pad(file_len, alignment);

		chunk *new_chunk = new chunk;
		new_chunk->addr = range_start;
		new_chunk->data.resize(len, 0xFF);

		img->first_chunk = img->last_chunk = new_chunk;
		img->len = len;
		img->max_addr = range_start + len;

		rewind(file);

		if (fread(&new_chunk->data[0], 1, file_len, file) != (Uint32)file_len)
		{
			//Failed to read binary;
			goto fail;
		}

		//Read binary bytes ok
	}
	fclose(file);
	return img;

fail:
	delete img;
	fclose(file);
	return NULL;
}

// Callback function for call to IHEX_ReadFile.
// Collects data into dynamically (re)allocated block.
int hex_read_callback(void *ctxt, const Uint8 *buf, Uint32 addr, Uint32 len)
{
	image *img = static_cast<image *>(ctxt);

	if ((addr < img->range_start) || (addr + len > img->range_start + img->range_len))
		return IHEX_ADDR_ERROR;

	chunk *last_chunk = img->last_chunk;
	if (!last_chunk || addr >= last_chunk->addr + last_chunk->data.size() + img->alignment) 
	{
		chunk *new_chunk = new chunk;
		new_chunk->addr = addr & ~(img->alignment - 1);
		if (last_chunk)
			last_chunk->next = new_chunk;
		else
			img->first_chunk = new_chunk;
		last_chunk = img->last_chunk = new_chunk;
	}
	//定义vecotr引用
	vector<Uint8>& data = last_chunk->data;
	int32 old_size = data.size();
	int32 new_size = g_serial->pad(addr + len - last_chunk->addr, img->alignment);
	data.resize(new_size, 0xFF);
	memcpy_s(&data[addr - last_chunk->addr],len,buf, len);

	img->len += new_size - old_size;
	img->max_addr = max(img->max_addr, addr + len);

	return 0;
}

int32 SerialCtl::pad(int32 len, int32 align) 
{
	return (len + (align - 1)) & (~(align - 1));
}

bool SerialCtl::do_autobaud(int16 axis, Uint8 c, Uint8 *buf, Uint32 len, int16 com_type, int16 stationId)
{
	Uint32 written;
	if ((GTSD_CMD_WriteSerialPort(axis, &c, 1, (int32*)&written, com_type,stationId) != 0) || written != 1)
	{
		
		//Write of autobaud character failed;
		return FALSE;
	}

	Uint8 *read_buf = buf;
	Uint32 read_len = len;


	Uint32 received;
	if ((GTSD_CMD_ReadSerialPort(axis, read_buf, read_len, (int32*)&received, com_type,stationId) != 0) || !received)
	{
		//No autobaud response;
		return false;
	}

	if (received != len) 
	{
		//Invalid autobaud response;
		return false;
	}

	return true;
}

bool SerialCtl::do_ldr_autobaud(int16 axis, int16 com_type, int16 stationId)
{
	Uint8 buf[4];
	if (!do_autobaud(axis, '@', buf, sizeof(buf),com_type, stationId))
		return false;

	if (buf[0] != 0xBF || buf[3] != 0) 
	{
		// Invalid autobaud response;
		return false;
	}

	//Autobaud succeeded;
	return true;
}

bool SerialCtl::send_erase_cmd(int16 axis, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 com_type, int16 stationId)
{
	if (!do_ldr_autobaud(axis,com_type, stationId))
		return false;

	Uint8 cmd[4] = { 0xBA, 0xB3, 0xD0, 0x48 };

	Uint32 written = 0;
	if ((GTSD_CMD_WriteSerialPort(axis, cmd, sizeof(cmd),(int32*) &written, com_type) != 0) || written != sizeof(cmd))
	{
		//Sending erase command failed;
		return false;
	}

	//Erase command sent. If the flash is not locked, the command is ignored;
	//If the flash is locked, the erase operation may take up to 4 minutes.");
	//Completion is not reported back to the host, but is indicated by the");
	//SYS_FAULT pin connected to a red LED on ADSP-CM40x EZ-KITs.");
	Uint32 index = 0;
	for (int32 i = 1; i <= 240; i++)
	{
		Sleep(1000);
		index++;
		progress = (int16)(10 + (int32)((((double)index) / 240) * 90));
		if (progress >= 100)
		{
			progress = 100;
		}
		(*tpfUpdataProgressPt)(ptrv, &progress);
	}
	return true;
}
