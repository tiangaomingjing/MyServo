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


const int32			MAC_ADDR_BYTE_LENTH			=	6;							// MAC��ַ��byte����
const int32			MAX_PACKET_LEN				=	1514;						// ���ͺͽ�������buf����󳤶�
const int32 		MIN_PACKET_LEN				=	34;							// ���ذ���С��������
const int32 		PACKET_HEADER_LENTH			=	21;							// ��ͷ����
const int32 		MIN_SEND_PKT_LEN			=	64;							// ��С�ķ��Ͱ��ĳ��ȣ������ĺ��油��

const int32 		RETURN_REQUEST_MASK			=	0x0002;						// ��ѯ���ر�־������
const int32 		FPGA_QUERY_ADDR0			=	0x0002;						// ��ѯFPGA��־λ�����Ϊ1������Զ�ȡ����ֵ
const int32 		FPGA_QUERY_ADDR1			=	0x8002;						// ��ѯFPGA��־λ�����Ϊ1������Զ�ȡ����ֵ,�������λΪ���������ĸ�DSP


const int32 		LENTH_INCLUDE				=	5;							// lenth���������֣���index��addrһ����5��byte ���Ϻ����data��dma_num����� dma_num*2+5 = length
const int32 		LENTH_BEFORE				=	16;							// lenth֮ǰ���ֽڳ��ȣ�����lenth�ֽ�һ����16��byte������dmac��smac���ֱ�ռ8byte��

const int32			PACKET_MODE_READ_FPGA		=	0;							// FPGA��ģʽ
const int32			PACKET_MODE_WRITE_FPGA		=	1;							// FPGAдģʽ

const int32			LENTH_MASK					=	0x03ff;						// ��Ϊ���ȷֱ��������ֽ��У�����Ҫ��mask��ȡ���Ӧλ��ֵ
const int32			DMA_NUMBER_MASK				=	0x01ff;						// ��ΪNUMBER�ֱ��������ֽ��У�����Ҫ��mask��ȡ���Ӧλ��ֵ
const int32			LENTH_MASK_ZERO				=	0x1000;						// ��lenth��Ӧλ����
const int32			LENTH_MASK_ONE				=	0x13ff;						// ��lenth��Ӧλ��1
const int32			DMA_NUMBER_MASK_WRITE		=   0x8000;						// mask дģʽ

const int32			NET_COM_EXECUTE_FAIL		=	0;							// ����ִ��ʧ��
const int32			NET_COM_EXECUTE_SUCCESS		=	1;							// ����ִ�гɹ�
const int32			NET_COM_PARAMETER_INVALID	=	2;							// ��Ч����
const int32			NET_COM_INSTRUCTION_INVALID =	3;							// �Ƿ�ָ��

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
	Uint8 lengthbit9_8;						// {6��b000100, length[9:8]}
	Uint8 index;							// index number 
	Uint8 dma_numbit7_0;					// dma_num[7:0]
	Uint8 dma_numbit8;						// {cmd,6��b0,dma_num[8]}
	Uint8 dmaAddrbit7_0;					// dma addr[7:0]
	Uint8 dmaAddrbit15_8;					// dma addr[15:8]
	//header format end
	Uint8 dataSection[MAX_PACKET_LEN - PACKET_HEADER_LENTH];		// user data section ��������ȥ��ͷ���� 

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
	Uint8				mac[256];			// mac addr	��ȡ���е�����mac
	bool				bHaveMac;			// whether we have the local mac addr
	Uint32				rx_length;			// receive data lenth
	Uint32				tx_length;			// send data lenth
public:

	//get tx index
	Uint8 get_tx_index();

	/*******************************************************************************************
	���ܣ�	�����д������־λ����ķ���ֵ
	���룺	��
	����ֵ��true��ʾ������־λ,���Զ�ȡ����ֵ
	*******************************************************************************************/
	bool decoder_return_flag(int16 mode);

	/*******************************************************************************************
	���ܣ������дdsp����ķ���ֵ
	���룺
	index��		��������ֵ
	pData:		��������
	dam_num:	�������ݳ���
	�����
	pData:		��ȡ����
	dam_num:	��ȡ���ݳ���
	����ֵ��  0��ʾ�ɹ��������ο������б�
	*******************************************************************************************/
	int16 decoder_dsp_packet(Uint8 index, int16* pData, int16 &dma_num);

	/*******************************************************************************************
	���ܣ������дFPGA����ķ���ֵ
	���룺
	index��		��������ֵ
	pData:		��������
	dam_num:	�������ݳ���
	�����
	pData:		��ȡ����
	dam_num:	��ȡ���ݳ���
	����ֵ��  0��ʾ�ɹ��������ο������б�
	*******************************************************************************************/
	int16 decoder_fpga_packet(Uint8 index, int16* pData, int16 &dma_num);

	/*******************************************************************************************
	���ܣ���ȡ����mac��ַ
	���룺adapterName������������
	*******************************************************************************************/
	void get_local_mac(const int8* adapterName);


	/*******************************************************************************************
	���ܣ������е�����
	���룺
	dma_addr��	���ʵ�ַ
	pData:		��������
	dam_num:	�������ݳ���
	FPGAmode��	������д
	*******************************************************************************************/
	void fillPacket(int16 dma_addr, int16* pData, int16 dma_num, int16 FPGAmode);


protected:
	void init_pack();
};
#endif