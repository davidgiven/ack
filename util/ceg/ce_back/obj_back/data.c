#include <system.h>
#include <out.h>
#include "mach.h"

/* Global datastructures : 
 * - 'text_area' points to the text segment, 'text' points to first free
 *    entry in the text segment.
 * - 'data_area' points to the data segment, 'data' points to the first free
 *    entry in the data segmnet.
 * - 'string_area' points to the string area, 'string' points to the first free
 *    entry in the string area.
 * - 'reloc_info' points to the relocation table, 'relo' points to the first 
 *    free entry in the relocation table.
 * - 'symbol_table' points to the symbol table, 'nname' is the index of the 
 *    first free entry in the symbol_table. If pointers were used it is a pain
 *    to do a realloc on the symbol table, because all pointers in the 
 *    relocation table to the symbol table have to be updated.
 * - The bss segment contains only one vaue, so its enough to count the
 *   the bytes wanted.
 * - The 'size_*' variables count the number of entries in each segment.
 * - 'cur_seg' contains the number of the segment to be filled.
 *   (see "back.h")
 */


char 		*text_area,
     		*data_area,
     		*string_area,
		*text, *data, *string;
struct outrelo 	*reloc_info, *relo;
struct outname 	*symbol_table;


int 		cur_seg = -1 , nname = 0;
long 		nbss = 0, size_text, size_data, size_reloc, size_symbol,
		size_string, _text_cnt, _data_cnt;


put1(sect,addr,b)
char *sect;
long addr;
char b;
{
	sect[addr] = b;
}


put2(sect,addr,w)
char *sect;
long addr;
int w;
{
#ifdef BYTES_REVERSED
	put1(sect,addr,(char) (w>>8));
	put1(sect,addr+1,(char) w);
#else
	put1(sect,addr,(char) w);
	put1(sect,addr+1,(char) (w>>8));
#endif
}


put4(sect,addr,l)
char *sect;
long addr;
long l;
{
#ifdef WORDS_REVERSED
	put2(sect,addr,(short) (l>>16));
	put2(sect,addr+2,(short) l);
#else
	put2(sect,addr,(short) l);
	put2(sect,addr+2,(short) (l>>16));
#endif
}


char get1( sect, addr)
char *sect;
long addr;
{
	return( sect[addr]);
}


short get2(sect,addr)
char *sect;
long addr;
{
	short h,l;

#ifdef BYTES_REVERSED
	h = sect[addr];
	l = sect[addr+1];
#else
	l = sect[addr];
	h = sect[addr+1];
#endif
	return( ( h << 8) | ( l & 255));
}

	
long get4(sect,addr)
char *sect;
long addr;
{
	long l,h;

#ifdef WORDS_REVERSED
	h = get2(sect,addr);
	l = get2(sect,addr+2);
#else
	l = get2(sect,addr);
	h = get2(sect,addr+2);
#endif
	return( ( h << 16) | ( l & 65535L));
}
