//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	hex file operation		 									//
//	file				:	ihex.h														//
//	Description			:	use for change format from hex to ldr						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/11/21	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef _GTSD_IHEX_H_
#define _GTSD_IHEX_H_

// Error return values
#define IHEX_OK 0
#define IHEX_OPEN_FAILED 1
#define IHEX_INVALID_LINE 2
#define IHEX_INVALID_CHECKSUM 3
#define IHEX_UNKNOWN_FORMAT 4

#ifdef __cplusplus
#define LINKAGE_MOD extern "C"
#else
#define LINKAGE_MOD
#endif

typedef int (*IHEX_ReadCallback)(void *ctxt, const unsigned char *buf,
                                 unsigned long addr, unsigned long len);
LINKAGE_MOD int IHEX_ReadFile(FILE *fpHexFile, IHEX_ReadCallback pReadCallBack, void *ctxt);
LINKAGE_MOD int IHEX_CheckPathIsValidFile(const char *pcPath);

#endif
