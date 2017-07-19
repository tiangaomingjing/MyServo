//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	ring net Communicaiton Packet Define		 				//
//	file				:	RingNetPacket.cpp											//
//	Description			:	use for pc using Rn communicaiton							//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/12/30	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PlotWave.h"
#include "RingNetPacket.h"
#include "RingNetCom.h"
#include "BaseReturn_def.h"

//test 

//#define SUBID_TEST

#ifdef SUBID_TEST
int16 test_txsubid[10000] = { 0 };
int16 test_rxsubid[10000] = { 0 };
int32 test_txsubIdindex = 0;
int32 test_rxsubIdindex = 0;
int16 test_protol0[1000] = { 0 };
int16 test_protol1[1000] = { 0 };
int32 test_protol0index = 0;
int32 test_protol1index = 0;
int16 test_dstid0[1000] = { 0 };
int16 test_dstid1[1000] = { 0 };
#endif // SUBID_TEST



//test end


//////////////////////////////////////////////////////////////////////////
extern HANDLE Com_Rcv_Event;                 //Event for RN receive 

CURRENTPACKETCMD g_cur_cmd_log;

//////////////////////////////////////////////////////////////////////////

CRnPacket::CRnPacket(void)
{
	com_sts		= false;
	pTx			= new RN_PACKET_FORMAT;
	pRx			= new RN_PACKET_FORMAT;
	for (int16 i = 0; i < 256;++i)
	{
		index[i] = 0xff;
	}
	
	init_pack();
}

CRnPacket::~CRnPacket(void)
{
	if (NULL != pTx)
		delete pTx;
	if (NULL != pRx)
		delete pRx;
}

void CRnPacket::init_pack()
{
	pTx->ttl				= 0;
	pTx->basecontrol		= PACKET_USER;// PACKET_USER = 4,
	//源id固定，目标id是目标站号，需要发送的时候设置
	pTx->src_id				= 0xf2;
	pTx->ttlbase			= 0xf0;
	//sum,需要发送的时候输入目标站号后才能够计算。
	pTx->protocolType0		= 0xff; //初始化为0xff，每次加1
	pTx->protocolType1		= 0xff;

	tx_length				= RN_PACKET_HEADER_LENTH;
	rx_length				= 0;

	g_cur_cmd_log.baseControl	= pTx->basecontrol;
	g_cur_cmd_log.src_id		= pTx->src_id;
}


//////////////////////////////////////////////////////////////////////////
//填写等环网包结构
//des_id 目标站号
//des_ch 目标通道，用于区分是哪个CM408
//pData  数据指针
//datalenth 数据长度，从实际数据开始 lenth是16bit的长度
//cmd 等环网命令，读还是写，什么方式读写 
//NeedReq 是否需要响应
//IsReq 是否是返回的请求
//FPGAaddr 操作FPGA的时候需要使用，操作dsp的时候不需要
//mode :mailbox 还是 pdu
//////////////////////////////////////////////////////////////////////////
void CRnPacket::fillPacket(int16 des_id, int16 des_ch,int16* pData, int16 datalenth, int16 cmd,int16 NeedReq,int16 IsReq,int16 FPGAaddr, int16 mode)
{
	int16 i, j;
	
	//------------------------------------------------------------------------
	tx_length							= RN_PACKET_HEADER_LENTH;
	
	//计算后面补零的个数
	int16 iAddZero						= RN_MIN_SEND_PKT_LEN - (RN_PACKET_HEADER_LENTH) - (datalenth*2);

	//获取目标id，也就是站号
	pTx->des_id							= (Uint8)des_id;

	//计算checksum
	pTx->sum							= (pTx->ttl + pTx->basecontrol + pTx->des_id + pTx->src_id + pTx->ttlbase);
	
	//包index自加
	pTx->protocolType0 = index[(Uint16)des_id];//*4+(Uint16)des_ch
	index[(Uint16)des_id]++; //* 4 + (Uint16)des_ch
#ifdef SUBID_TEST
	/*if (des_ch == 1)
	{
	test_dstid0[test_protol0index] = des_id;
	test_protol0[test_protol0index] = index[(Uint16)des_id * 4 + (Uint16)des_ch];
	test_protol0index++;
	if (test_protol0index >=1000)
	{
	test_protol0index = 0;
	}
	}
	else if (des_ch == 2)
	{
	test_dstid1[test_protol1index] = des_id;
	test_protol1[test_protol1index] = index[(Uint16)des_id * 4 + (Uint16)des_ch];
	test_protol1index++;
	if (test_protol1index >= 1000)
	{
	test_protol1index = 0;
	}
	}*/
#endif

	//目标通道，因为有两个CM408
	pTx->header.bit.des_ch				= des_ch;

	//因为是PC发送，因此发送源通道是PCI
	pTx->header.bit.src_ch				= RN_PCI_CH_ID;

	//pTx->header.bit.len_low			= ((datalenth*2 + RN_NUM2LENTH) & 0xff);
	//pTx->header.bit.len_bit8			= (((datalenth*2 + RN_NUM2LENTH) >> 8) & 0xff);
	
	pTx->header.bit.num_low				= ((datalenth*2) & 0xff);
	pTx->header.bit.num_bit8			= (((datalenth*2) >> 8) & 0xff);

	pTx->header.bit.cmd					= cmd;
	pTx->header.bit.delta				= RN_ADDR_INC;		//地址自增
	pTx->header.bit.resp_req			= NeedReq;			//是否需要应答，读必须要应答，写可有可无，
	pTx->header.bit.resp				= IsReq;			//该包是否是响应包
	pTx->header.bit.mode				= mode;				//是pdu还是mailbox

	pTx->header.bit.start_addr_bit7_0	= (FPGAaddr&0x00ff);
	pTx->header.bit.start_addr_bit15_8	= ((FPGAaddr>>8) & 0x00ff);
	for (i = 0, j = 0; i < (datalenth); i++)
	{
		pTx->dataSection[j] = ((*(pData + i)) & 0xff);
		pTx->dataSection[j + 1] = (((*(pData + i)) >> 8) & 0xff);
		j += 2;
		tx_length += 2;
	}
	//假如需要补零
	if (iAddZero >0)
	{
		for (i = 0; i < iAddZero; ++i)
		{
			pTx->dataSection[i + j] = 0x0;
			tx_length++;
		}
	}
	if (iAddZero >0)
	{
		pTx->header.bit.len_low = (((datalenth * 2) + RN_NUM2LENTH + iAddZero) & 0xff);
		pTx->header.bit.len_bit8 = ((((datalenth * 2) + RN_NUM2LENTH + iAddZero) >> 8) & 0xff);
	}
	else
	{
		pTx->header.bit.len_low = (((datalenth * 2) + RN_NUM2LENTH) & 0xff);
		pTx->header.bit.len_bit8 = ((((datalenth * 2) + RN_NUM2LENTH) >> 8) & 0xff);
	}


	//最后要发一个0xff
	pTx->dataSection[tx_length - RN_PACKET_HEADER_LENTH] = 0xff;
	tx_length++;
	//记录发送的命令，解析的时候用
	g_cur_cmd_log.des_id			= pTx->des_id;
	g_cur_cmd_log.des_channel		= pTx->header.bit.des_ch;
	g_cur_cmd_log.src_channel		= pTx->header.bit.src_ch;
	//假如是发送给fpga的，那么就不需要
	if (des_ch != RN_FPGA_CH_ID)
	{
		g_cur_cmd_log.cmd = ((pTx->dataSection[0] & 0x00ff) | ((pTx->dataSection[1] & 0x000f) << 8));
		g_cur_cmd_log.axis = ((pTx->dataSection[1] & 0x00f0) >> 4);

		//假如是通用指令，那么还需要记录ID
		if (g_cur_cmd_log.cmd == 41)
		{
			g_cur_cmd_log.cmdSubId = ((pTx->dataSection[6] & 0x00ff) | ((pTx->dataSection[7] << 8) & 0xff00));
#ifdef SUBID_TEST
			test_txsubid[test_txsubIdindex] = ((pTx->dataSection[6] & 0x00ff) | ((pTx->dataSection[7] << 8) & 0xff00));
			test_txsubIdindex++;
			if (test_txsubIdindex >= 10000)
			{
				test_txsubIdindex = 0;
			}
#endif // SUBID_TEST
		}		
	}

}

bool CRnPacket::checkComSts()
{
	if (com_sts == true)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
int16 CRnPacket::decoderPacket(int16* pData, int16 datalenth)
{
	if (rx_length < RN_MIN_PACKET_LEN)
		return RN_Rt_Receive_DLenth_short;

	Uint8 sum;
	int16 lenth;
	int16 dspId;
	//根据收到的不同类型
	switch (pRx->basecontrol & 0x0007)
	{
	//////////////////////////////////////////////////////////////////////////
	case PACKET_OFFSET_GEN:

		//检查checksum
		sum = pRx->ttl + pRx->basecontrol + pRx->des_id + pRx->src_id + pRx->ttlbase;
		if (sum == pRx->sum)
		{
			com_sts = true;
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	case PACKET_USER:	
		//接收到的命令分为两种情况，是发送命令的响应，还是别的站发过来请求数据的命令
		switch (pRx->header.bit.resp)
		{
		//////////////////////////////////////////////////////////////////////////
		//收到的不是响应
		case 0:
			//曲线部分
			lenth = ((pRx->header.bit.num_low & 0x00ff) | ((pRx->header.bit.num_bit8 << 8) & 0x0100));
			waveDataLenth = (lenth >> 1);
			memcpy_s(pData, (lenth)* sizeof(int8), &(pRx->dataSection[0]), (lenth)* sizeof(int8));
			//用于区分哪个dsp
			dspId = pRx->header.bit.src_ch;
			switch (dspId)
			{
				//DSPA
			case RN_DSP_CH_ID:
				g_plotWave->m_rn_noTimer->GetPlotWaveDataFromRn(GTSD_DSP_A);
				break;
				//DSPB
			case RN_PCI_CH_ID:
				g_plotWave->m_rn_noTimer->GetPlotWaveDataFromRn(GTSD_DSP_B);
				break;
			default:
				break;
			}
			break;
		//////////////////////////////////////////////////////////////////////////
		//收到的是响应
		case 1:
			//收到响应需要验证是否是发出去命令的响应。指令阻塞在哪里，等待这边收到返回后才能执行下一条指令
			if (g_cur_cmd_log.des_id == 0xff)
			{
				if ((pRx->des_id != g_cur_cmd_log.src_id)  /*|| (pRx->src_id!=g_cur_cmd_log.des_id)*/) //广播的时候需要注释后面部分代码
				{
					break;
				}
			}
			else
			{
				if ((pRx->des_id != g_cur_cmd_log.src_id) || (pRx->src_id != g_cur_cmd_log.des_id)/**/) //广播的时候需要注释后面部分代码
				{
					break;
				}
			}
			
			//检查checksum
			sum = pRx->ttl + pRx->basecontrol + pRx->des_id + pRx->src_id + pRx->ttlbase;
			if (sum != pRx->sum)
			{
				break;
			}
			if ((pRx->header.bit.des_ch != g_cur_cmd_log.src_channel) || (pRx->header.bit.src_ch != g_cur_cmd_log.des_channel))
			{
				break;
			}
			//假如返回的通道不是fpga的时候去判断命令和轴号等，还要查询返回值信息
			if (pRx->header.bit.src_ch != RN_FPGA_CH_ID)
			{
				if (((pRx->dataSection[0] & 0x00ff) | ((pRx->dataSection[1] & 0x000f) << 8)) != g_cur_cmd_log.cmd)
				{
					break;
				}
				if (((pRx->dataSection[1] & 0x00f0) >> 4) != g_cur_cmd_log.axis)
				{
					break;
				}
				//查询返回信息是否正确
				switch (pRx->dataSection[4])
				{
				case RN_NET_COM_EXECUTE_FAIL:
					return Net_Rt_Execute_Fail;

				case RN_NET_COM_EXECUTE_SUCCESS:
					break;

				case RN_NET_COM_PARAMETER_INVALID:
					return Net_Rt_Parameter_invalid;

				case RN_NET_COM_INSTRUCTION_INVALID:
					return Net_Rt_Instruction_invalid;

				default:
					return Net_Rt_Other_Error;
				}
				if (g_cur_cmd_log.cmd == 41)
				{
					if (g_cur_cmd_log.cmdSubId != ((pRx->dataSection[6] & 0x00ff) | ((pRx->dataSection[7]<< 8) & 0xff00)))
					{
						break;
					}
#ifdef SUBID_TEST
					test_rxsubid[test_rxsubIdindex] = ((pRx->dataSection[6] & 0x00ff) | ((pRx->dataSection[7] << 8) & 0xff00));
					test_rxsubIdindex++;
					if (test_rxsubIdindex >= 10000)
					{
						test_rxsubIdindex = 0;
					}
#endif // SUBID_TEST
					/*
					//test
					if ((g_cur_cmd_log.cmdSubId == 28) && (g_cur_cmd_log.cmd == 41))
					{
						Uint16 mytmp = ((pRx->dataSection[8] & 0x00ff) | ((pRx->dataSection[9] << 8) & 0xff00));
						if (mytmp != 7)
						{
							g_cur_cmd_log.cmdSubId = 28;
						}
						
					}
					*/
					//test end
				}
			}
		
			lenth = ((pRx->header.bit.num_low & 0x00ff)|((pRx->header.bit.num_bit8<<8)&0x0100));

			//dsp命令和fpga命令这里都从datasection[0]开始，在接口中FPGA从[0]开始，dsp从[6]开始
			memcpy_s(parsePacketCmd, (lenth)* sizeof(int8), &(pRx->dataSection[0]), (lenth)* sizeof(int8));

			//通知上层接收返回值成功
			SetEvent(Com_Rcv_Event);
			break;
		default:
			break;
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	default:
		break;
	}

	return Rt_Success;

}