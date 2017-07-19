//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	Communicaiton Packet Define		 							//
//	file				:	Packet.h													//
//	Description			:	use for pc and fpga communicaiton							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef		__GTSD_PACKET__
#define		__GTSD_PACKET__

#include "Basetype_def.h"


const int32			MAC_ADDR_BYTE_LENTH			=	6;							// MAC地址的byte长度
const int32			MAX_PACKET_LEN				=	1514;						// 发送和接收数据buf的最大长度
const int32 		MIN_PACKET_LEN				=	34;							// 返回包最小长度限制
const int32 		PACKET_HEADER_LENTH			=	21;							// 包头长度
const int32 		MIN_SEND_PKT_LEN			=	64;							// 最小的发送包的长度，不够的后面补零

const int32 		RETURN_REQUEST_MASK			=	0x0002;						// 查询返回标志的掩码
const int32 		FPGA_QUERY_ADDR0			=	0x0002;						// 查询FPGA标志位，如果为1，则可以读取返回值
const int32 		FPGA_QUERY_ADDR1			=	0x8002;						// 查询FPGA标志位，如果为1，则可以读取返回值,其中最高位为了区分是哪个DSP


const int32 		LENTH_INCLUDE				=	5;							// lenth包括两部分，从index到addr一共有5个byte 加上后面的data是dma_num定义的 dma_num*2+5 = length
const int32 		LENTH_BEFORE				=	16;							// lenth之前的字节长度（包括lenth字节一共有16个byte，其中dmac和smac都分别占8byte）

const int32			PACKET_MODE_READ_FPGA		=	0;							// FPGA读模式
const int32			PACKET_MODE_WRITE_FPGA		=	1;							// FPGA写模式

const int32			LENTH_MASK					=	0x03ff;						// 因为长度分别在两个字节中，所以要用mask获取其对应位的值
const int32			DMA_NUMBER_MASK				=	0x01ff;						// 因为NUMBER分别在两个字节中，所以要用mask获取其对应位的值
const int32			LENTH_MASK_ZERO				=	0x1000;						// 把lenth对应位置零
const int32			LENTH_MASK_ONE				=	0x13ff;						// 把lenth对应位置1
const int32			DMA_NUMBER_MASK_WRITE		=   0x8000;						// mask 写模式

const int32			NET_COM_EXECUTE_FAIL		=	0;							// 命令执行失败
const int32			NET_COM_EXECUTE_SUCCESS		=	1;							// 命令执行成功
const int32			NET_COM_PARAMETER_INVALID	=	2;							// 无效参数
const int32			NET_COM_INSTRUCTION_INVALID =	3;							// 非法指令

//////////////////////////////////////////////////////////////////////////
//DSP-FPGA-PC NET Transfer DATA format
//data0---data5 used for {cmd id ,axis number ,cmd type, return type}  
//////////////////////////////////////////////////////////////////////////
#pragma pack(1)
typedef struct packetformat
{
	//header format start
	Uint8 dmac[MAC_ADDR_BYTE_LENTH];		//  mac address
	Uint8 smac[MAC_ADDR_BYTE_LENTH];		// source mac address
	Uint8 type0;							// used for identify device
	Uint8 type1;							// used for identify device
	Uint8 lengthbit7_0;						// length[7:0] 
	Uint8 lengthbit9_8;						// {6’b000100, length[9:8]}
	Uint8 index;							// index number 
	Uint8 dma_numbit7_0;					// dma_num[7:0]
	Uint8 dma_numbit8;						// {cmd,6’b0,dma_num[8]}
	Uint8 dmaAddrbit7_0;					// dma addr[7:0]
	Uint8 dmaAddrbit15_8;					// dma addr[15:8]
	//header format end
	Uint8 dataSection[MAX_PACKET_LEN - PACKET_HEADER_LENTH];		// user data section 最大包长减去包头长度 

	////first six data byte used for transfer the cmd  
	//Uint8 data0_CmdIdbit7_0;				// cmd id bit7:0
	//Uint8 data1_CmdIdbit11_8_axisnum;		// cmd id bit11:8 15:12 --axis number
	//Uint8 data2_CmdType;					// cmd type 0:read 1:write
	//Uint8 data3;							// rsv
	//Uint8 data4_ret;						// return type
	//Uint8 data5;							// rsv
	
}PACKET_FORMAT;
#pragma pack()

class CPacket
{
public:
	CPacket(void);							// constructor
	~CPacket(void);							// destructor 
protected:
	Uint8				m_tx_index;			//send package index
	Uint8				m_rx_index;			//receive package index
public:
	PACKET_FORMAT*		pTx;				// send data buf
	PACKET_FORMAT*		pRx;				// receive data buf
	Uint8				mac[256];			// mac addr	获取所有的网卡mac
	bool				bHaveMac;			// whether we have the local mac addr
	Uint32				rx_length;			// receive data lenth
	Uint32				tx_length;			// send data lenth
public:

	//get tx index
	Uint8 get_tx_index();

	/*******************************************************************************************
	功能：	解码读写结束标志位命令的返回值
	输入：	无
	返回值：true表示读到标志位,可以读取返回值
	*******************************************************************************************/
	bool decoder_return_flag(int16 mode);

	/*******************************************************************************************
	功能：解码读写dsp命令的返回值
	输入：
	index：		命令索引值
	pData:		输入数据
	dam_num:	输入数据长度
	输出：
	pData:		读取数据
	dam_num:	读取数据长度
	返回值：  0表示成功，其他参看错误列表
	*******************************************************************************************/
	int16 decoder_dsp_packet(Uint8 index, int16* pData, int16 &dma_num);

	/*******************************************************************************************
	功能：解码读写FPGA命令的返回值
	输入：
	index：		命令索引值
	pData:		输入数据
	dam_num:	输入数据长度
	输出：
	pData:		读取数据
	dam_num:	读取数据长度
	返回值：  0表示成功，其他参看错误列表
	*******************************************************************************************/
	int16 decoder_fpga_packet(Uint8 index, int16* pData, int16 &dma_num);

	/*******************************************************************************************
	功能：获取本地mac地址
	输入：adapterName：适配器名称
	*******************************************************************************************/
	void get_local_mac(const int8* adapterName);


	/*******************************************************************************************
	功能：填充包中的数据
	输入：
	dma_addr：	访问地址
	pData:		输入数据
	dam_num:	输入数据长度
	FPGAmode：	读还是写
	*******************************************************************************************/
	void fillPacket(int16 dma_addr, int16* pData, int16 dma_num, int16 FPGAmode);


protected:
	void init_pack();
};
#endif