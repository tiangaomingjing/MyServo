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

	//----------------------------------------------------------------------------
	//ʵ�ִ򿪴��ڵĹ��ܣ�ͨ�����������FPGA��ַ
	if (axis >= COM_AXIS_MAX)
	{
		return Net_Rt_param_Err;
	}
	int16 Axis			= axis;												//���
	int16 dsp_number	= GTSD_DSP_A;
	//������ż������ĸ�dsp
	if (Axis > 1)
	{
		dsp_number = (int16)(GTSD_DSP_B);
	}
	//----------------------------------------------------------------------------
	int16 com_addr		= FPGA_UART_CONFIG_W;								//д���õ�ַ
	int16 com_addr1		= FPGA_UART_BAUDRATE_WR;							//��д�����ʵ�ַ
	int16 com_addr2		= FPGA_UART_STARTEND_W;								//д��ģ���ַ
	int16 com_addr3		= FPGA_UART_CONFIG_R;								//��ȡ�Ƿ��״̬��ַ

	//����dsp��ѡ��FPGA����ַ
	int16 base_addr;
	if (dsp_number == GTSD_DSP_A)
	{
		base_addr = (int16)FPGA_DSPA_UART_BASEADDR;
	}
	else if (dsp_number == GTSD_DSP_B)
	{
		base_addr = (int16)FPGA_DSPB_UART_BASEADDR;
	}
	else
	{
		return Net_Rt_param_Err;
	}
	//����ʵ�ʵĵ�ַ
	int16 comAddr	= base_addr + (com_addr>>1);
	int16 comAddr1	= base_addr + (com_addr1 >> 1);
	int16 comAddr2	= base_addr + (com_addr2 >> 1);
	int16 comAddr3	= base_addr + (com_addr3 >> 1);
	//----------------------------------------------------------------------------
	//��ģ��
	int16 comNum		= 1;
	int16 startValue	= (int16)0xFF00; //д��ֵ�򿪸�ģ��
	int16 rtn			= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr2, &startValue, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	//��ȡ��ֵ��֤�Ƿ��Ѿ��򿪸�ģ��
	int16 startVauleRtn;
	rtn					= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr3, &startVauleRtn, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	//���bit15��1��˵���Ѿ��򿪣�����δ�򿪳ɹ�
	if ((startVauleRtn &0x8000)==0)
	{
		return Net_Rt_param_Err;
	}
	//----------------------------------------------------------------------------
	//�����·��Ĳ����� (baudRate unit bps),ת��Ϊns
	double BD		= (1.0 / baudRate)*1000.0*1000.0*1000.0;
	double clk		= (1.0 / FPGA_CLOCK)*1000.0*1000.0*1000.0;
	
	//download �Ĳ�������BD/clk;
	int16  dl_bd		= (int16)(BD / clk)-1;
	int16  dl_bds		= dl_bd;
	int16  dl_cfg		= (0x0700);

	//����ֹͣλ����żУ�飬����λ���
	rtn					= g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &dl_cfg, comNum, stationId);
	if (rtn != GTSD_COM_SUCCESS)
	{
		return false;
	}
	//��ȡ���ý�����֤
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
	//���ò�����
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
		//ʵ�ֹرմ��ڵĹ��ܣ�ͨ�����������FPGA��ַ
		if (axis >= COM_AXIS_MAX)
		{
			return Net_Rt_param_Err;
		}
		int16 Axis			= axis;												//���
		int16 dsp_number	= GTSD_DSP_A;
		//������ż������ĸ�dsp
		if (Axis > 1)
		{
			dsp_number = (int16)(GTSD_DSP_B);
		}
		int16 com_addr		= FPGA_UART_STARTEND_W;								//�ر�ģ���ַ
		int16 com_addr1		= FPGA_UART_CONFIG_R;								//�鿴�Ƿ�δʹ��
		//����dsp��ѡ��FPGA����ַ
		int16 base_addr;
		if (dsp_number == GTSD_DSP_A)
		{
			base_addr = (int16)FPGA_DSPA_UART_BASEADDR;
		}
		else if (dsp_number == GTSD_DSP_B)
		{
			base_addr = (int16)FPGA_DSPB_UART_BASEADDR;
		}
		else
		{
			return Net_Rt_param_Err;
		}
		//����ʵ�ʵĵ�ַ
		int16 comAddr	= base_addr + (com_addr >> 1);
		int16 comAddr1	= base_addr + (com_addr1 >> 1);
		//----------------------------------------------------------------------------
		//�ر�ģ��
		int16 comNum = 1;
		int16 stopValue = 0x0000; //д��ֵ�رո�ģ��
		int16 rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_WRITE, comAddr, &stopValue, comNum, stationId);
		if (rtn != GTSD_COM_SUCCESS)
		{
			return false;
		}
		//��ȡ��ֵ��֤�Ƿ��Ѿ��رո�ģ��
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
	//ʵ�ֶ����ڵĹ��ܣ�ͨ�����������FPGA��ַ
	if (status == true)
	{
		if (axis >= COM_AXIS_MAX)
		{
			return Net_Rt_param_Err;
		}
		int16 Axis			= axis;												//���
		int16 dsp_number	= GTSD_DSP_A;
		int16 com_addr		= FPGA_UART_RX_STATE_R;								// RX FIFO STATE
		int16 com_addr1		= FPGA_UART_RECEIVE_FIFO_R;							// RX FIFO 

		//������ż������ĸ�dsp
		if (Axis > 1)
		{
			dsp_number = (int16)(GTSD_DSP_B);
		}
		//����dsp��ѡ��FPGA����ַ
		int16 base_addr;
		if (dsp_number == GTSD_DSP_A)
		{
			base_addr = (int16)FPGA_DSPA_UART_BASEADDR;
		}
		else if (dsp_number == GTSD_DSP_B)
		{
			base_addr = (int16)FPGA_DSPB_UART_BASEADDR;
		}
		else
		{
			return Net_Rt_param_Err;
		}
		//����ʵ�ʵĵ�ַ
		int16 comAddr = base_addr + (com_addr >> 1);
		int16 comAddr1 = base_addr + (com_addr1 >> 1);
		//----------------------------------------------------------------------------
		//��ѯRX״̬
		int16 comNum = 1;
		int16 stateValue = 0x0000;
		int16 rtn;
		//----------------------------------------------------------------------------
		do 
		{	
			//��ѯ��ǰ rx fifo���ж������ݡ�
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &stateValue, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
			//��ѯfifo�Ƿ��������մ���
			if (stateValue & 0x4000)
			{
				return false;
			}
		} while ((stateValue&0x03ff)<length);
	
		//������յ��㹻�����ݣ���ô�Ϳ�ʼ��fifo�ж�ȡ����
		int16 tmp;
		int32 i;
		for (i = 0; i < length; ++i)
		{
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr1, &tmp, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
			//16bit����ת��Ϊ8bit���ݣ�16bit���ݵĸ�8bit��Ч��
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
	//ʵ��д���ڵĹ��ܣ�ͨ�����������FPGA��ַ
	if (status == true)
	{
		if (axis >= COM_AXIS_MAX)
		{
			return Net_Rt_param_Err;
		}
		int16 Axis			= axis;												//���
		int16 dsp_number	= GTSD_DSP_A;
		int16 com_addr		= FPGA_UART_RX_STATE_R;								//RTS�źŶ�ȡ��Ϊ0��ʾ���Է������ݵ�uart��Ϊ1ֹͣ���ͣ��ù�����FPGA��������ֻ�ǲ�ѯ�ñ�־
		int16 com_addr1		= FPGA_UART_TX_STATE_R;								// TX FIFO ״̬
		int16 com_addr2		= FPGA_UART_SEND_FIFO_W;							// tx fifo
		//������ż������ĸ�dsp
		if (Axis > 1)
		{
			dsp_number = (int16)(GTSD_DSP_B);
		}
		//����dsp��ѡ��FPGA����ַ
		int16 base_addr;
		if (dsp_number == GTSD_DSP_A)
		{
			base_addr = (int16)FPGA_DSPA_UART_BASEADDR;
		}
		else if (dsp_number == GTSD_DSP_B)
		{
			base_addr = (int16)FPGA_DSPB_UART_BASEADDR;
		}
		else
		{
			return Net_Rt_param_Err;
		}
		//����ʵ�ʵĵ�ַ
		int16 comAddr = base_addr + (com_addr >> 1);
		int16 comAddr1 = base_addr + (com_addr1 >> 1);
		int16 comAddr2 = base_addr + (com_addr2 >> 1);
		//----------------------------------------------------------------------------
		//��ѯRTS״̬
		int16 comNum = 1;
		int16 stateValue = 0x0000; 
		int16 rtn;
		do 
		{
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr, &stateValue, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
		} while (stateValue & 0x2000);//����bit13��1��ʾоƬδ׼���ý��գ���ô�Ȳ�Ҫ��������
		
		//----------------------------------------------------------------------------
		//��ѯ��ǰ����fifo���ж��ٿռ䡣
		do
		{
			rtn = g_AbsCom->GTSD_Com_Firmware_handler(com_type, GTSD_COM_MODE_READ, comAddr1, &stateValue, comNum, stationId);
			if (rtn != GTSD_COM_SUCCESS)
			{
				return false;
			}
			//��ѯfifo�Ƿ��������
			if (stateValue & 0x1000)
			{
				return false;
			}
		} while ((1024 - (int16)(stateValue & 0x07ff))<length);

		//����ռ��㹻����ô�Ϳ�ʼ�������ݡ�дtx fifo
		int16 tmp;
		int32 i;
		for (i = 0; i < length;++i)
		{
			//8bit����ת��Ϊ16bit���ݣ�16bit���ݵĸ�8bit��Ч��
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
	//�ж��Ƿ�key����
	if ((!inputKey.empty()) && (!parse_key(inputKey, key)))
		return false;

	unique_ptr<image> img(read_img(filename, 0, 0x80000000, 4));
	/*
	��������ָ���������ָ��������� get() ������
	��������ָ����һ����������if (my_smart_object)��ԶΪ�棬
	Ҫ�ж�����ָ�����ָ���Ƿ�Ϊ�գ���Ҫ�����жϣ�if (my_smart_object.get())��
	*/
	if (!img.get())
		return false;

	if (img->first_chunk != img->last_chunk) 
	{
		//Fragmented boot stream;
		return false;
	}

	//�ٷֱȽ���
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
	//�ٷֱȽ���
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
	//�ٷֱȽ���
	progress = 35;
	(*tpfUpdataProgressPt)(ptrv, &progress);

	// Download blocks
	addr = 0;
	Uint32 chunk_size = baudrate / 50;  // About 0.2 seconds per chunk.

	if (chunk_size >= 1000)
	{
		chunk_size = 512;
	}

	Uint32 index = 0;
	for (size_t i = 0; i < blocks.size(); i++)
	{
		Uint32 size = blocks[i];

		Uint32 remaining = size;
		while (remaining)
		{
			Uint32 to_write = min(remaining, chunk_size);
			if ((GTSD_CMD_WriteSerialPort(axis, data + addr, to_write, (int32*)&written, com_type,stationId) != 0) || !written)
			{
				// Download failed;
				return false;
			}
			remaining -= written;
			addr += written;
			//Sleep(100);
		}

		index++;
		progress = (int16)(35 + (int32)((((double)index) / blocks.size()) * 65));
		if (progress >= 100)
		{
			progress = 100;
		}
		(*tpfUpdataProgressPt)(ptrv, &progress);

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
		//���ȴ���
		return -1;
	}
	for (int16 i = 0; i < 32; i++) 
	{
		if (!isxdigit(inputKey[i]))
		{
			//��ʽ���󣬲���ʮ����������
			return false;
		}
	}
	// Ϊ�˰������¹������3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12.
	// ʹ�����
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
		rewind��C �����еĿ⺯���������ǽ��ļ��ڲ���ָ������ָ��һ�����Ŀ�ͷ��
		������: rewind()
		�� ��: ���ļ��ڲ���λ��ָ������ָ��һ������������/�ļ����Ŀ�ͷ
		ע�⣺�����ļ�ָ������ļ��ڲ���λ��ָ�룬���Ŷ��ļ��Ķ�д�ļ���λ��ָ�루ָ��ǰ��д�ֽڣ�����ƶ������ļ�ָ����ָ�������ļ�����������¸�ֵ�ļ�ָ�벻��ı䡣
		rewind�������õ�ͬ�� (void)fseek(stream, 0L, SEEK_SET);
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
	//����vecotr����
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