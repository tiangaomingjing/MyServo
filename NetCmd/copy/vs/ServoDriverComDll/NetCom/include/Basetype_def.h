//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	base type define		 									//
//	file				:	basetype_def.h												//
//	Description			:	use for Improving the portability of program				//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/1/20	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef		__GTSD_BASE_TYPE_DEFINE__
#define		__GTSD_BASE_TYPE_DEFINE__

//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//									BASIC DATA TYPE DEFINITION
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
typedef		char							int8;
typedef		short							int16;
typedef		long							int32;
typedef		long long						int64;

typedef		unsigned char					Uint8;
typedef		unsigned short					Uint16;
typedef		unsigned long					Uint32;
typedef		unsigned long long				Uint64;
//------------------------------------------------------------------------------------------------
typedef		volatile signed char			vint8;
typedef		volatile short					vint16;
typedef		volatile long					vint32;

typedef		volatile unsigned char			vUint8;
typedef		volatile unsigned short			vUint16;
typedef		volatile unsigned long			vUint32;
//------------------------------------------------------------------------------------------------
typedef		const	Uint8					CUint8;
typedef		const	Uint16					CUint16;
typedef		const	Uint32					CUint32;
//------------------------------------------------------------------------------------------------

#endif