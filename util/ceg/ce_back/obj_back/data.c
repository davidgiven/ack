#include <system.h>
#include <out.h>
#include "data.h"
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


put2(sect,addr,w)
char *sect;
long addr;
int w;
{
#ifdef BYTES_REVERSED
	put1(sect, addr, (w>>8));
	put1(sect, addr+1, w);
#else
	put1(sect, addr, w);
	put1(sect, addr+1, (w>>8));
#endif
}


put4(sect,addr,l)
char *sect;
long addr;
long l;
{
#ifdef WORDS_REVERSED
	put2(sect,addr,(int) (l>>16));
	put2(sect,addr+2,(int) l);
#else
	put2(sect,addr,(int) l);
	put2(sect,addr+2,(int) (l>>16));
#endif
}

int get2(sect,addr)
char *sect;
long addr;
{
#ifdef BYTES_REVERSED
	return (get1(sect,addr) << 8) | (get1(sect,addr+1) & 255);
#else
	return (get1(sect,addr+1) << 8) | (get1(sect,addr) & 255);
#endif
}

	
long get4(sect,addr)
char *sect;
long addr;
{
#ifdef WORDS_REVERSED
	return ((long)get2(sect,addr) << 16) | get2(sect, addr+2);
#else
	return ((long)get2(sect,addr+2) << 16) | get2(sect, addr);
#endif
}
