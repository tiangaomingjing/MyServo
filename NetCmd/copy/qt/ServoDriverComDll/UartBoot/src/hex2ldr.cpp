//////////////////////////////////////////////////////////////////////////////////////////
//	summary				:	hex2ldr		 												//
//	file				:	hex2ldr.cpp													//
//	Description			:	use for change format from out to ldr						//
//	lib					:	none														//
//																						//
//======================================================================================//
//		programmer:		|	date:		|	Corporation:	|		copyright(C):		//
//--------------------------------------------------------------------------------------//
//		wang.bin(1420)  |	2016/11/18	|	googoltech		|		2016 - 2019			//
//--------------------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

#include "hex2ldr.h"

CHex2ldr* g_hex2ldr = NULL;

int compare_program_header_addr(const void *v1, const void *v2)
{
	/* Compare the start addresses of the segments with headers v1 and v2.
	** This function is used by qsort.
	*/

	program_header_pointer_type *a = (program_header_pointer_type *)v1;
	program_header_pointer_type *b = (program_header_pointer_type *)v2;

	if (((*a)->p_vaddr) > ((*b)->p_vaddr))
	{
		return 1;
	}
	else if (((*a)->p_vaddr) < ((*b)->p_vaddr))
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
CHex2ldr::CHex2ldr()
{
	gen_header = false;
	column = 0;
	max_columns = 3;
	header_bytes = 0;
}
CHex2ldr::~CHex2ldr()
{

}
int16 CHex2ldr::read_elf_header(fstream* hex, elf32_header_type* elf32_header)
{
	/* Read the ELF HEADER from file fd into elf32_header returning 1 if the
	** operation is successful or 0 if unsuccessful.
	** return 0 means read the end of the file 
	*/
	if ((*hex).read((char*)(elf32_header), sizeof(elf32_header_type)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int16 CHex2ldr::is_ARM_exec_file(elf32_header_type *elf32_header)
{
	/* If the file with elf32 header elf32_header is a valid ARM executable file
	** that the ARM Minimal Loader is able to process, return 1 else return 0.
	*/
	return ((elf32_header->e_ident[EI_MAG0] == ELFMAG0) &&
		(strncmp((const char*)(&elf32_header->e_ident[EI_MAG1]), E_IDENT, 3) == 0) &&
		(elf32_header->e_ident[EI_CLASS] == ELFCLASS32) &&
		(elf32_header->e_machine == EM_ARM) &&
		(elf32_header->e_type == ET_EXEC));
}
int16 CHex2ldr::read_program_header(fstream* hex, elf32_header_type* elf32_header, program_header_type** program_header_table, elf32_word_type* num_program_headers)
{
	/* Read the PROGRAM HEADER from file fd into *program_header_table and
	** return the number of segments in the table in *num_program_headers.
	** Return 1 if the operation is successful or 0 if unsuccessful.
	**
	** The overall ELF header gives both the number of Program Headers in
	** the table, and the size of a Program Header, in bytes.
	** (Note: Program Headers give info about the Program Segment.
	*/
	int32 size = elf32_header->e_phnum * elf32_header->e_phentsize;

	*program_header_table = (program_header_type *)malloc(size);
	*num_program_headers = elf32_header->e_phnum;
	if (program_header_table) 
	{
		(*hex).seekg(elf32_header->e_phoff,ios::beg);
		if ((*hex).read((char*)(*program_header_table), size))
		{
			return 1;
		}
		else
		{
			return 0;
		}	
	}
	else
	{
		return 0;
	}
}
int16 CHex2ldr::write_addr_boot_block(fstream* ldr, Uint32 addr)
{
	/* Write the first boot block that specifies the execution start address or
	** vector table address addr. Return 1 if the operation is successful or
	** 0 if not.
	*/

	boot_block_header_type h;
	h.block_code = IGNORES;
	h.byte_count = 0;
	h.argument = 0;
	h.target_addr = addr;

	/* Compute the checksum for the block. */
	complete_block_code(&h, 1, 0);

	/* Write out the block's contents. There's no corresponding payload for this block. */
	int16 rtn  = write_stream((void*)(&h),sizeof(boot_block_header_type),ldr);
	
	return rtn;
}
void CHex2ldr::complete_block_code(boot_block_header_type *p, Uint32 boot_block_num, Uint32 boot_blocks)
{
	/* Complete field block_code of boot block header p for boot block
	** number boot_block_num.
	*/

	int i;
	unsigned char checksum = 0;
	unsigned char mask = 0xff;
	unsigned char *pc = (unsigned char *)p;

	p->block_code |= HDRSIGN;
	if (boot_block_num == 1) {
		p->block_code |= FIRST;
	}
	if (boot_block_num == boot_blocks) {
		p->block_code |= FINAL;
	}

	p->block_code &= (~(mask << HDRCHK));
	for (i = 0; i < sizeof(boot_block_header_type); i++) {
		checksum ^= pc[i];
	}
	p->block_code |= (checksum << HDRCHK);
}
int16 CHex2ldr::write_stream(void* data, Uint32 size, fstream* fd)
{
	/* Write the indicated data to the boot stream.
	** Return 1 if successful, and 0 if there's a problem.
	*/
	if (gen_header) 
	{
		unsigned int i;
		char *ptr = (char *)data;
		for (i = 0; i < size; i++) 
		{
			const char *fmt;
			if (header_bytes == 0) 
			{
				fmt = "0x%02x";
				(*fd) << "0x";
				(*fd) << setw(2) << hex <<setfill('0') << (ptr[i] & 0xff);
			}
			else if (column == max_columns) 
			{
				fmt = ",\n0x%02x";
				(*fd) <<","<<endl;
				(*fd) << "0x";
				(*fd) << setw(2) << setfill('0')<< hex << (ptr[i] & 0xff);
				column = 0;
			}
			else
			{
				fmt = ", 0x%02x";
				(*fd) << ", ";
				(*fd) << "0x";
				(*fd) << setw(2) << setfill('0') << hex << (ptr[i] & 0xff);
				column += 1;
			}
			
			header_bytes++;
		}
		return 1;
	}
	else 
	{
		(*fd).write((char*)data, size);
		return 1;
	}
}
int16 CHex2ldr::read_segment(fstream* fd,program_header_type *ph,Uint8 **block,elf32_word_type *blocksize)
{
	/* Read segment ph from file fd returning the block of values in *block and the
	** block size in *block. Return 1 if the operation is successful or 0 if unsuccessful.
	*/
	*blocksize = ph->p_filesz;
	*block = (unsigned char *)malloc(*blocksize);

	if (*block) {
		(*fd).seekg(ph->p_offset, ios::beg);
		if ((*fd).read((char*)(*block), (*blocksize)))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else 
	{
		return 0;
	}
}
int16 CHex2ldr::write_boot_block(fstream* fd,program_header_type *ph,Uint8 *block,elf32_word_type blocksize,Uint32 boot_block_num,Uint32 boot_blocks,Uint32 align,Uint32 *addr,Uint32 *size)
{
	/* Write normal boot block number boot_block_num for segment ph with payload block
	** of size blocksize bytes. Returns 1 if the operation is successful or 0 if not.
	*/

	/* Boot streams should consist of blocks that are multiples of four bytes in length,
	** and be aligned on a four-byte boundary, because of constraints in existing Boot
	** kernels. If a segment does not obey these constraints, then we'll need to adjust
	** either the starting address or the length (or both). In such cases, we'll need to
	** write out some padding bytes, so declare those here.
	*/
	static unsigned char zeroes[4] = { 0, 0, 0, 0 };

	boot_block_header_type h;
	unsigned int target_addr_adj, byte_count_adj;
	unsigned int good;

	h.block_code = 0;
	h.byte_count = ph->p_filesz;
	h.argument = 0;
	h.target_addr = ph->p_vaddr;

	/* Ensure target address is four-byte aligned. We adjust the starting
	** address downwards to the boundary. Note that this will be problematic
	** if there are contiguous segments with a non-aligned boundary.
	*/

	target_addr_adj = align ? (h.target_addr % 4) : 0;
	h.target_addr -= target_addr_adj;
	*addr = h.target_addr;

	/* Ensure bytecount is a multiple of four. We adjust upwards to the next
	** boundary.
	*/

	h.byte_count += target_addr_adj;
	byte_count_adj = align ? h.byte_count % 4 : 0;
	if (byte_count_adj) {
		byte_count_adj = (4 - byte_count_adj);
		h.byte_count += byte_count_adj;
	}
	*size = h.byte_count;

	/* After updating the key attributes of the block, finalize the
	** block, setting flags and computing the checksum.
	*/
	complete_block_code(&h, boot_block_num, boot_blocks);

	/* Emit the block header itself. */
	good = (write_stream((void *)(&h), sizeof(boot_block_header_type), fd) == 1);

	/* If we've adjusted the start address downwards, write out padding to take us
	** back up to the original start address.
	*/
	if (good && target_addr_adj) {
		good = (write_stream((void *)(&zeroes[0]), target_addr_adj, fd) == 1);
	}
	/* Write out the block's payload. */
	if (good) {
		good = (write_stream((void *)(block), blocksize, fd) == 1);
	}
	/* If we've adjusted the size upwards, write out padding to take us to the
	** new size.
	*/
	if (good && byte_count_adj) {
		good = (write_stream((void *)(&zeroes[0]), byte_count_adj, fd) == 1);
	}
	return good;
}
int16 CHex2ldr::write_fill_boot_block(fstream* fd,program_header_type *ph,Uint32 boot_block_num,Uint32 boot_blocks,Uint32 align,Uint32 addr,Uint32 size)
{
	/* Write fill boot block number boot_block_num for segment ph. Return 1 if
	** the operation is successful or 0 if not.
	*/

	boot_block_header_type h;
	unsigned int adj;

	/* We're emitting a new block that occupies the space that's not
	** taken up by any payload in the segment.
	*/
	h.block_code = FILL;
	h.byte_count = (ph->p_memsz - size);

	/* Adjust the length upwards to the next four-byte boundary, if the
	** length is not a multiple of four.
	*/
	adj = align ? h.byte_count % 4 : 0;
	if (adj) {
		h.byte_count += (4 - adj);
	}
	h.argument = 0; /* fill value */
	h.target_addr = addr + size;

	/* Set remaining flags on the block and compute the checksum. */
	complete_block_code(&h, boot_block_num, boot_blocks);

	/* Write out the fill block's header. Since it's a fill block, there's
	** no corresponding payload to write out.
	*/
	return write_stream((void *)(&h), sizeof(boot_block_header_type), fd);
}
void CHex2ldr::close_files(fstream* hex, fstream* ldr)
{
	if (!(*hex)) 
	{
		(*hex).close();
	}
	if (!(*ldr))
	{
		(*ldr).close();
	}
}
int16 CHex2ldr::hex2ldr_execute(string hexName, string ldrName, string vtaAddr)
{
	fstream Hexfile;
	fstream ldrfile;
	//open hex file 
	
	Hexfile.open(hexName.c_str(), ios::in | ios::out | ios::binary);
	if (!Hexfile)
	{
		return LOADER_ERROR;
	}

	//open ldr file
	//如果文件不存在，不能用in这种模式
	ldrfile.open(ldrName.c_str(), ios::out | ios::binary);
	if (!ldrfile)
	{
		close_files(&Hexfile, &ldrfile);
		return LOADER_ERROR;
	}

	//read hex file header
	
	if (!read_elf_header(&Hexfile, &elf32_header))
	{
		close_files(&Hexfile, &ldrfile);
		return LOADER_ERROR;
	}
	//judge if the file is the arm file
	if (!is_ARM_exec_file(&elf32_header)) 
	{
		close_files(&Hexfile, &ldrfile);
		return LOADER_ERROR;
	}

	//read program header
	if (!read_program_header(&Hexfile, &elf32_header, &program_header_table, &num_program_headers))
	{
		close_files(&Hexfile, &ldrfile);
		return LOADER_ERROR;
	}

	/* Get executable start address. */
	start_addr = elf32_header.e_entry;

	Uint32 i;
	Uint32 n, num_boot_blocks;   /* n is just for whether we need an extra boot block at the start */
	/* Construct the list of loadable segments and count the number of boot blocks that will be written. */

	num_boot_blocks = 0;
	program_header_pointer_list_size = 0;
	program_header_pointer_list =(program_header_pointer_type *)malloc(num_program_headers * sizeof(program_header_pointer_type));

	for (i = 0; i < num_program_headers; i++) 
	{
		program_header_type *ph = &program_header_table[i];
		/* We're only interested in Program Segments of LOAD type. */
		if (ph->p_type == PT_LOAD)
		{
			program_header_pointer_list[program_header_pointer_list_size++] = ph;
			num_boot_blocks++;
			/* If the Program Segment has a larger in-memory size than in the exec file,
			** we'll emit two blocks - a normal block for the payload from the file,
			** and a fill block for the remainder.
			*/
			if (ph->p_memsz > ph->p_filesz)
			{
				num_boot_blocks++;
			}
		}
	}
	//if the size is zero 
	if (program_header_pointer_list_size == 0) 
	{
		close_files(&Hexfile, &ldrfile);
		return LOADER_ERROR;
	}
	//sort the addr
	if (program_header_pointer_list_size > 1) {
		/* Ensure loadable program headers are in order of increasing memory address. */
		qsort(program_header_pointer_list, program_header_pointer_list_size,
			sizeof(program_header_pointer_type),(compare_program_header_addr));
	}
	Uint32 align = true;
	Uint32 vector_table = false;
	Uint32 vector_table_addr;
	Uint32 good = true;
	Uint32 addr, size;
	gen_header = false;

	good = ((strlen(vtaAddr.c_str()) == 8) && (sscanf_s(vtaAddr.c_str(), "%08x", &vector_table_addr,sizeof(vtaAddr)) == 1));
	if (good)
	{
		vector_table = true;
	}
	else
	{
		vector_table = false;
	}

	first_addr = program_header_pointer_list[0]->p_vaddr;
	if ((first_addr != start_addr) || vector_table) {
		/* We'll emit an extra, ignore-type block at the start of the stream to
		** specify the start address.
		*/
		num_boot_blocks++;
		n = 1;
	}
	else {
		n = 0;
	}
	/* Write the boot blocks. Start with the block that indicates the entry
	** point for the overall executable.
	*/

	if (n)
	{
		write_addr_boot_block(&ldrfile, (vector_table ? vector_table_addr : start_addr));
	}

	for (i = 0; i < program_header_pointer_list_size; i++) 
	{

		program_header_type *ph = program_header_pointer_list[i];

		/* Read in each program segment header, and then write out a corresponding
		** boot block to the boot stream.
		*/
		if (!read_segment(&Hexfile, ph, &segment_block, &segment_block_size)) 
		{
			close_files(&Hexfile, &ldrfile);
			return LOADER_ERROR;
		}

		if (!write_boot_block(&ldrfile, ph, segment_block, segment_block_size, ++n, num_boot_blocks,align, &addr, &size)) 
		{
			close_files(&Hexfile, &ldrfile);
			return LOADER_ERROR;
		}

		/* Write the extra fill block if the Program Segment's file content is smaller
		** than its in-memory space.
		*/
		if (ph->p_memsz > size) {
			if (!write_fill_boot_block(&ldrfile, ph, ++n, num_boot_blocks, align, addr, size)) {
				close_files(&Hexfile, &ldrfile);
				return LOADER_ERROR;
			}
		}
		free(segment_block);

	}

	/* Clean up. */

	free(program_header_table);
	free(program_header_pointer_list);
	close_files(&Hexfile, &ldrfile);
	return LOADER_OK;
}
