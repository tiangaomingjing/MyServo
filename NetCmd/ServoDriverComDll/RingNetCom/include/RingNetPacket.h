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


const int32			RN_MAX_PACKET_LEN				= 480;							// ���ͺͽ�������buf����󳤶�
const int32 		RN_MIN_PACKET_LEN				= 34;							// ���ذ���С��������
const int32 		RN_PACKET_HEADER_LENTH			= 14;							// ��ͷ����
const int32 		RN_MIN_SEND_PKT_LEN				= 64;							// ��С�ķ��Ͱ��ĳ��ȣ������ĺ��油��
const int32			RN_NUM2LENTH					= 4;							// NUM = LENTH - 4


const int32			RN_NET_COM_EXECUTE_FAIL			= 0;							// ����ִ��ʧ��
const int32			RN_NET_COM_EXECUTE_SUCCESS		= 1;							// ����ִ�гɹ�
const int32			RN_NET_COM_PARAMETER_INVALID	= 2;							// ��Ч����
const int32			RN_NET_COM_INSTRUCTION_INVALID	= 3;							// �Ƿ�ָ��

//////////////////////////////////////////////////////////////////////////

//channel define
#define		RN_PCI_CH_ID	1		//PC ͨ��   //��Ӧ�ڶ���dsp                                           
#define		RN_FPGA_CH_ID	0		//FPGA ͨ��
#define		RN_DSP_CH_ID	2		//dspͨ��   //��Ӧ��һ��dsp

//����cm408��ͨ����
#define		RN_FIRST_DSP_CH		RN_DSP_CH_ID
#define		RN_SECOND_DSP_CH	RN_PCI_CH_ID

//������ͬ��ַ��������
#define		RN_ADDR_SAME		0
#define		RN_ADDR_INC			1

//�Ƿ���ҪӦ��
#define		RN_NOTNEED_REQ		0
#define		RN_NEED_REQ			1

//pdu����mailboxģʽ
#define		RN_MODE_PDU			0
#define		RN_MODE_MAIL		1

//�Ƿ�����Ӧ
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
//����ṹ��Ӧ����Ľṹ��ϸ����ÿһλ�Ķ���

typedef struct
{
	//////////////the first uint16////////////////////////////////////////////////////////////
	Uint16				        len_bit8	: 1;						//data lenth bit 8
	Uint16				        des_ch		: 3;						//when  PC send to dsp ,user[2:0] be 1 or 2 ,when pc send to fpga ,user[2:0] be 0 ,when dsp response,the user[2:0] and user[5:3]change the position
	Uint16				        src_ch		: 3;						//user[5:3] pc channel Ϊ1
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
	Uint8 src_id;							// source station number 0xf0Ϊ����վ��, 0xf1Ϊ��������վ,(use 0xf2,����0xf3...)ͨ���Ȼ������Ӻ��İ��ʱ������Ŀ��idһ����0xf0
	Uint8 ttlbase;							// time to live base (write 0xf0)
	Uint8 sum;								// sum of byte0 to byte4
	Uint8 protocolType0;					// index����ȥ�ظ����� 0xFF��ʼ�Լ�
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
//�ýṹ���ڷ���������ڽ�����ʱ���ж��Ƿ��յ�����ķ���ֵ����Ϊָ���һȥһ�صģ����ж��̱߳������������������ͨ��һ���ӿڷ�������
//��������ף�ÿ�η������ʱ���������ṹ��,������ʱ��ע��Դ��Ŀ��Ҫ�ߵ�һ�¡�
typedef struct CurrentPacketCmd
{
	//��ͷ��
	Uint8	baseControl;								//��������������
	Uint8	des_id;										//Ŀ��station	
	Uint8	src_id;										//Դstation
	//header��
	Uint8	des_channel;								//Ŀ��ͨ��
	Uint8	src_channel;								//Դͨ��
	Uint16	cmd;										//����
	Uint8	axis;										//���
	Uint16  cmdSubId;									//������id
}CURRENTPACKETCMD;

//basecontrol ����
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
	bool				com_sts;				//���ڿ�ʼ����ʱ,�����ĸ���������״̬
	Uint8				index[256];			//���ڲ�ͬ��վ�Ĳ�ͬͨ����index 1024�㹻����ÿ��վ��ÿ��ͨ��


public:
	/*******************************************************************************************
	���ܣ�����Ȼ�����
	*******************************************************************************************/
	int16 decoderPacket(int16* pData, int16 datalenth);

	/*******************************************************************************************
	���ܣ������е�����
	*******************************************************************************************/
	void fillPacket(int16 des_id, int16 des_ch, int16* pData, int16 datalenth, int16 cmd, int16 NeedReq, int16 IsReq, int16 FPGAaddr, int16 mode);

	/*******************************************************************************************
	���ܣ����ͨ������״̬�������ж��ĸ�����������������
	*******************************************************************************************/
	bool checkComSts();

protected:
	void init_pack();
};




//////////////////////////////////////////////////////////////////////////
#endif