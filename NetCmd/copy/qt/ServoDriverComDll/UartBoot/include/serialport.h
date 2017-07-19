//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	serialport		 											//
//	file				:	serialport.h												//
//	Description			:	use for control the serial port								//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/11/19	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GTSD_SERIALPORT_
#define	_GTSD_SERIALPORT_

#include "Basetype_def.h"

#define IHEX_ADDR_ERROR 5

struct chunk
{
	struct chunk *next;
	Uint32 addr;
	vector<Uint8> data;
	chunk() : next(NULL), addr(0) {}
};

struct image
{
	chunk *first_chunk, *last_chunk;
	Uint32 len;
	Uint32 max_addr;
	Uint32 range_start, range_len;
	Uint32 alignment;
	image(Uint32 range_start, Uint32 range_len, Uint32 alignment)
		: first_chunk(NULL), last_chunk(NULL), len(0), max_addr(0),
		range_start(range_start), range_len(range_len), alignment(alignment) {}
	~image();
};



// class SerialCtl
//-----------------------------------------------------------------------------
// Description: This class handle the functionality that interface with
// the serial communication.
//
class SerialCtl
{
public:
	SerialCtl();  // Constructor
	~SerialCtl(); // Destructor

	enum Action {
		AC_NONE = 0,
		AC_PROGRAM,
		AC_LOAD,
		AC_WRITE_KEY,
		AC_ERASE,
		AC_ERASE_USER,
	} action;

public:
	bool open(int16 axis, int32 baudRate, int16 com_type, int16 stationId);
	bool close(int16 axis, int16 com_type, int16 stationId);
	bool read(int16 axis, Uint8 *buf, int32 length, int32 *length_read, int16 com_type, int16 stationId);
	bool write(int16 axis, Uint8 *buf, int32 length, int32 *length_written, int16 com_type, int16 stationId);

	bool downloadBootStream(int16 axis, string& inputKey, int16 cmd, string& filename, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 com_type, int16 stationId);
	bool send_ldr_img(int16 axis, string& inputKey, string& filename, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 com_type, int16 stationId);
	int16 parse_key(string& inputKey, Uint8 key[16]);
	image* read_img(string& filename, Uint32 range_start, Uint32 range_len, Uint32 alignment);
	int32 pad(int32 len, int32 align);
	bool do_autobaud(int16 axis, Uint8 c, Uint8 *buf, Uint32 len, int16 com_type, int16 stationId);
	bool do_ldr_autobaud(int16 axis, int16 com_type, int16 stationId);
	bool send_erase_cmd(int16 axis, void(*tpfUpdataProgressPt)(void*, int16*), void* ptrv, int16& progress, int16 com_type, int16 stationId);

private:
	bool status;
	int32 baudrate;
};
int hex_read_callback(void *ctxt, const Uint8 *buf, Uint32 addr, Uint32 len);

extern SerialCtl* g_serial;

#endif //SERIAL_CTL_HPP

/****************************End of file**************************************/
