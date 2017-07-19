//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	hex2ldr		 												//
//	file				:	hex2ldr.h													//
//	Description			:	use for change format from out to ldr						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/11/18	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////

//
//hex2ldr parameter: [-a4][-d][-vta address] executablefile loaderfile
//
//where executablefile is the name of the ELF executable file
//loaderfile     is the name of the loader file
//
//option        description
//- a4           ensure quad byte alignment of boot block addresses and boot block sizes that are a multiple of 4
//- d            generate a legible dump of loader_file on stdout
//- vta address  specify the vector table address(comprising 8 hex digits)

//The boot blocks in loaderfile are limited to 3 types:
//
//-a normal boot block with a payload to load into memory
//- a FILL boot block used for zeroing memory
//- an IGNORE boot block for specifying the execution start address or
//vector table address
//
//An IGNORE boot block will be produced if the execution start address differs
//from the address of the first normal boot block or if option - vta has been
//specified.In this case the IGNORE boot block will be the first in the loader
//file.
//
//If the code at the execution start address is ARM code the start address will
//be even; if it is Thumb code the start address will be odd and is one greater
//than the address where execution is to start.

#ifndef _GTSD_HEX2LDR_
#define	_GTSD_HEX2LDR_

#include "Basetype_def.h"
#include "ServoDriverComDll.h"
/* We're only interested in executable files compiled for ARM
** processors. If it's any other kind of ELF file, we don't
** accept it.
*/

#define ET_EXEC    2
#define EM_ARM     0x28
#define ELFMAG0    0x7f
#define E_IDENT    "ELF"
#define EI_MAG0    0
#define EI_MAG1    1
#define EI_CLASS   4
#define ELFCLASS32 1

/* We're only interested in Program Segments that need to be loaded, because
** they're part of the application. Anything else will be ignored.
*/

#define PT_LOAD 1


/* The ARM Minimal Loader will generate boot blocks using the following
** flags in block_code:
**
**   bits     name          description
**   31:24    HDRSIGN       0xA9 (ARM Cortex M)
**   23:16    HDRCHK        Bytewise XOR checksum of the 16 byte security header
**      15    FINAL         if 1 the final block in the boot stream
**      14    FIRST         if 1 the first block in the boot stream
**      12    IGNORE        if 1 the block has no payload
**       8    FILL          if 1 a memory fill block with 32 bit value in argument
*/

#define FINAL (1<<15)
#define FIRST (1<<14)
#define IGNORES (1<<12)
#define FILL  (1<<8)
#define HDRCHK 16
#define HDRSIGN (0xAD << 24)

//return value
#define LOADER_OK    0
#define LOADER_ERROR 1

/* ELF HEADER
** The ELF Header indicates the kind of file, and also provides
** the offsets to the Program Segment table, which we use to
** emit the loadable segments.
*/

typedef unsigned int       elf32_addr_type;
typedef unsigned short int elf32_half_type;
typedef unsigned int       elf32_off_type;
typedef unsigned int       elf32_word_type;

static const int16 EI_NIDENT = 16;
typedef struct 
{
	Uint8   e_ident[EI_NIDENT];
	elf32_half_type e_type;
	elf32_half_type e_machine;
	elf32_word_type e_version;
	elf32_addr_type e_entry;
	elf32_off_type  e_phoff;
	elf32_off_type  e_shoff;
	elf32_word_type e_flags;
	elf32_half_type e_ehsize;
	elf32_half_type e_phentsize;
	elf32_half_type e_phnum;
	elf32_half_type e_shentsize;
	elf32_half_type e_shnum;
	elf32_half_type e_shstrndx;
} elf32_header_type;


/* PROGRAM HEADER
** The Program Header points to the Program Segments that the application needs
** to load. These segments tell us which parts of the overall ELF file need to
** be converted into a boot stream.
*/

typedef struct 
{
	elf32_word_type p_type;
	elf32_off_type  p_offset;
	elf32_addr_type p_vaddr;
	elf32_addr_type p_paddr;
	elf32_word_type p_filesz;
	elf32_word_type p_memsz;
	elf32_word_type p_flags;
	elf32_word_type p_align;
} program_header_type;

typedef program_header_type * program_header_pointer_type;

/* BOOT BLOCK HEADER
** We write out boot block headers as part of the .ldr boot stream.
*/

typedef struct 
{
	unsigned int block_code;
	unsigned int target_addr;
	unsigned int byte_count;
	unsigned int argument;
} boot_block_header_type;

// function declaration
int compare_program_header_addr(const void *v1, const void *v2);

class CHex2ldr
{
public:
	CHex2ldr();
	~CHex2ldr();

	int32 gen_header;
	int32 column; 
	int32 max_columns;
	int32 header_bytes;

	elf32_header_type elf32_header;
	program_header_type *program_header_table;
	elf32_word_type num_program_headers;
	Uint32 first_addr, start_addr;
	program_header_pointer_type* program_header_pointer_list;
	Uint32 program_header_pointer_list_size;
	Uint8 *segment_block;
	elf32_word_type segment_block_size;


	int16 write_stream(void* data, Uint32 size, fstream* fd);
	void close_files(fstream* hex, fstream* ldr);
	int16 read_segment(fstream* fd, program_header_type *ph, Uint8 **block, elf32_word_type *blocksize);
	int16 read_elf_header(fstream* hex, elf32_header_type* elf32_header);
	int16 is_ARM_exec_file(elf32_header_type* elf32_header);
	int16 read_program_header(fstream* hex, elf32_header_type* elf32_header, program_header_type** program_header_table, elf32_word_type* num_program_headers);
	int16 write_addr_boot_block(fstream* ldr, Uint32 addr);
	void  complete_block_code(boot_block_header_type *p, Uint32 boot_block_num, Uint32 boot_blocks);
	int16 write_boot_block(fstream* fd, program_header_type *ph, Uint8 *block, elf32_word_type blocksize, Uint32 boot_block_num,Uint32 boot_blocks, Uint32 align, Uint32 *addr, Uint32 *size);
	int16 write_fill_boot_block(fstream* fd, program_header_type *ph, Uint32 boot_block_num, Uint32 boot_blocks, Uint32 align, Uint32 addr, Uint32 size);
	
	
	int16 hex2ldr_execute(string hexName, string ldrName, string vtaAddr);
};
extern CHex2ldr* g_hex2ldr;

#endif