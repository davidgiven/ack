#include <system.h>
#include <out.h>
#include "mach.h"


char 		*text_area,
     		*data_area,
     		*string_area,
		*text, *data, *string;
struct outrelo 	*reloc_info, *relo;
struct outname 	*symbol_table;


int 		cur_seg = -1 , nname = 0;
long 		nbss = 0, size_text, size_data, size_reloc, size_symbol,
		size_string;


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
