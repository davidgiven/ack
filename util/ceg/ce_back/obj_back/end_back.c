#include <out.h>
#include "mach.h"
#include "back.h"
#include "header.h"


end_back()
{
	sync();                 
	define_segments();  
	do_local_relocation();
}


sync()
{
	while ( ( text - text_area) % EM_WSIZE != 0 ) 
		text1( '\0');
	while ( ( data - data_area) % EM_WSIZE != 0 )
		con1( '\0');
}


static char *seg_name[] = {
		".text",
		".rom",
		".con",
		".bss"
	};


define_segments()
{
	int i, s;

	for ( s = SEGTXT; s <= SEGBSS; s++) {
		i = find_sym( seg_name[s], SYMBOL_DEFINITION);
		symbol_table[i].on_type = ( S_MIN + s) | S_SCT;
	}
}

