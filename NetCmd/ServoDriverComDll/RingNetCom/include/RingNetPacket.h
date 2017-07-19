//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	ring net Communicaiton Packet Define		 				//
//	file				:	RingNetPacket.h												//
//	Description			:	use for pc using Rn communicaiton							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/12/30	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef		__GTSD_RN_PACKET__
#define		__GTSD_RN_PACKET__

#include "Basetype_def.h"


const int32			RN_MAX_PACKET_LEN				= 480;							// 发送和接收数据buf的最大长度
const int32 		RN_MIN_PACKET_LEN				= 34;							// 返回包最小长度限制
const int32 		RN_PACKET_HEADER_LENTH			= 14;							// 包头长度
const int32 		RN_MIN_SEND_PKT_LEN				= 64;							// 最小的发送包的长度，不够的后面补零
const int32			RN_NUM2LENTH					= 4;							// NUM = LENTH - 4


const int32			RN_NET_COM_EXECUTE_FAIL			= 0;							// 命令执行失败
const int32			RN_NET_COM_EXECUTE_SUCCESS		= 1;							// 命令执行成功
const int32			RN_NET_COM_PARAMETER_INVALID	= 2;							// 无效参数
const int32			RN_NET_COM_INSTRUCTION_INVALID	= 3;							// 非法指令

//////////////////////////////////////////////////////////////////////////

//channel define
#define		RN_PCI_CH_ID	1		//PC 通道   //对应第二个dsp                                           
#define		RN_FPGA_CH_ID	0		//FPGA 通道
#define		RN_DSP_CH_ID	2		//dsp通道   //对应第一个dsp

//两个cm408的通道号
#define		RN_FIRST_DSP_CH		RN_DSP_CH_ID
#define		RN_SECOND_DSP_CH	RN_PCI_CH_ID

//访问相同地址还是自增
#define		RN_ADDR_SAME		0
#define		RN_ADDR_INC			1

//是否需要应答
#define		RN_NOTNEED_REQ		0
#define		RN_NEED_REQ			1

//pdu还是mailbox模式
#define		RN_MODE_PDU			0
#define		RN_MODE_MAIL		1

//是否是响应
#define		RN_ISNOT_REQ		0
#define		RN_IS_REQ			1

//-----------------------------------------------------------------------------
//cmd define 
#define		RN_LWR		0x0                                                     //logic address write
#define		RN_AWR		0x1                                                     //abs address write
#define		RN_LMWR		0x2                                                     //logic address map write
#define		RN_TCWR		0x3                                                     //packet send cfg address write
#define		RN_LRD		0x4                                                     //logic address read
#define		RN_ARD		0x5                                                     //abs address write
#define		RN_LMRD		0x6                                                     //logic address map read
#define		RN_TCRD		0x7                                                     //packet send cfg address read

//------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////
//this format is same as cm408 packet header struct ,except this do not need the first uint16 
typedef	struct
{
	Uint16 data0;
	Uint16 data1;
	Uint16 data2;
}RN_PC_PACKET_HEAD_ALL;

//////////////////////////////////////////////////////////////////////////
//这个结构对应上面的结构，细化出每一位的定义

typedef struct
{
	//////////////the first uint16////////////////////////////////////////////////////////////
	Uint16				        len_bit8	: 1;						//data lenth bit 8
	Uint16				        des_ch		: 3;						//when  PC send to dsp ,user[2:0] be 1 or 2 ,when pc send to fpga ,user[2:0] be 0 ,when dsp response,the user[2:0] and user[5:3]change the position
	Uint16				        src_ch		: 3;						//user[5:3] pc channel 为1
	Uint16				        mode		: 1;                        //1:mailbox 0:pdu        
	Uint16				        len_low		: 8;                        //data lenth low bit7-0
	///////////////the second uint16////////////////////////
	Uint16				        cmd			: 3;                        //this cmd is refer to the cmd define (AWR and AWD)
	Uint16				        delta		: 1;                        //address delta 0:acess the same address,1:acess the address++
	Uint16				        resp_req	: 1;                        //whether need response, 0:do not need response.1:need response when the cmd is 4,5,6,7,this bit must be 1 
	Uint16				        resp		: 1;                        //resp state
	Uint16				        num_bit8	: 1;                        //address times bit8
	Uint16				        resp_ok		: 1;                        //not use
	Uint16				        num_low		: 8;                        //address times lowbit7-0
	/////////////the third uint16///////////////////////////
	Uint16 				        start_addr_bit7_0	:8;                 //start address ,low 8bit in the start, high 8bit in the end 
	Uint16						start_addr_bit15_8	:8;		
}RN_PC_PACKET_HEAD_BITS;

//////////////////////////////////////////////////////////////////////////
typedef union
{
	RN_PC_PACKET_HEAD_ALL	    all;
	RN_PC_PACKET_HEAD_BITS		bit;

}RN_PC_PACKET_HEAD;

//////////////////////////////////////////////////////////////////////////
//RINGNET Transfer DATA format
//data0---data5 used for {cmd id ,axis number ,cmd type, return type}  
//////////////////////////////////////////////////////////////////////////
#pragma pack(1)
typedef struct RnPacketFormat
{
	//header format start
	Uint8 smac[6];
	Uint8 dmac[6];
	Uint8 protocal_type[2];					// 0x55d5
	Uint8 ttl;								//  time to live  (write 0)
	Uint8 basecontrol;						// basecontrol [2:0]
											// PACKET_OFFSET_GEN = 0,
											// PACKET_OFFSET_RESP = 1,
											// PACKET_SYNC = 2,
											// PACKET_USER = 4,//(now use this)
											// PACKET_EXTERN = 7,

	Uint8 des_id;							// destination station id number 
	Uint8 src_id;							// source station number 0xf0为特殊站点, 0xf1为监听所有站,(use 0xf2,或者0xf3...)通过等环网不接核心板的时候下面目标id一定是0xf0
	Uint8 ttlbase;							// time to live base (write 0xf0)
	Uint8 sum;								// sum of byte0 to byte4
	Uint8 protocolType0;					// index用于去重复报文 0xFF开始自加
	Uint8 protocolType1;					// write 0xff
	RN_PC_PACKET_HEAD	header;				// packet header
	//header format end
	Uint8 dataSection[RN_MAX_PACKET_LEN];	// user data section 

	////first six data byte used for transfer the cmd  
	//Uint8 data0_CmdIdbit7_0;				// cmd id bit7:0
	//Uint8 data1_CmdIdbit11_8_axisnum;		// cmd id bit11:8 15:12 --axis number
	//Uint8 data2_CmdType;					// cmd type 0:read 1:write
	//Uint8 data3;							// rsv
	//Uint8 data4_ret;						// return type
	//Uint8 data5;							// rsv

}RN_PACKET_FORMAT;
#pragma pack()
//////////////////////////////////////////////////////////////////////////
//该结构用于发送命令后，在解析的时候判断是否收到命令的返回值，因为指令都是一去一回的，进行多线程保护，否则各个函数都通过一个接口发送命令
//命令会乱套，每次发命令的时候更新这个结构体,解析的时候注意源和目标要颠倒一下。
typedef struct CurrentPacketCmd
{
	//包头中
	Uint8	baseControl;								//包基本控制类型
	Uint8	des_id;										//目标station	
	Uint8	src_id;										//源station
	//header中
	Uint8	des_channel;								//目标通道
	Uint8	src_channel;								//源通道
	Uint16	cmd;										//命令
	Uint8	axis;										//轴号
	Uint16  cmdSubId;									//命令子id
}CURRENTPACKETCMD;

//basecontrol 类型
typedef enum
{
	PACKET_OFFSET_GEN	= 0,
	PACKET_OFFSET_RESP	= 1,
	PACKET_SYNC			= 2,
	PACKET_USER			= 4,
	PACKET_EXTERN		= 7
}BaseControlType;
//////////////////////////////////////////////////////////////////////////
class CRnPacket
{
public:
	CRnPacket(void);							// constructor
	~CRnPacket(void);							// destructor 
public:
	RN_PACKET_FORMAT*	pTx;					// send data buf
	RN_PACKET_FORMAT*	pRx;					// receive data buf
	Uint16				rx_length;				// receive data lenth
	Uint16				tx_length;				// send data lenth
	bool				com_sts;				//用于开始连接时,连接哪个适配器的状态
	Uint8				index[256];			//用于不同的站的不同通道的index 1024足够覆盖每个站的每个通道


public:
	/*******************************************************************************************
	功能：解码等环网包
	*******************************************************************************************/
	int16 decoderPacket(int16* pData, int16 datalenth);

	/*******************************************************************************************
	功能：填充包中的数据
	*******************************************************************************************/
	void fillPacket(int16 des_id, int16 des_ch, int16* pData, int16 datalenth, int16 cmd, int16 NeedReq, int16 IsReq, int16 FPGAaddr, int16 mode);

	/*******************************************************************************************
	功能：检查通信连接状态，用于判断哪个适配器进行相连。
	*******************************************************************************************/
	bool checkComSts();

protected:
	void init_pack();
};




//////////////////////////////////////////////////////////////////////////
#endif