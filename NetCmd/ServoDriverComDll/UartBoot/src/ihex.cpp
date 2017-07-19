//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	hex file operation		 									//
//	file				:	ihex.cpp													//
//	Description			:	use for change format from hex to ldr						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/11/21	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "ihex.h"

// General constants                            
#define INVALID_ADDR_MARK                       0xFFFFFFFFUL

// Intel Hex constants
#define IHEX_DATA_RECORD                        0x000
#define IHEX_END_OF_FILE_RECORD                 0x001
#define IHEX_EXTENDED_SEGMENT_ADDRESS_RECORD    0x002
#define IHEX_START_SEGMENT_ADDRESS_RECORD       0x003
#define IHEX_EXTENDED_LINEAR_ADDRESS_RECORD     0x004
#define IHEX_START_LINEAR_ADDRESS_RECORD        0x005
#define IHEX_COLON_POS                          0x000
#define IHEX_MAX_LINE_DATA                      0x100
#define IHEX_MAX_LENGTH                         0x200

#define MAKE_UCHAR(LOW_NIBBLE,HIGH_NIBBLE)  (unsigned char)((((unsigned char)LOW_NIBBLE) | (((unsigned char)HIGH_NIBBLE)<<4)))
#define MAKE_UINT(LOW_UBYTE,HIGH_UBYTE)  (((unsigned short int)LOW_UBYTE) | (((unsigned short int)HIGH_UBYTE)<<8))
#define MAKE_ULONG(LOW_WORD, HIGH_WORD) ((unsigned long)(((unsigned short int)(LOW_WORD)) | (((unsigned long)((unsigned short int)(HIGH_WORD))) << 16)))


// Function    : HexToChar
// Decription  : Convert hexadecimal ASCII to binary
unsigned char HexToChar(unsigned char ucHexVal)
{
   if ((ucHexVal >= '0') && (ucHexVal <= '9'))
      return(unsigned char)(ucHexVal - '0');
   else if ((ucHexVal >= 'A') && (ucHexVal <= 'F'))
      return(unsigned char)(ucHexVal - 'A' + 10);
   else
      return(unsigned char)(ucHexVal - 'a' + 10);
}

// Function     : IHEX_ReadFile
// Description  : Read an intel hex file
int IHEX_ReadFile(FILE *fpHexFile, IHEX_ReadCallback pReadCallBack, void *ctxt)
{
   char           szInBuffer    [IHEX_MAX_LENGTH];
   unsigned char  pszDataBuffer [IHEX_MAX_LINE_DATA];
   unsigned char  ucCheckSum;
   unsigned long  ulBaseAddress = 0x0;
   unsigned char  ucRecordType;
   unsigned long  ulCount;
   unsigned long  ulAddress;
   unsigned int   i;
   unsigned long  ulLastAddress = INVALID_ADDR_MARK;
   unsigned long  ulLastCount   = 0x0;

   // Process the files
   while (!ferror(fpHexFile) && !feof(fpHexFile))
      {
      if (fgets(szInBuffer,sizeof(szInBuffer),fpHexFile))
         {
         // Check for empty line
         if ((strlen(szInBuffer) == 0) ||
             (szInBuffer[0] == '\r') ||
             (szInBuffer[0] == '\n'))
            continue;

         if (szInBuffer[IHEX_COLON_POS] != ':')
            {
            return IHEX_INVALID_LINE;
            }
         // Right, let's first check the checksum...
         ucRecordType = MAKE_UCHAR(HexToChar(szInBuffer[8]), HexToChar(szInBuffer[7]));
         ulCount      = MAKE_UCHAR(HexToChar(szInBuffer[2]), HexToChar(szInBuffer[1]));
         ulAddress    = MAKE_UINT(MAKE_UCHAR(HexToChar(szInBuffer[6]), HexToChar(szInBuffer[5])),
                                  MAKE_UCHAR(HexToChar(szInBuffer[4]), HexToChar(szInBuffer[3])));
         ucCheckSum = 0;
         // Add the data + checksum...
         for (i=0; i <= ulCount; i++)
            ucCheckSum += MAKE_UCHAR(HexToChar(szInBuffer[10+i*2]), HexToChar(szInBuffer[9+i*2]));
         ucCheckSum += ucRecordType;
         ucCheckSum += (unsigned char)ulCount;
         ucCheckSum += (unsigned char)ulAddress;
         ucCheckSum += (unsigned char)(ulAddress >> 8);

         if (ucCheckSum != 0)
            {
            return IHEX_INVALID_CHECKSUM;
            }

         switch (ucRecordType)
            {
            case IHEX_DATA_RECORD:
               // We need to output an plain binary address record in some cases
               ulLastAddress = ulAddress + ulBaseAddress;
               ulLastCount   = ulCount;

               // extract the data bytes 
               for (i=0; i<ulCount; i++)
                  pszDataBuffer[i] = MAKE_UCHAR(HexToChar(szInBuffer[10+i*2]), HexToChar(szInBuffer[9+i*2]));

               // Output
               if (pReadCallBack) {
                  int err = pReadCallBack(ctxt, pszDataBuffer,ulAddress + ulBaseAddress,ulCount);
                  if (err)
                     return err;
               }
               break;

            case IHEX_EXTENDED_SEGMENT_ADDRESS_RECORD:
               ulBaseAddress = MAKE_UINT(MAKE_UCHAR(HexToChar(szInBuffer[12]), HexToChar(szInBuffer[11])),
                                         MAKE_UCHAR(HexToChar(szInBuffer[10]), HexToChar(szInBuffer[ 9])));
               ulBaseAddress <<= 4;
               break;
            case IHEX_EXTENDED_LINEAR_ADDRESS_RECORD:
               ulBaseAddress = MAKE_UINT(MAKE_UCHAR(HexToChar(szInBuffer[12]), HexToChar(szInBuffer[11])),
                                         MAKE_UCHAR(HexToChar(szInBuffer[10]), HexToChar(szInBuffer[ 9])));
               ulBaseAddress <<= 16;
               break;
            case IHEX_END_OF_FILE_RECORD:
            case IHEX_START_SEGMENT_ADDRESS_RECORD:
            case IHEX_START_LINEAR_ADDRESS_RECORD:
               // Ignore these !
               break;
            default:
               return IHEX_UNKNOWN_FORMAT;
            }
         }
      } 
   return IHEX_OK;
}

// Function     : IHEX_CheckPathIsValidFile
// Description  : Check whether path points to correctly formed intel hex file
int IHEX_CheckPathIsValidFile(const char *pcPath)
{
	FILE *fpFile = NULL;
	fopen_s(&fpFile,pcPath, "r");
   if (fpFile)
      {
      int result = IHEX_ReadFile(fpFile, NULL, NULL);
      fclose(fpFile);
      return result;
      }
   else
      return IHEX_OPEN_FAILED;
}
