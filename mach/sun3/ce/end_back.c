#include <out.h>
#include "mach.h"
#include "back.h"
#include "header.h"

static finish_tables();

end_back()
{
	finish_tables();                 
	do_local_relocation();
	output_back();
}


static
finish_tables()
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
}
