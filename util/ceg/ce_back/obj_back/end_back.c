#include <out.h>
#include "mach.h"
#include "back.h"
#include "header.h"

static
finish_tables()

/* Prepare tables for do_local_relocation() and output().
 */
{
	register struct outname *np = symbol_table;
	register int i = nname;

	for (; i; i--, np++) {
		if ((np->on_type & S_COM) && ! (np->on_type & S_EXT)) {
                        long sz = np->on_valu;

                        switchseg(SEGBSS);
                        align_word();
                        np->on_type &= (~S_COM);
                        np->on_valu = cur_value();
                        bss(sz);
                }
	}
	while ( ( text - text_area) % EM_WSIZE != 0 ) 
		text1( '\0');
	while ( ( data - data_area) % EM_WSIZE != 0 )
		con1( '\0');
	define_segments();  
}

static char *seg_name[] = {
		".text",
		".rom",
		".con",
		".bss"
	};

static
define_segments()
{
	int i, s;

	for ( s = SEGTXT; s <= SEGBSS; s++) {
		i = find_sym( seg_name[s], SYMBOL_DEFINITION);
		symbol_table[i].on_type = ( S_MIN + s) | S_SCT;
	}
}

end_back()
{
	finish_tables();                 
	do_local_relocation();
}
